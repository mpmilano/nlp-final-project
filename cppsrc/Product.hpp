#pragma once

#include <string>
#include <set>
#include "Review.hpp"
#include <memory>
#include <unordered_map>
#include <cassert>
#include "Memoize.hpp"

class Review;

class Product;
typedef std::shared_ptr<Product> Product_p;

class Product : public Memoizeable<Product_p>{
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
	
	class Memo : public ::Memo<Product_p> {
	std::string productID;
	std::string title;
	double price;
	friend class Product;
	Memo(std::string p, std::string t, double pr):productID(p),title(t),price(pr){}
	
	Product_p unpack() const { return build(productID, title, price); }
	
	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/) {
		ar & productID;
		ar & title;
		ar & price;
	}
	};

	Product(const std::string &productID, const std::string &title, const double &price)
		:productID(productID),title(title),price(price),id(idr()++){
		assert(lookup().find(productID) == lookup().end());
	}

public: 
  
  	Memo_p pack() const { return Memo_p(new Memo(productID,title,price));}
  
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
