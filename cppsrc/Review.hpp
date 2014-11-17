#pragma once
#include <string>
#include <memory>
#include "Helpfulness.hpp"
#include "Reviewer.hpp"
#include "Product.hpp"

class Review {
public:
	const std::string summary;
	const double score;
	const int time;
	const Reviewer_p reviewer;
	const Helpfulness help;
	const Product_p product;
	const std::string text;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/) {
		ar & summary;
		ar & score;
		ar & time;
		ar & reviewer;
		ar & help;
		ar & product;
		ar & text;
	}
	
private: 
	Review(const std::string summary, 
	       double score, 
	       int time, 
	       const Reviewer_p& reviewer, 
	       const Helpfulness& help, 
	       const Product_p& product, 
	       const std::string& text):
		summary(summary),
		score(score),
		time(time),
		reviewer(reviewer),
		help(help),
		product(product),
		text(text){}
	
public: 
	static std::shared_ptr<Review> build(const std::string &summary, 
			    double score, 
			    int time, 
			    Reviewer_p &reviewer, 
			    const Helpfulness &help, 
			    Product_p &product, 
			    const std::string &text){
		std::shared_ptr<Review> ret(new Review(summary, score, time, reviewer, help, product, text));
		reviewer->reviews.insert(ret);
		product->reviews.insert(ret);
		return ret;
	}

	
};

typedef std::shared_ptr<Review> Review_p;
