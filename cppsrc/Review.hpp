#pragma once
#include <string>
#include <memory>
#include "Helpfulness.hpp"
#include "Reviewer.hpp"
#include "Product.hpp"
#include <boost/serialization/export.hpp>

class Review;
typedef std::unique_ptr<Review> Review_p;

class Review : public Memoizeable<Review_p> {

private:
public:
	const std::string summary;
	const double score;
	const int time;
	const Reviewer_p reviewer;
	const Helpfulness help;
	const Product_p product;
	const std::string text;

  
	class Memo : public ::Memo<Review_p> {
		friend class boost::serialization::access;	
		std::string summary;
		double score;
		int time;
		Reviewer::Memo reviewer;
		Helpfulness help;
		Product::Memo product;
		std::string text;
	
		template<class Archive>
		void serialize(Archive &ar, const unsigned int /*version*/)  {
			ar & summary;
			ar & score;
			ar & time;
			ar & reviewer;
			ar & help;
			ar & product;
			ar & text;
		}
		
	public: 
		Review_p unpack() const{
			auto rr = reviewer.unpack();
			auto pr = product.unpack();
			return build(summary,score,time,rr,help,pr,text); 
		}
		

		Memo(std::string s, double sc, int t, Reviewer::Memo r, Helpfulness h, Product::Memo p, std::string tx)
			:summary(s),score(sc),time(t),reviewer(std::move(r)),help(h),product(std::move(p)),text(tx){}
		Memo(){}
		
	};	

	Memo_p pack() const {return Memo_p(new Memo(summary,score,time,reviewer->pod_pack(),help,product->pod_pack(),text));}
	Memo pod_pack() const {return Memo(summary,score,time,reviewer->pod_pack(),help,product->pod_pack(),text);}

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
	
	~Review(){ Product_p p = product;
		   Reviewer_p r = reviewer;
		   p->reviews.erase(this);
		   r->reviews.erase(this);
	}
	
};

BOOST_CLASS_EXPORT_GUID(Review::Memo, "reviewmemo")
