#pragma once
#include <vector>
#include <sstream>
#include "StringUtils.hpp"

template<class Input>
class ReviewParser {

private: 
	Input r;
	ReviewParser(Input f):r(f){}

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
			if ((auto pos = tmp.find(done)) != std::string::npos){
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
		if (strContains(overflow,done)) return post_substr(acc,prefix);
		while (r.ready()){
			std::getline(r,acc);
			if ((int split = acc.find(done)) != string::npos){
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

	private static void readFromFile(const std::string &filename, std::set<std::shared_ptr<Reviewer> > &cs, 
					 std::set<std::shared_ptr<Product> > &ps, 
					 std::set<std::shared_ptr<Review> > &rs) throws IOException{
		std::string cachefileprefix = "/tmp/" + filename.replace('/','%');
		std::string rprr = cachefileprefix + "rp-rr.obj";
		std::string rpp = cachefileprefix + "rp-p.obj";
		std::string rpr = cachefileprefix + "rp-r.obj";

		assert(false && "still porting");

		return;
	}
	
	public static void parse(const std::string &filename, std::set<Reviewer_p> &cs, std::set<Product_p> &ps, std::set<Review_p> &rs ) throws IOException{
		Input f(filename);
		ReviewParser rp = new ReviewParser(f);
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
			try {
				while (f.ready()){
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
					catch (call_cont<std::string> e){
						reviewText = e.t;
					}
					double price = std::stod(productPrice);
					Product p = Product.build(productId, productTitle, price);
					Reviewer c = Reviewer.build(reviewProfileName, reviewUserId);
					Helpfulness h = Helpfulness.build(std::stoi(pre_substr(reviewHelpfulness,"/")), 
									  std::stoi(post_substr(reviewHelpfulness,"/")));
					Review r = Review.build(reviewSummary,std::stod(reviewScore), std::stoi(reviewTime),c,h,p,reviewText);
					cs.add(c);
					ps.add(p);
					rs.add(r);
				}
				/*
				new ObjectOutputStream(new FileOutputStream(new File(rprr))).writeObject(cs);
				new ObjectOutputStream(new FileOutputStream(new File(rpr))).writeObject(rs);
				new ObjectOutputStream(new FileOutputStream(new File(rpp))).writeObject(ps);
				*/
			}
			catch (... e){
				std::cout << "Got this far: " << count << std::endl;
				throw e;
			}
			/*}//*/
	}
};
