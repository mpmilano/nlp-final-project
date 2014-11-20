#pragma once
#include <string>
#include <memory>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
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

	const std::string summary;
	const double score;
	const int time;
	const Reviewer_pp reviewer;
	const Helpfulness help;
	const Product_pp product;
	const std::string text;

	friend std::ostream& operator<<(std::ostream&, const Review& );

	class Memo : public ::Memo<Review_pp> {

		bool serialize_called = false;
		const bool from_const = false;

		friend class boost::serialization::access; 

		smart_int id;
		std::string summary;
		Score score;
		int time;
		smart_int reviewer;
		Helpfulness help;
		smart_int product;
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

		Review_pp unpack() const{
			auto rr = Reviewer_pp((Reviewer::Memo(reviewer)).unpack());
			auto pr = Product_pp((Product::Memo(product)).unpack());
			return builder::b()->build(id,summary,score,time,rr,help,pr,text);
		}
		

		Memo(smart_int id, std::string s, double sc, int t, Reviewer::Memo r, Helpfulness h, Product::Memo p, std::string tx)
			:from_const(true),
			 id(id),summary(s),score(sc),time(t),reviewer(r.id),help(h),product(p.id),text(tx){}
		Memo():id(-1),reviewer(-1),product(-1){}

		bool from_archive() const {return serialize_called && (!from_const); }
		bool from_pack() const {return from_const;}

		
	};	

	Memo_p pack() const {return Memo_p(new Memo(id,summary,score,time,reviewer->pod_pack(),help,product->pod_pack(),text));}
	Memo pod_pack() const {return Memo(id,summary,score,time,reviewer->pod_pack(),help,product->pod_pack(),text);}

private: 
	Review(	smart_int id, 
	       const std::string summary, 
	       double score, 
	       int time, 
	       const Reviewer_pp& reviewer, 
	       const Helpfulness& help, 
	       const Product_pp& product, 
	       const std::string& text):
		id(id),
		summary(summary),
		score(score),
		time(time),
		reviewer(reviewer),
		help(help),
		product(product),
		text(text){}
	
public:

	friend class builder;
	class builder{
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
	
	virtual ~Review(){ 
		Product_pp p = product;
		Reviewer_pp r = reviewer;
		p->reviews.erase(self);
		r->reviews.erase(self);
	}

	//no caches to purge here!
	
};


std::ostream& operator<<(std::ostream& os, const Review& r){
	os << "Title : " << r.summary << std::endl;
	os << "Score : " << r.score << std::endl;
	os << "Time : " << r.time << std::endl;
	os << "Reviewer : " << r.reviewer->profileName << std::endl;
	os << "Helpfulness : " << r.help << std::endl;
	os << "Product : " << r.product->title << std::endl;
	os << "Review : " << r.text << std::endl;
	return os;
}



BOOST_CLASS_EXPORT_GUID(Review::Memo, "reviewmemo")
