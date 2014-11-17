#pragma once
#include <string>
#include <memory>
#include "Helpfulness.hpp"
#include "Reviewer.hpp"
#include "Product.hpp"
#include <boost/serialization/split_member.hpp>

class Review;
typedef std::unique_ptr<Review> Review_p;

class Review : public Memoizeable<Review_p> {

private:
	Reviewer_p _reviewer;  
	Product_p _product;
public:
	const std::string summary;
	const double score;
	const int time;
	const Reviewer_p& reviewer(){return _reviewer;}
	const Helpfulness help;
	const Product_p& product(){return _product;}
	const std::string text;

private:
  
	class Memo : public ::Memo<Review_p> {
	friend class boost::serialization::access;
	
	std::string summary;
	double score;
	int time;
	::Memo<Reviewer_p>::t reviewer;
	Helpfulness help;
	::Memo<Product_p>::t product;
	std::string text;
	
	template<class Archive>
	void save(Archive &ar, const unsigned int /*version*/) const {
		ar & summary;
		ar & score;
		ar & time;
		ar & reviewer;
		ar & help;
		ar & product;
		ar & text;
	}
	
	template<class Archive>
	void load(Archive &ar, const unsigned int /*version*/) {
		ar & summary;
		ar & score;
		ar & time;
		ar & reviewer;
		ar & help;
		ar & product;
		ar & text;
	}
	
	Review_p unpack() const{
	 	auto rr = reviewer->unpack();
		auto pr = product->unpack();
		return build(summary,score,time,rr,help,pr,text); 
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	
	public: 
	  Memo(std::string s, double sc, int t, ::Memo<Reviewer_p>::t r, Helpfulness h, ::Memo<Product_p>::t p, std::string tx)
	  :summary(s),score(sc),time(t),reviewer(std::move(r)),help(h),product(std::move(p)),text(tx){}

	};
	
public:
	Memo_p pack() const {return Memo_p(new Memo(summary,score,time,_reviewer->pack(),help,_product->pack(),text)); }

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
		_reviewer(reviewer),
		help(help),
		_product(product),
		text(text){}
	
public: 
	static std::unique_ptr<Review> build(const std::string &summary,
			    double score, 
			    int time, 
			    Reviewer_p &reviewer, 
			    const Helpfulness &help, 
			    Product_p &product, 
			    const std::string &text){
		std::unique_ptr<Review> ret(new Review(summary, score, time, reviewer, help, product, text));
		reviewer->reviews.insert(ret.get());
		product->reviews.insert(ret.get());
		return ret;
	}
	
	~Review(){ Product_p p = _product;
		   Reviewer_p r = _reviewer;
		   p->reviews.erase(this);
		   r->reviews.erase(this);
	}
	
};

