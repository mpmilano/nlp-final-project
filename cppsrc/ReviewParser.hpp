#pragma once
#include <vector>
#include <sstream>
#include <string>
#include "StringUtils.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include "Product.hpp"
#include "Reviewer.hpp"
#include "Review.hpp"
#include "Helpfulness.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/list.hpp>

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
	std::string rangeFromOverflow(const std::string &prefix, const std::string &done){
		if (strContains(overflow, prefix)){
			if (strEndsWith(overflow, prefix)) return prefix;
			static std::vector<std::string> v1;
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

		struct Memo {

			friend class boost::serialization::access;
			std::list<Reviewer::Memo> rrs;
			std::list<Product::Memo> ps;
			std::list<Review::Memo> rs;
			

			template<typename Archive>
			void serialize(Archive &a, const unsigned int ){
				a & rrs;
				a & ps;
				a & rs;
			}

			sets unpack() const {
				sets s;
				for (auto &e : rrs) s.rrs.insert(e.unpack());
				for (auto &e : ps) s.ps.insert(e.unpack());
				for (auto &e : rs) s.rs.insert(e.unpack());
				{ std:: cout << "sizes: " << s.rrs.size() << " " << s.ps.size() << " " << s.rs.size() << std::endl; }
				return s;
			}

			friend struct sets;
			friend class ReviewParser;
			Memo(){}
		};		

		Memo pod_pack() const {
			Memo m;
			{ std:: cout << "sizes: " << rrs.size() << " " << ps.size() << " " << rs.size() << std::endl; }
			for (auto &e: rrs) m.rrs.push_back(e->pod_pack());
			for (auto &e: ps) m.ps.push_back(e->pod_pack());
			for (auto &e: rs) m.rs.push_back(e->pod_pack());
			return m;
		}

			
	};

private:

	static bool readFromFile(const std::string &filename, sets &s) {
	  
	  
		std::string cachefile = "/tmp/rc/" + strReplace(filename,'/','%');
		std::ifstream ifs(cachefile);
		if (! ifs.good()) return false;
		std::cout << "file good - proceeding!" << std::endl;
		boost::archive::binary_iarchive ia(ifs);
		typename sets::Memo m;
		ia >> m;
		s = m.unpack();
		return true;
	}
	
	static void writeToFile(const std::string& filename, sets &s) {
	  	std::string cachefile = "/tmp/rc/" + strReplace(filename,'/','%');
		std::ofstream ofs(cachefile);
		boost::archive::binary_oarchive oa(ofs);
		auto t = s.pod_pack();
		oa << t;
		return;
	}
	
	public: 
	    static void parse(const std::string &filename, sets &s ) {

		
		    std::cout << "starting parse" << std::endl;

		    if (readFromFile(filename,s)) return;

		    std::set<Reviewer_p> &cs = s.rrs;
		    std::set<Product_p> &ps = s.ps;
		    std::set<Review_p> &rs = s.rs;
		    
		    Input f(filename);
		    ReviewParser rp(f);
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
			    Product_p p = Product::build(productId, productTitle, price);
			    Reviewer_p c = Reviewer::build(reviewProfileName, reviewUserId);
			    Helpfulness h = Helpfulness::build(std::stoi(pre_substr(reviewHelpfulness,"/")), 
							       std::stoi(post_substr(reviewHelpfulness,"/")));
			    auto r = Review::build(reviewSummary,std::stod(reviewScore), std::stoi(reviewTime),c,h,p,reviewText);
			    cs.insert(c);
			    ps.insert(p);
			    rs.insert(std::move(r));
		    }
		    
		    std::cout << "completed parse" << std::endl;
		    writeToFile(filename,s);
		    
		    { std::cout << "number of reviewers: " << cs.size() << std::endl; }
	    }
};
