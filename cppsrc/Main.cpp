#include <set>
#include <string>
#include <iostream>
#include "Review.hpp"
#include "Product.hpp"
#include "Reviewer.hpp"
#include "ReviewParser.hpp"
#include "vector_assembler.hpp"
#include "ReviewDB.hpp"
#include <fstream>
#include <unistd.h>
#include <vector>
#include "getusage.hpp"

#include "ReviewModel.hpp"

using namespace std;

typedef ReviewParser<istream>::sets sets;

template<int features>
class Questions{
private:
	NLTKInstance &nltk;
	ReviewDB &db;
	std::unique_ptr<ReviewModel<features> > model;
	NLTKInstance::Sentence_Tokenizer tok;
	Review::builder rb;
	Product::builder pb;
	Reviewer::builder rrb;

public:

	void parse(ReviewParser<std::istream>::sets &funny, const std::string &prefix, const std::string &conf){
		ReviewParser<std::istream>::parse(prefix,conf,rrb,pb,rb,funny);
	}

	void initial_parse(ReviewParser<std::istream>::sets &funny, ReviewParser<std::istream>::sets &normal, const std::string &prefix, const std::string &conf, const std::string &nope){
		auto &funny_reviews = funny.rs;
		NLTKInstance::Word_Tokenizer wt(nltk);
		NLTKInstance::Stemmer stemmer(nltk);
		{
			parse(funny, prefix, conf);
			parse(normal, prefix, nope);
			std::cout << "parsing done" << std::endl;
		}
	}
	
	void try_from_db(ReviewParser<istream>::sets &funny, ReviewParser<istream>::sets &normal, 
					 const std::string &prefix, const std::string &conf, const std::string &nope){
		{
			if (! (ReviewParser<istream>::readFromFile(rb,pb,rrb, prefix + conf, funny) 
				   && ReviewParser<istream>::readFromFile(rb, pb, rrb, prefix + nope, normal)) ) 
			{
				{
					
					funny.clearAll();
					normal.clearAll();
					rb.reset();
					rrb.reset();
					pb.reset();
					ReviewParser<istream>::sets oldfunny;
					ReviewParser<istream>::sets oldnormal;
					initial_parse(oldfunny,oldnormal, prefix, conf, nope);
					Review::builder rb(tok);
					Product::builder pb;
					Reviewer::builder rrb;
					assert(oldfunny.rs.size() > 0);
					
					db.getAllReviews(rb, pb, rrb, oldfunny.rs, funny, [](auto &r) -> auto& {return *r; });
					std::cout << "got funny!" << std::endl;
					db.getAllReviews(rb, pb, rrb, oldnormal.rs, normal, [](auto &r) -> auto& {return *r; });
					std::cout << "sizes: newr: " << funny.rs.size() <<  " oldr: " << oldfunny.rs.size() 
							  << " newp: " << funny.ps.size() << " oldp " << oldfunny.ps.size()
							  << " newrr: " << funny.rrs.size() <<  " oldrr: " << oldfunny.rrs.size() 
							  << std::endl;
					for (auto &r : funny.rs){
						std::cout << *r << std::endl;
						break;
					}
					
					ReviewParser<istream>::writeToFile(prefix + conf, funny);
					ReviewParser<istream>::writeToFile(prefix + nope, normal);				
				}
			}
		}
	}
	
	
	Questions(NLTKInstance &nltk, ReviewDB &db)
		:nltk(nltk),db(db),tok(nltk),rb(tok){}

	double overall_funny(auto &rrs){
		long agg = 0;
		double aggsum = 0;
		for (auto &r : rrs){
			++agg;
			aggsum += percent_funny(*r);
		}
		return aggsum / agg;

	}

	template<typename... Args>
	void init_model(Args... a){
		model.reset(new ReviewModel<features>(a...));
	}

	double percent_funny(const Reviewer &r){
		assert(model);
		sets s;
		db.getAllReviews(rb,pb,rrb,r,s, {" and not reviewer = 5"} );
		long long funny = 0;
		long long normal = 0;
		auto vecs = populate_vecs(s.rs);
		auto &vm = *std::get<0>(vecs);
		for (auto &p : vm){
			auto h = model.predict(p.second).h;
			switch(h){
			case humor::funny : 
				++funny;
				break;
			case humor::normal : 
				++normal;
				break;
			}
		}

		assert((normal + funny) != 0);
		
		return (normal == 0 ? 1 : ((long double) funny) / (funny + normal));
	}
};

int main() {

	static const std::string prefix = "/home/milano/course/nlp/data/traintest/";
	static const std::string conf = "-funny.txt";
	static const std::string nope = "-normal.txt";


	ReviewParser<istream>::sets funnytrain;
	ReviewParser<istream>::sets normaltrain;
	NLTKInstance nltk;
	ReviewDB db("AmazonReviewsAllCpp");
	static constexpr int num_features = 3;
	static const auto feature_selector = [](const FirstVector &v, int pos) -> double {
		switch(pos){
		case 0: 
		return v.review.reviewer->id.asInt();
		case 1: 
		return std::sqrt(v.numchars * 1.0);
		case 2:
		return v.allpunct;
		default:
		assert(false);
		}
		return -1;
	};

	Questions<3> q(nltk,db);
	q.try_from_db(funnytrain,normaltrain,prefix,str_add("first",conf), str_add("first",nope));

	{
		auto fvs = populate_vecs(funnytrain.rs);
		auto nvs = populate_vecs(normaltrain.rs);

		ReviewModel<num_features> m1(feature_selector, *std::get<0>(fvs), *std::get<0>(nvs), true );
		std::ofstream out("/tmp/training-vector");
		out << m1.getProblem();
	}

	{
		sets industrial;
		q.parse(industrial, "/home/milano/course/nlp/data/","Industrial_&_Scientific.txt" );
		auto ivs = populate_vecs(industrial.rs);
		
		ReviewModel<num_features> m2(feature_selector, *std::get<0>(ivs), *std::get<0>(ivs), true );
		std::ofstream out("/tmp/industrial-vector");
		out << m2.getProblem();	
	}

}

/*
int main2() {


	static const std::string prefix = "/home/milano/course/nlp/data/traintest/";
	static const std::string conf = "-funny.txt";
	static const std::string nope = "-normal.txt";


	ReviewParser<istream>::sets funnytrain;
	ReviewParser<istream>::sets normaltrain;
	ReviewParser<istream>::sets funnytest;
	ReviewParser<istream>::sets normaltest;
	NLTKInstance nltk;
	ReviewDB db("AmazonReviewsAllCpp");
	try_from_db(funnytrain,normaltrain,nltk,db,prefix,str_add("first",conf), str_add("first",nope));
	try_from_db(funnytest,normaltest,nltk,db,prefix,str_add("second",conf), str_add("second",nope));

	auto funny_train_vecs = populate_vecs(funnytrain.rs);
	auto normal_train_vecs = populate_vecs(normaltrain.rs);

	auto funny_test_vecs = populate_vecs(funnytest.rs);
	auto normal_test_vecs = populate_vecs(normaltest.rs);


	auto &vm1 = *std::get<0>(funny_train_vecs);
	auto &vm2 = *std::get<0>(normal_train_vecs);

	auto &vm1_t = *std::get<0>(funny_test_vecs);
	auto &vm2_t = *std::get<0>(normal_test_vecs);

	std::cout << "Vectors assembled" << std::endl;

	ReviewModel<10> m([](const FirstVector &v, int pos) -> double{
			switch(pos){
			case 0: 
				return std::sqrt(v.numchars * 1.0);
			case 1:
				return v.allpunct;
			case 2://books
				return v.category == "Books";
			case 3://clothing
				return v.category == "Clothing";
			case 4://grocery
				return v.category == "Grocery";
			case 5://industrial
				return v.category == "Industrial";
			case 6://movies
				return v.category == "Movies";
			case 7://music
				return v.category == "Music";
			case 8://sports
				return v.category == "Sports";
			case 9://toys
				return v.category == "Toys";
			default: 
				assert(false);
				return -1;
			}},
		vm1, vm2);
	std::cout << "Model trained" << std::endl;
	std::cout << "Model testing: " << m.print_test(m.test(vm1_t, vm2_t)) << std::endl;
	Questions<10> q(nltk,db,m);
	
	std::cout << "overall funny for funny training: " << q.overall_funny(funnytrain.rrs) << std::endl;
	std::cout << "overall funny for normal training: " << q.overall_funny(normaltrain.rrs) << std::endl;
	
	//std::cout << m.getProblem() << std::endl;
	return 0;
	} //*/
