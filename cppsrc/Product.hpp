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
public:
	const std::string productID;
	const std::string title;
	const double price;
	std::set<Review_p> reviews;
	std::string productType;
	const smart_int id;

private:
	Product(smart_int id, const std::string &productID, const std::string &title, const double &price, const std::string& productType)
		:productID(productID),title(title),price(price),productType(productType),id(id) {
	}
public:

	Product(const Product &) = delete;

	class Memo;
  	friend class Memo;
	
  	Memo_p pack() const;
  	Memo pod_pack() const;

	class builder;
	friend class builder;
	
	int compareTo(const Product &o) const {
		return (this == &o) ? 0 : (id < o.id ? -1 : id == o.id ? 0 : 1);
	}

};


