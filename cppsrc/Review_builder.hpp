#pragma once
#include "Review.hpp"
#include "Python_utils.hpp"

class Review::builder{
private:

	smart_int idr;
	static plain_ptr<builder>& b() {static plain_ptr<builder> b(nullptr); return b;}
public:
	NLTKInstance::Sentence_Tokenizer &tok;
	friend class Review;
	builder(NLTKInstance::Sentence_Tokenizer &tok)
		:idr(0),tok(tok){ assert(b() == nullptr); b() = this; }
	builder(const builder&) = delete;
	virtual ~builder() {b() = nullptr; std::cout << "builder done" << std::endl;}

	void reset(){
		idr = 0;
	}

	Review_pp build(smart_int id, 
					const strt &summary,
					const strlt &sentences,
					double score, 
					int time, 
					Reviewer_pp &reviewer, 
					const Helpfulness &help, 
					Product_pp &product, 
					const strt &text){
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
		auto sent = tok.tokenize< Review::strlt>(text);
		return build(id,summary,sent,score, time, reviewer, help, product, text);
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
