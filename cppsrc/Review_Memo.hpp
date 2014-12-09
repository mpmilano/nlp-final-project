#pragma once
#include "Review.hpp"
#include "Review_builder.hpp"
#include <boost/serialization/list.hpp>

//Review Memo!
class Review::Memo : public ::Memo<Review_pp> {
	
	bool serialize_called = false;
	const bool from_const = false;
public: 
	
	friend class boost::serialization::access; 

	smart_int id;
	std::string summary;
	std::list<std::string> sentences;
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
		ar & score;
		ar & time;
		ar & reviewer;
		ar & help;
		ar & product;
		ar & text;
	}
	
	bool operator<(const Memo &o) const {
		return id < o.id;
	}
	
	Review_pp unpack() const{
		auto rr = Reviewer_pp((Reviewer::Memo(reviewer)).unpack());
		auto pr = Product_pp((Product::Memo(product)).unpack());
		strlt sents;
		for (auto s : sentences) sents.push_back(strt(s));
		strt summ(summary);
		strt txt(text);
		return builder::b()->build(id,summ,sents,score,time,rr,help,pr,txt);
	}
	
	Memo(int id, std::string s, std::list<std::string> ss, 
	     double sc, int t, int r, Helpfulness h, int p, std::string tx)
		:from_const(true),
		 id(id),summary(s),sentences(ss),score(sc),time(t),reviewer(r),help(h),product(p),text(tx){}


	Memo(smart_int id, std::string s, std::list<std::string> ss, 
	     double sc, int t, Reviewer::Memo r, Helpfulness h, Product::Memo p, std::string tx)
		:from_const(true),
		 id(id),summary(s),sentences(ss),score(sc),time(t),reviewer(r.id),help(h),product(p.id),text(tx){}
	Memo():id(-1),reviewer(-1),product(-1){}
	
	bool from_archive() const {return serialize_called && (!from_const); }
	bool from_pack() const {return from_const;}
	
	
};

Review::Memo_p Review::pack() const {
	std::list<std::string> snt;
	for (auto &s : sentences) snt.push_back(std::string(s));
	return Memo_p(new Memo(id,summary,snt,score,time,reviewer->pod_pack(),help,product->pod_pack(),text));
}
Review::Memo Review::pod_pack() const {
	std::list<std::string> snt;
	for (auto &s : sentences) snt.push_back(std::string(s));
	return Memo(id,summary,snt,score,time,reviewer->pod_pack(),help,product->pod_pack(),text);
}

BOOST_CLASS_EXPORT_GUID(Review::Memo, "reviewmemo")
