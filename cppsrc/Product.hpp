#pragma once

#include <string>
#include <set>
#include "Review.hpp"
#include <memory>
#include <unordered_map>
#include <cassert>
#include "Memoize.hpp"
#include "StringUtils.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

class Review;

class Product;
typedef std::shared_ptr<Product> Product_p;

class Product : public Memoizeable<Product_p> {
public:
	const std::string productID;
	const std::string title;
	const double price;
	std::set<Review*> reviews;
	const int id;
	
	
private:
	static std::unordered_map<std::string, std::shared_ptr<Product> >& lookup() {
		static std::unordered_map<std::string, std::shared_ptr<Product> > lookup;
		return lookup;
	}
	
	static int& idr() {static int idr = 0; return idr;}
	
	
	Product(const std::string &productID, const std::string &title, const double &price)
		:productID(productID),title(title),price(price),id(++(idr())){
		assert(lookup().find(productID) == lookup().end());
	}

	Product(int id, const std::string &productID, const std::string &title, const double &price)
		:productID(productID),title(title),price(price),id(id) {
		assert(id > idr()); //ID collision is technically possible.
		idr() += (id - idr() + 1);
	}

	
public: 
	
	
	class Memo : public ::Memo<Product_p> {
  	public:
  		bool serialize_called = false;
  		const bool from_const = false;
  		int id = -1;
  		std::string productID;
  		std::string title;
  		double price;
  		friend class Product;
  		Memo(int id, std::string p, std::string t, double pr)
			:from_const(true),id(id),productID(p),title(t),price(pr){}
		Memo(){}
		
  		Product_p unpack() const { 
			assert(serialize_called);
			assert(id != -1);
			static std::unordered_map<int, Product_p> pm;
			if (pm.find(id) != pm.end()) return pm.at(id);
			if (failnow) assert(false && "Product not in set?");
			Product_p ret(new Product(id,productID, title, price));
			lookup()[productID] = ret;
			pm[id] = ret;
			return ret;
		}

		bool operator<(const Memo &o) const {return id < o.id; }

		bool from_archive() const {return serialize_called && (!from_const); }
		bool from_pack() const {return from_const;}
		
  		template<class Archive>
  		void serialize(Archive &ar, const unsigned int /*version*/) {
  			ar & boost::serialization::base_object<::Memo<Product_p> > (*this);
  			ar & id;
  			ar & productID;
  			ar & title;
  			ar & price;
  			serialize_called = true;
  			assert(id != -1);
  		}
	};
	
  	friend class Memo;
	
  	Memo_p pack() const { return Memo_p(new Memo(id,productID,title,price));}
  	Memo pod_pack() const { return Memo(id,productID,title,price);}
	
	static std::shared_ptr<Product> build(const std::string &productID, const std::string &title, double price){
		if (lookup().find(productID) != lookup().end()) return lookup().at(productID);
		std::shared_ptr<Product> p( new Product(productID, title, price));
		lookup()[productID] = p;
		return p;
	}
	
	static Product_p build(const std::string &productID){
		return lookup().at(productID);
	}
	
	int compareTo(const Product &o) const {
		// TODO Auto-generated method stub
		return (this == &o) ? 0 : (id < o.id ? -1 : id == o.id ? 0 : 1);
	}
};
typedef std::shared_ptr<Product> Product_p;

BOOST_CLASS_EXPORT_GUID(Product::Memo, "productmemo")
