#pragma once
#include "Review.hpp"
#include "Python_utils.hpp"

class Review::builder{
private:

	NLTKInstance::Sentence_Tokenizer &tok;
	smart_int idr;
	static plain_ptr<builder>& b() {static plain_ptr<builder> b(nullptr); return b;}
public:
	friend class Review;
	builder(NLTKInstance::Sentence_Tokenizer &tok)
		:tok(tok),idr(0){ assert(b() == nullptr); b() = this; }
	builder(const builder&) = delete;
	virtual ~builder() {b() = nullptr; std::cout << "builder done" << std::endl;}
	

	Review_pp build(smart_int id, 
					const std::string &summary,
					const std::list<boost::flyweight<std::string> > &sentences,
					double score, 
					int time, 
					Reviewer_pp &reviewer, 
					const Helpfulness &help, 
					Product_pp &product, 
					const std::string &text){
		Review_pp ret(new Review(id,summary, std::move(sentences), score, time, reviewer, help, product, text));
		ret->self = ret;
		reviewer->reviews.insert(ret);
		product->reviews.insert(ret);
		return ret;
	}
	Review_pp build(smart_int id, 
					const Review::strt &summary,
					double score, 
					int time, 
					Reviewer_pp &reviewer, 
					const Helpfulness &help, 
					Product_pp &product, 
					const Review::strt &text){
		auto sent = tok.tokenize< Review::strlt, Review::strt>(text);
		return build(id,summary,std::move(sent),score, time, reviewer, help, product, text);
	}
	
	Review_pp build(const Review::strt &summary,
			double score, 
			int time, 
			Reviewer_pp &reviewer, 
			const Helpfulness &help, 
			Product_pp &product, 
					const Review::strt &text){
		return build(++idr,summary,score,time,reviewer,help,product,text);
	}
};
