#pragma once
#include "Product.hpp"

class Product::builder {
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
		Product_pp p( new Product(++idr, productID, title, price));
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
