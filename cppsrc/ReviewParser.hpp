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
#include "Product_Memo.hpp"
#include "Reviewer_Memo.hpp"
#include "Review.hpp"
#include "Review_Memo.hpp"
#include "Review_builder.hpp"
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
		
		typedef Reviewer_pp rrp;
		typedef Product_pp pp;

		std::set<rrp> rrs;
		std::set<pp> ps;
		std::set<Review_pp> rs;

		sets(std::set<rrp> rrs, std::set<pp> ps, std::set<Review_pp> rs)
			:rrs(rrs),ps(ps),rs(rs){}
		sets(){}

		void clearAll(){
			rrs.clear();
			ps.clear();
			rs.clear();
		}


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
			for (auto &e: rrs) m.rrs.push_back(lock(e)->pod_pack());
			for (auto &e: ps) m.ps.push_back(lock(e)->pod_pack());
			for (auto &e: rs) m.rs.push_back(e->pod_pack());
			return m;
		}


		
	};
	
private:

	static bool readFromFile(const std::string &filename, sets &s) {
		try {
			std::string cachefile = "/tmp/rc2/" + strReplace(filename,'/','%');
			mmapStream ms(cachefile);
			std::istream &ifs = ms.s;
			//std::ifstream ifs(cachefile, std::ios::binary | std::ios_base::in);
			if (! ifs.good()) return false;
			{
				std::cout << "file good - proceeding!" << std::endl;
				boost::archive::binary_iarchive ia(ifs);
				typename sets::Memo m;
				ia >> m;
				m.unpack_into(s);
			}//*/
			return true;
		}
		catch (...) { return false; }
	}


public:

	static void writeToFile(const std::string& filename, sets &s) {
	  	std::string cachefile = "/tmp/rc2/" + strReplace(filename,'/','%');
		std::ofstream ofs(cachefile, std::ios::binary | std::ios_base::out);
		{
			boost::archive::binary_oarchive oa(ofs);
			auto t = s.pod_pack();
			oa << t;
		}//*/
		ofs.close();
		return;
	}
	


	static bool readFromFile(Review::builder&, Product::builder&, Reviewer::builder&, const std::string &filename, sets &s){
		return readFromFile(filename, s);
	}

	/*
	static void parseAndUpdateCategories(const std::string &filename){
		mmapStream ms(filename);
		std::istream &f = ms.s;
		

		bool first = true;
		std::string category;
		std::string product;
		ReviewDB<Input> db("AmazonReviewsAllCpp");

		while (f.good()){
			std::string acc;
			std::getline(f,acc);
			if (acc.at(0) == ' '){
				if (first) {
					std::stringstream ss(acc);
					std::string fw;
					ss >> fw;
					try {
						if (ispunct(fw.at(fw.length() -1 ))){
							fw = fw.substr(0,fw.length() -1 );
						}
					}
					catch (std::out_of_range) {
						category="<OTHER>";
					}
					category = fw;
					first = false;
					db.updateCategory(product,category);
				}
			}
			else {
				product = acc;
				first = true;
			}
			acc.clear();
		}
		} //*/

	static void parse(const std::string &pre, const std::string &post, 
			  Reviewer::builder &rrb, Product::builder &pb, Review::builder &rb, sets &s ) {
		const std::string filename = pre + post;
		
		    std::cout << "starting parse" << std::endl;

			mmapStream ms(filename);
			std::istream &f = ms.s;
		    ReviewParser rp(f);

		    if (rp.readFromFile(filename,s)) return;
		    
		    sets sout;

		    auto &cs = sout.rrs;
		    auto &ps = sout.ps;
		    auto &rs = sout.rs;
		    
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
				const std::string &productType = post;
			    try {
				    reviewText = rp.reallyRead("review/text: ", "product/productId: ");
			    }
			    catch (call_cont<std::string> &e){
				    reviewText = e.t;
			    }
			    double price = -1;
			    try {price = std::stod(productPrice);}
			    catch (...) {}
			    auto p = (pb.build(productId, productTitle, price, productType));
			    auto c = (rrb.build(reviewProfileName, reviewUserId));
			    Helpfulness h = Helpfulness::build(std::stoi(pre_substr(reviewHelpfulness,"/")), 
							       std::stoi(post_substr(reviewHelpfulness,"/")));
			    auto r = rb.build(Review::strt(reviewSummary),std::stod(reviewScore), std::stoi(reviewTime),c,h,p,Review::strt(reviewText));
			    typename sets::rrp rrp(c);
			    cs.insert(rrp);
			    s.rrs.insert(rrp);
			    typename sets::pp pp(p);
			    ps.insert(pp);
			    s.ps.insert(pp);
			    s.rs.insert(r);
			    rs.insert(r);

		    }
		    
		    std::cout << "completed parse" << std::endl;
		    
		    rp.writeToFile(filename,sout);
		    
		    { std::cout << "number of reviewers: " << cs.size() << std::endl; }
	    }
};
