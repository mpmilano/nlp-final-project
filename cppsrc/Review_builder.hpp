#pragma once
#include "Review.hpp"

class Review::builder{
private:
	smart_int idr;
	static plain_ptr<builder>& b() {static plain_ptr<builder> b(nullptr); return b;}
public:
	friend class Review;
	builder():idr(0){ assert(b() == nullptr); b() = this; }
	virtual ~builder() {b() = nullptr; std::cout << "builder done" << std::endl;}
	
	Review_pp build(smart_int id, 
			const std::string &summary,
			double score, 
			int time, 
			Reviewer_pp &reviewer, 
			const Helpfulness &help, 
			Product_pp &product, 
			const std::string &text){
		Review_pp ret(new Review(id,summary, score, time, reviewer, help, product, text));
		ret->self = ret;
		reviewer->reviews.insert(ret);
		product->reviews.insert(ret);
		return ret;
	}
	
	Review_pp build(const std::string &summary,
			double score, 
			int time, 
			Reviewer_pp &reviewer, 
			const Helpfulness &help, 
			Product_pp &product, 
			const std::string &text){
			return build(++idr,summary,score,time,reviewer,help,product,text);
	}
};
