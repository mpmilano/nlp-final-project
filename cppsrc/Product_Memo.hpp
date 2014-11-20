#pragma once
#include "Product_builder.hpp"

class Product::Memo : public ::Memo<Product_pp> {
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



Product::Memo_p Product::pack() const { return Memo_p(new Memo(id,productID,title,price));}
Product::Memo Product::pod_pack() const { return Memo(id,productID,title,price);}

BOOST_CLASS_EXPORT_GUID(Product::Memo, "productmemo")
