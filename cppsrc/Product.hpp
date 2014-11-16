#pragma once

#include <string>
#include <set>
#include "Review.hpp"
#include <memory>
#include <unordered_map>

class Product {
public:
	const std::string productID;
	const int id;
	const std::string title;
	const double price;
	const std::set<std::shared_ptr<Review> > reviews;

private: 
	static int idr = 0;
	static std::unordered_map<std::string, std::shared_ptr<Product> > lookup;

			
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		ar & productID;
		ar & id;
		ar & title;
		ar & price;
		ar & idr;
	}

	Product(const std::string &productID, const std::string &title, const double &price)
		:productID(productID),price(price),title(title){
		assert(lookup.find(productID) == lookup.end());
		id = idr++;
	}

public: 
	static std::shared_ptr<Product> build(const std::string &productID, const std::string &title, double price){
		if (lookup.find(productID) != lookup.end()) return lookup.at(productID);
		std::shared_ptr<Product> p = new Product(productID, title, price);
		lookup.put(productID, p);
		return p;
	}

	static Product build(const std::string &productID){
		return lookup.at(productID);
	}

	public int compareTo(const Product &o) const {
		// TODO Auto-generated method stub
		return (this == o) ? 0 : (id < o.id ? -1 : id == o.id ? 0 : 1);
	}
};
typedef std::shared_ptr<Product> Product_p;
