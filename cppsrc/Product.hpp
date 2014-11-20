#pragma once

#include <string>
#include <set>
#include <memory>
#include <unordered_map>
#include <cassert>
#include <boost/serialization/export.hpp>
#include "Memoize.hpp"
#include "StringUtils.hpp"
#include "Review.hpp"

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
	std::set<Review_p> reviews;
	const smart_int id;
	
	
private:

	
	Product(const std::string &productID, const std::string &title, const double &price)
		:productID(productID),title(title),price(price),id(++(builder::b()->idr)){
		generated_id = true;
		assert(builder::b()->lookup.find(productID) == builder::b()->lookup.end());
	}
	
	Product(smart_int id, const std::string &productID, const std::string &title, const double &price)
		:productID(productID),title(title),price(price),id(id) {
		assert((!generated_id) || id > builder::b()->idr); //ID collision is technically possible.
		builder::b()->idr = (id + 1);
	}
	
	
public: 
	
	
	class Memo : public ::Memo<Product_pp> {
  	public:
  		bool serialize_called = false;
  		const bool from_const = false;
  		smart_int id = -1;
  		std::string productID;
  		std::string title;
  		double price;
  		friend class Product;
  		Memo(smart_int id, std::string p, std::string t, double pr)
			:from_const(true),id(id),productID(p),title(t),price(pr){}
		Memo(){}
			Memo(smart_int id):serialize_called(true),id(id){}
		
  		Product_pp unpack() const {
			assert(serialize_called);
			assert(id != -1);
			if (builder::b()->pm.find(id) != builder::b()->pm.end()) return builder::b()->pm.at(id);
			Product_pp ret(new Product(id,productID, title, price));
			builder::b()->lookup[productID] = ret;
			builder::b()->pm[id] = ret;
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


	friend class builder;
	class builder {
		friend class Product;
		static plain_ptr<builder>& b() {static plain_ptr<builder> b(nullptr); return b;}
		std::unordered_map<std::string, Product_pp > lookup;
		smart_int idr;
		std::unordered_map<smart_int, Product_pp> pm;
	public:

		builder():idr(0){ assert(b() == nullptr); b() = this; }
		virtual ~builder() {b() = nullptr; std::cout << "builder done" << std::endl;}

		Product_pp build(const std::string &productID, const std::string &title, double price){
			if (lookup.find(productID) != lookup.end()) return lookup.at(productID);
			Product_pp p( new Product(productID, title, price));
			lookup[productID] = p;
			return p;
		}
		
		Product_pp build(const std::string &productID){
			return lookup.at(productID);
		}

		auto interned(const std::string &productID){
			return lookup.find(productID) != lookup.end();
		}
	};
	
	int compareTo(const Product &o) const {
		// TODO Auto-generated method stub
		return (this == &o) ? 0 : (id < o.id ? -1 : id == o.id ? 0 : 1);
	}

};

BOOST_CLASS_EXPORT_GUID(Product::Memo, "productmemo")
