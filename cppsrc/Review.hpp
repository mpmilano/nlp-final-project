#pragma once
#include <string>
#include <memory>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <list>
#include <boost/flyweight.hpp>
#include "Helpfulness.hpp"
#include "Reviewer.hpp"
#include "Product.hpp"


class Review;
typedef std::shared_ptr<Review> Review_pp;

class Review : public Memoizeable<Review_pp> {

private:
	Review_p self;
	const smart_int id;
public:

	typedef boost::flyweight<std::string> strt;
	typedef std::list<boost::flyweight<std::string> > strlt;
	const strt summary;
	const strlt sentences; 
	
	const double score;
	const int time;
	const Reviewer_pp reviewer;
	const Helpfulness help;
	const Product_pp product;
	const strt text;

	friend std::ostream& operator<<(std::ostream&, const Review& );


private: 
	Review(	smart_int id, 
		std::string summary,
			const std::list<boost::flyweight<std::string> > &sentences,
			double score, 
			int time, 
			const Reviewer_pp& reviewer, 
			const Helpfulness& help, 
			const Product_pp& product, 
			const std::string& text):
		id(id),
		summary(summary),
		sentences(sentences),
		score(score),
		time(time),
		reviewer(reviewer),
		help(help),
		product(product),
		text(text){}
	
public:

	Review(const Review &r) = delete;

	virtual ~Review(){ 
		Product_pp p = product;
		Reviewer_pp r = reviewer;
		p->reviews.erase(self);
		r->reviews.erase(self);
	}

	//for resource-management of caching structures 
	class builder;
	friend class builder;

	//convert to POD format for serialization: 

	class Memo;

	Memo_p pack() const; 
	Memo pod_pack() const; 
	
};


std::ostream& operator<<(std::ostream& os, const Review& r){
	os << "Title : " << r.summary << std::endl;
	os << "Score : " << r.score << std::endl;
	os << "Time : " << r.time << std::endl;
	os << "Reviewer : " << r.reviewer->profileName << std::endl;
	os << "Helpfulness : " << r.help << std::endl;
	os << "Product : " << r.product->title << std::endl;
	os << "Review : " << r.text << std::endl;
	os << "Sentences tokenized : BEGIN" << std::endl;
	for (auto &s : r.sentences) os << s << std::endl << "-----" << std::endl;
	os << "Sentences tokenized : END" << std::endl;	
	return os;
}

