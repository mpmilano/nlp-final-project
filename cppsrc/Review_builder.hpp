#pragma once
#include "Review.hpp"
#include "Python_utils.hpp"

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
			const std::list<std::string> &sentences,
			const std::list<std::string> &stemmed_sentences,
			double score, 
			int time, 
			Reviewer_pp &reviewer, 
			const Helpfulness &help, 
			Product_pp &product, 
			const std::string &text){
		Review_pp ret(new Review(id,summary, std::move(sentences), std::move(stemmed_sentences), score, time, reviewer, help, product, text));
		ret->self = ret;
		reviewer->reviews.insert(ret);
		product->reviews.insert(ret);
		return ret;
	}
	Review_pp build(NLTKInstance::Stemmer &stemmer, smart_int id, 
			const std::string &summary,
			double score, 
			int time, 
			Reviewer_pp &reviewer, 
			const Helpfulness &help, 
			Product_pp &product, 
			const std::string &text){
		auto sent = StringSplit<std::list<std::string> >(summary); //TODO - actually tokenize sentences.
		std::list<std::string> stemsent; //TODO - hookin python correctly
		for (auto &w : sent){
			stemsent.push_back(stemmer.stem(w));
		}
		return build(id,summary,std::move(sent),std::move(stemsent),score, time, reviewer, help, product, text);
	}
	
	Review_pp build(NLTKInstance::Stemmer &stemmer,
			const std::string &summary,
			double score, 
			int time, 
			Reviewer_pp &reviewer, 
			const Helpfulness &help, 
			Product_pp &product, 
			const std::string &text){
		return build(stemmer, ++idr,summary,score,time,reviewer,help,product,text);
	}
};
