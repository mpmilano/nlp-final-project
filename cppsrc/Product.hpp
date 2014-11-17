#pragma once

#include <string>
#include <set>
#include "Review.hpp"
#include <memory>
#include <unordered_map>
#include <cassert>

class Review;
typedef std::shared_ptr<Review> Review_p;

class Product;
typedef std::shared_ptr<Product> Product_p;

class Product {
public:
	const std::string productID;
	const std::string title;
	const double price;
	std::set<Review_p> reviews;
	const int id;


private:
	static std::unordered_map<std::string, std::shared_ptr<Product> >& lookup() {
	      static std::unordered_map<std::string, std::shared_ptr<Product> > lookup;
	      return lookup;
	  }

	static int& idr() {static int idr = 0; return idr;}
	
			
	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/) {
		ar & productID;
		ar & id;
		ar & title;
		ar & price;
		ar & idr;
	}

	Product(const std::string &productID, const std::string &title, const double &price)
		:productID(productID),title(title),price(price),id(idr()++){
		assert(lookup().find(productID) == lookup().end());
	}

public: 
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
