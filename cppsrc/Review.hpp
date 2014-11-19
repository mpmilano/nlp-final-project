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
	int & idr() {static int idr = 0; return idr; }
	int id;
public:

	const std::string summary;
	const double score;
	const int time;
	const Reviewer_pp reviewer;
	const Helpfulness help;
	const Product_pp product;
	const std::string text;

  
	class Memo : public ::Memo<Review_p> {

		bool serialize_called = false;
		const bool from_const = false;

		friend class boost::serialization::access;	
		int id;
		std::string summary;
		double score;
		int time;
		int reviewer;
		Helpfulness help;
		int product;
		std::string text;
	
		template<class Archive>
		void serialize(Archive &ar, const unsigned int /*version*/)  {
			ar & id;
			ar & summary;
			ar & score;
			ar & time;
			ar & reviewer;
			ar & help;
			ar & product;
			ar & text;
		}
		
	public: 
		bool operator<(const Memo &o) const {
			return id < o.id;
		}

		Review_p unpack() const{
			auto rr = Reviewer_pp((Reviewer::Memo(reviewer)).unpack());
			auto pr = Product_pp((Product::Memo(product)).unpack());
			return build(summary,score,time,rr,help,pr,text); 
		}
		

		Memo(int id, std::string s, double sc, int t, Reviewer::Memo r, Helpfulness h, Product::Memo p, std::string tx)
			:from_const(true),
			 id(id),summary(s),score(sc),time(t),reviewer(r.gid()),help(h),product(p.gid()),text(tx){}
		Memo(){}

		bool from_archive() const {return serialize_called && (!from_const); }
		bool from_pack() const {return from_const;}

		
	};	

	Memo_p pack() const {return Memo_p(new Memo(id,summary,score,time,reviewer->pod_pack(),help,product->pod_pack(),text));}
	Memo pod_pack() const {return Memo(id,summary,score,time,reviewer->pod_pack(),help,product->pod_pack(),text);}

private: 
	Review(const std::string summary, 
	       double score, 
	       int time, 
	       const Reviewer_pp& reviewer, 
	       const Helpfulness& help, 
	       const Product_pp& product, 
	       const std::string& text):
		id(++(idr())),
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
			    Reviewer_pp &reviewer, 
			    const Helpfulness &help, 
			    Product_pp &product, 
			    const std::string &text){
		std::unique_ptr<Review> ret(new Review(summary, score, time, reviewer, help, product, text));
		reviewer->reviews.insert(ret.get());
		product->reviews.insert(ret.get());
		return ret;
	}
	
	virtual ~Review(){ 
		Product_pp p = product;
		Reviewer_pp r = reviewer;
		p->reviews.erase(this);
		r->reviews.erase(this);
	}

	//no caches to purge here!
	static void constructionDone(){}
	
};

BOOST_CLASS_EXPORT_GUID(Review::Memo, "reviewmemo")
