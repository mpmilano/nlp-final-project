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
	std::string productType;
	friend class Product;
	Memo(smart_int id, std::string p, std::string t, double pr, std::string productType)
		:from_const(true),id(id),productID(p),title(t),price(pr),productType(productType){}
	Memo(){}
	Memo(smart_int id):serialize_called(true),id(id){}
	
	Product_pp unpack() const {
		assert(id != -1);
		auto b = builder::b();
		if (b->pm.find(id) != b->pm.end()) return b->pm.at(id);
		Product_pp ret(new Product(id,productID, title, price, productType));
		b->lookup[productID] = ret;
		b->pm[id] = ret;
		if (id > b->idr) b->idr = id;
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
		ar & productType;
		serialize_called = true;
		assert(id != -1);
	}
};



Product::Memo_p Product::pack() const { return Memo_p(new Memo(id,productID,title,price, productType));}
Product::Memo Product::pod_pack() const { return Memo(id,productID,title,price, productType);}

BOOST_CLASS_EXPORT_GUID(Product::Memo, "productmemo")
