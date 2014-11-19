#pragma once
#include <vector>
#include <sstream>
#include <string>
#include "StringUtils.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "Product.hpp"
#include "Reviewer.hpp"
#include "Review.hpp"
#include "Helpfulness.hpp"


template<typename T> 
bool operator< (const std::weak_ptr<T> &a, const std::weak_ptr<T> &b){
	return a.lock() < b.lock();
}

template<class Input>
class ReviewParser {

private: 
	Input &r;
	ReviewParser(Input &f):r(f){}

	template<typename T>
	class call_cont {
	public:
		const T t;
		call_cont(T &&t):t(t){}
	};

	
	std::string overflow = "";
	std::vector<std::string> v1;
	std::string rangeFromOverflow(const std::string &prefix, const std::string &done){
		if (strContains(overflow, prefix)){
			if (strEndsWith(overflow, prefix)) return prefix;
			v1.clear();
			std::string tmp = overflow.substr(overflow.find(prefix) + prefix.length());
			auto pos = tmp.find(done);
			if (pos != std::string::npos){
				overflow = tmp.substr(pos);
				return prefix + tmp.substr(0,pos);
			}
			else return prefix + tmp;
		}
		return "";
	}

	std::string reallyRead(const std::string &prefix, const std::string &done) {
		std::string acc = rangeFromOverflow(prefix, done);
		std::stringstream racc;
		racc << acc;
		if (strContains(overflow,done)) return post_substr(acc,prefix);
		while (r.good()){
			std::getline(r,acc);
			std::string::size_type split = acc.find(done);
			if (split != std::string::npos){
				overflow = acc.substr(split);
				racc << acc.substr(0,split);
				std::string ret_w_prefix = racc.str();
				return post_substr(ret_w_prefix,prefix);
			}
			else racc << acc;
		}
		
		throw call_cont<std::string> (post_substr(racc.str(),prefix));
	}


public:
	struct sets {
		std::set<Reviewer_p> rrs;
		std::set<Product_p> ps;
		std::set<Review_p> rs;

		sets(std::set<Reviewer_p> rrs, std::set<Product_p> ps, std::set<Review_p> rs)
			:rrs(rrs),ps(ps),rs(rs){}
		sets(){}


		struct Memo : public ::Memo<sets> {

			friend class boost::serialization::access;
			std::vector<Reviewer::Memo> rrs;
			std::vector<Product::Memo> ps;
			std::vector<Review::Memo> rs;
			

			template<typename Archive>
			void serialize(Archive &a, const unsigned int ){
				a & rrs;
				a & ps;
				a & rs;
			}

			void unpack_into(sets &s) const {
				for (auto &e : rrs) s.rrs.insert(e.unpack());
				for (auto &e : ps) s.ps.insert(e.unpack());
				for (auto &e : rs) s.rs.insert(e.unpack());
				{ std:: cout << "sizes: " << s.rrs.size() << " " << s.ps.size() << " " << s.rs.size() << std::endl; }

			}

			sets unpack() const {
				sets s;
				unpack_into(s);
				return s;
			}



			friend struct sets;
			friend class ReviewParser;
			Memo(){}

			//don't need these.
			virtual bool from_archive() const { assert(false && "na"); }
			virtual bool from_pack() const { assert(false && "na"); }

		};

		Memo pod_pack() const {
			Memo m;
			m.rrs.reserve(rrs.size());
			m.ps.reserve(ps.size());
			m.rs.reserve(rs.size());
			{ std:: cout << "sizes: " << rrs.size() << " " << ps.size() << " " << rs.size() << std::endl; }
			for (auto &e: rrs) m.rrs.push_back(e.lock()->pod_pack());
			for (auto &e: ps) m.ps.push_back(e.lock()->pod_pack());
			for (auto &e: rs) m.rs.push_back(e->pod_pack());
			return m;
		}


		
	};
	
private:

	bool readFromFile(const std::string &filename, sets &s) {
	  
	  
		std::string cachefile = "/tmp/rc/" + strReplace(filename,'/','%');
		std::ifstream ifs(cachefile, std::ios::binary | std::ios_base::in);
		if (! ifs.good()) return false;
		{
			std::cout << "file good - proceeding!" << std::endl;
			boost::archive::binary_iarchive ia(ifs);
			typename sets::Memo m;
			ia >> m;
			m.unpack_into(s);
		}//*/
		ifs.close();
		return true;
	}
	
	void writeToFile(const std::string& filename, sets &s) {
	  	std::string cachefile = "/tmp/rc/" + strReplace(filename,'/','%');
		std::ofstream ofs(cachefile, std::ios::binary | std::ios_base::out);
		{
			boost::archive::binary_oarchive oa(ofs);
			auto t = s.pod_pack();
			oa << t;
		}//*/
		ofs.close();
		return;
	}
	
	public: 
	static void parse(const std::string &filename, Reviewer::builder &rrb, Product::builder &pb, Review::builder &rb, sets &s ) {

		
		    std::cout << "starting parse" << std::endl;

		    Input f(filename);
		    ReviewParser rp(f);

		    if (rp.readFromFile(filename,s)) return;
		    
		    sets sout;

		    std::set<Reviewer_p> &cs = sout.rrs;
		    std::set<Product_p> &ps = sout.ps;
		    std::set<Review_p> &rs = sout.rs;
		    
		    int count = 0;

		    std::cout << "beginning parse" << std::endl;
		    while (f.good()){
			    ++count;
			    std::string productId = rp.reallyRead("product/productId: ","product/title: ");
			    std::string productTitle = rp.reallyRead("product/title: ","product/price: ");
			    std::string productPrice = rp.reallyRead("product/price: ", "review/userId: ");
			    std::string reviewUserId = rp.reallyRead("review/userId: ", "review/profileName: ");
			    std::string reviewProfileName = rp.reallyRead("review/profileName: ", "review/helpfulness: ");
			    std::string reviewHelpfulness = rp.reallyRead("review/helpfulness: ", "review/score: ");
			    std::string reviewScore = rp.reallyRead("review/score: ", "review/time: ");
			    std::string reviewTime = rp.reallyRead("review/time: ", "review/summary: ");
			    std::string reviewSummary = rp.reallyRead("review/summary: ", "review/text: ");
			    std::string reviewText = "";
			    try {
				    reviewText = rp.reallyRead("review/text: ", "product/productId: ");
			    }
			    catch (call_cont<std::string> &e){
				    reviewText = e.t;
			    }
			    double price = -1;
			    try {price = std::stod(productPrice);}
			    catch (...) {}
			    auto p = (pb.build(productId, productTitle, price));
			    auto c = (rrb.build(reviewProfileName, reviewUserId));
			    Helpfulness h = Helpfulness::build(std::stoi(pre_substr(reviewHelpfulness,"/")), 
							       std::stoi(post_substr(reviewHelpfulness,"/")));
			    auto r = rb.build(reviewSummary,std::stod(reviewScore), std::stoi(reviewTime),c,h,p,reviewText);
			    cs.insert(Reviewer_p(c));
			    s.rrs.insert(Reviewer_p(c));
			    ps.insert(Product_p(p));
			    s.ps.insert(Product_p(p));
			    s.rs.insert(Review_p(new Review(*r)));
			    rs.insert(std::move(r));
		    }
		    
		    std::cout << "completed parse" << std::endl;
		    
		    rp.writeToFile(filename,sout);
		    
		    { std::cout << "number of reviewers: " << cs.size() << std::endl; }
	    }
};
