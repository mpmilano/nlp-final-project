#pragma once
#include "Review.hpp"
#include "Review_builder.hpp"
#include <boost/serialization/list.hpp>

//Review Memo!
class Review::Memo : public ::Memo<Review_pp> {
	
	bool serialize_called = false;
	const bool from_const = false;
	
	friend class boost::serialization::access; 

	smart_int id;
	std::string summary;
	std::list<std::string> sentences;
	std::list<std::string> stemmed_sentences;
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
		ar & sentences;
		ar & stemmed_sentences;
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
		return builder::b()->build(id,summary,sentences, stemmed_sentences,score,time,rr,help,pr,text);
	}
	

	Memo(smart_int id, std::string s, std::list<std::string> ss, std::list<std::string> sts, 
	     double sc, int t, Reviewer::Memo r, Helpfulness h, Product::Memo p, std::string tx)
		:from_const(true),
		 id(id),summary(s),sentences(ss),stemmed_sentences(sts),score(sc),time(t),reviewer(r.id),help(h),product(p.id),text(tx){}
	Memo():id(-1),reviewer(-1),product(-1){}
	
	bool from_archive() const {return serialize_called && (!from_const); }
	bool from_pack() const {return from_const;}
	
	
};

Review::Memo_p Review::pack() const {
	return Memo_p(new Memo(id,summary,sentences,stemmed_sentences,score,time,reviewer->pod_pack(),help,product->pod_pack(),text));
}
Review::Memo Review::pod_pack() const {
	return Memo(id,summary,sentences,stemmed_sentences,score,time,reviewer->pod_pack(),help,product->pod_pack(),text);
}

BOOST_CLASS_EXPORT_GUID(Review::Memo, "reviewmemo")
