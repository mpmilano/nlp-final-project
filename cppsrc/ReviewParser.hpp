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
		//{ std::cout << "starting at: " << prefix << " stopping at: " << done << std::endl; }
		std::string acc = rangeFromOverflow(prefix, done);
		//{ std::cout << "overflow had: " << acc << std::endl; }
		std::stringstream racc;
		racc << acc;
		if (strContains(overflow,done)) return post_substr(acc,prefix);
		while (r.good()){
			std::getline(r,acc);
			std::string::size_type split = acc.find(done);
			if (split != std::string::npos){
				/*{
					assert(acc.find(done) == split);
					assert(done[0] == acc[split]);
					std::cout << "diagnostics" << std::endl;
					std::cout << acc << std::endl;
					std::cout << done << " at " << split << std::endl;
					std::cout << "racc has " << racc.str() << std::endl;
				}//*/
				overflow = acc.substr(split);
				racc << acc.substr(0,split);
				std::string ret_w_prefix = racc.str();
				return post_substr(ret_w_prefix,prefix);
			}
			else racc << acc;
		}
		
		throw call_cont<std::string> (post_substr(racc.str(),prefix));
	}

	/**
	   Give me a filename and three empty sets, and I'll populate the sets
	   with all of the stuff in the file. 
	 * @throws FileNotFoundException 
	 */

	static bool readFromFile(const std::string &filename, std::set<Reviewer_p > &rrs,
					 std::set<Product_p > &ps, 
					 std::set<Review_p > &rs) {
	  
	  
		std::string cachefileprefix = "/tmp/" + strReplace(filename,'/','%');
		std::string rprr = cachefileprefix + "rp-rr.obj";
		std::string rpp = cachefileprefix + "rp-p.obj";
		std::string rpr = cachefileprefix + "rp-r.obj";
		std::ifstream ifs(rpr);
		if (! ifs.good()) return false;
		std::cout << "file good - proceeding!" << std::endl;
		boost::archive::binary_iarchive ia(ifs);
		std::set<Reviewer_p>::size_type rrsize;
		ia >> rrsize;
		std::set<Product_p>::size_type psize;
		ia >> psize;
		std::set<Review_p>::size_type rsize;
		ia >> rsize;
		
		for (std::set<Reviewer_p>::size_type i = 0; i < rrsize; ++i){
			Reviewer::Memo r;
			ia >> r;
			rrs.insert(Reviewer_p(r.unpack()));
		}

		for (std::set<Product_p>::size_type i = 0; i < psize; ++i){
			Product::Memo r;
			ia >> r;
			ps.insert(Product_p(r.unpack()));
		}

		for (std::set<Review_p>::size_type i = 0; i < rsize; ++i){
			Review::Memo r;
		    ia >> r;
		    rs.insert(Review_p(r.unpack()));
		}
//*/
		return true;
	}
	
	static void writeToFile(const std::string& filename, std::set<Reviewer_p > &rrs,
					 std::set<Product_p > &ps, 
					 std::set<Review_p > &rs){
	  	std::string cachefileprefix = "/tmp/" + strReplace(filename,'/','%');
		std::string rprr = cachefileprefix + "rp-rr.obj";
		std::string rpp = cachefileprefix + "rp-p.obj";
		std::string rpr = cachefileprefix + "rp-r.obj";
		std::ofstream ofs(rpr);
		boost::archive::binary_oarchive oa(ofs);
		{
		  auto tmp = rrs.size();
		  oa << tmp;
		}
		for (const auto &r : rrs) { auto t1 = r->pod_pack(); oa << t1;}
		{
		  auto tmp = ps.size();
		  oa << tmp;
		}
		for (const auto &r : ps) { auto t1 = r->pod_pack(); oa << t1;}
		{
		  auto tmp = rs.size();
		  oa << tmp;
		}
		for (const auto &r : rs) { auto t1 = r->pod_pack(); oa << t1;}

		return;
	}
	
	public: 
	    static void parse(const std::string &filename, std::set<Reviewer_p> &cs, std::set<Product_p> &ps, std::set<Review_p> &rs ) {
		
		    std::cout << "starting parse" << std::endl;

		    if (readFromFile(filename,cs,ps,rs)) return;

		Input f(filename);
		ReviewParser rp(f);
		int count = 0;
		

/*
		std::string cachefileprefix = "/tmp/" + filename.replace('/','%');
		std::string rprr = cachefileprefix + "rp-rr.obj";
		std::string rpp = cachefileprefix + "rp-p.obj";
		std::string rpr = cachefileprefix + "rp-r.obj";
	
		try {
			cs.addAll((std::set<Reviewer>) new ObjectInputStream(new FileInputStream(new File(rprr))).readObject());
			ps.addAll((std::set<Product>) new ObjectInputStream(new FileInputStream(new File(rpp))).readObject());
			rs.addAll((std::set<Review>) new ObjectInputStream(new FileInputStream(new File(rpr))).readObject());
			return;
		}
		catch (Exception ignoreme){
*/

		std::cout << "beginning parse" << std::endl;
				while (f.good()){
					++count;
					std::string productId = rp.reallyRead("product/productId: ","product/title: ");
					// { std::cout << "(yay) productID: " << productId << std::endl; }
					std::string productTitle = rp.reallyRead("product/title: ","product/price: ");
					//{ std::cout << "(yay) productTitle: " << productTitle << std::endl; }
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
				writeToFile(filename,cs, ps, rs);
				/*
				new ObjectOutputStream(new FileOutputStream(new File(rprr))).writeObject(cs);
				new ObjectOutputStream(new FileOutputStream(new File(rpr))).writeObject(rs);
				new ObjectOutputStream(new FileOutputStream(new File(rpp))).writeObject(ps);
				*/
			
			/*}//*/
	}
};
