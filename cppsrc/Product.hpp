#pragma once

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <string>
#include <set>
#include "Review.hpp"
#include <memory>
#include <unordered_map>
#include <cassert>
#include "Memoize.hpp"
#include "StringUtils.hpp"

class Review;

class Product;
typedef std::weak_ptr<Product> Product_p;
typedef std::shared_ptr<Product> Product_pp;


class Product : public Memoizeable<Product_pp> {
	bool generated_id = false;
public:
	const std::string productID;
	const std::string title;
	const double price;
	std::set<Review*> reviews;
	const int id;
	
	
private:
	static std::unordered_map<std::string, Product_p >& lookup() {
		static std::unordered_map<std::string, Product_p > lookup;
		return lookup;
	}
	
	static int& idr() {static int idr = 0; return idr;}
	
	
	Product(const std::string &productID, const std::string &title, const double &price)
		:productID(productID),title(title),price(price),id(++(idr())){
		generated_id = true;
		assert(lookup().find(productID) == lookup().end());
	}
	
	Product(int id, const std::string &productID, const std::string &title, const double &price)
		:productID(productID),title(title),price(price),id(id) {
		assert((!generated_id) || id > idr()); //ID collision is technically possible.
		idr() += (id - idr() + 1);
	}
	
	
public: 
	
	
	class Memo : public ::Memo<Product_pp> {
	private:
		static std::unordered_map<int, Product_p> & pm(){static std::unordered_map<int, Product_p> pm; return pm;}
  	public:
  		bool serialize_called = false;
  		const bool from_const = false;
  		int id = -1;
  		int gid() {return id;}
  		std::string productID;
  		std::string title;
  		double price;
  		friend class Product;
  		Memo(int id, std::string p, std::string t, double pr)
			:from_const(true),id(id),productID(p),title(t),price(pr){}
		Memo(){}
			Memo(int id):serialize_called(true),id(id){}
		
  		Product_pp unpack() const {
			assert(serialize_called);
			assert(id != -1);
			if (pm().find(id) != pm().end()) return pm().at(id).lock();
			Product_pp ret(new Product(id,productID, title, price));
			lookup()[productID] = ret;
			pm()[id] = ret;
			return ret;
		}

		bool operator<(const Memo &o) const {return id < o.id; }

		bool from_archive() const {return serialize_called && (!from_const); }
		bool from_pack() const {return from_const;}
		
  		template<class Archive>
  		void serialize(Archive &ar, const unsigned int /*version*/) {
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
	
	static Product_pp build(const std::string &productID, const std::string &title, double price){
		if (lookup().find(productID) != lookup().end()) return lookup().at(productID).lock();
		Product_pp p( new Product(productID, title, price));
		lookup()[productID] = p;
		return p;
	}
	
	static Product_pp build(const std::string &productID){
		return lookup().at(productID).lock();
	}
	
	int compareTo(const Product &o) const {
		// TODO Auto-generated method stub
		return (this == &o) ? 0 : (id < o.id ? -1 : id == o.id ? 0 : 1);
	}
	
	static void constructionDone() { lookup().clear(); Memo::pm().clear(); }

};

BOOST_CLASS_EXPORT_GUID(Product::Memo, "productmemo")
