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
#include "Questions.hpp"

using namespace std;

int main1() {

	NLTKInstance nltk;
	ReviewDB db("AmazonReviewsAllCpp");
	static constexpr int num_features = 4;
	static const auto feature_selector = [](const FirstVector &v, int pos) -> double {
		switch(pos){
		case 0: 
		return v.review.reviewer->id.asInt();
		case 1: 
		return std::sqrt(v.numchars * 1.0);
		case 2:
		return v.allpunct;
		case 3:
		return v.review.product->id.asInt();
		default:
		assert(false);
		}
		return -1;
	};

	Questions<3> q(nltk,db);
	
	/*
	{
		static const std::string prefix = "/home/milano/course/nlp/data/traintest/";
		static const std::string conf = "-funny.txt";
		static const std::string nope = "-normal.txt";
		ReviewParser<istream>::sets funnytrain;
		ReviewParser<istream>::sets normaltrain;

		q.initial_parse(funnytrain,normaltrain,prefix,str_add("first",conf), str_add("first",nope));
		auto fvs = populate_vecs(funnytrain.rs);
		auto nvs = populate_vecs(normaltrain.rs);

		ReviewModel<num_features> m1(feature_selector, *std::get<0>(fvs), *std::get<0>(nvs), true );
		std::ofstream out("/tmp/training-vector");
		out << m1.getProblem();
	} //*/

	{
		sets industrial;
		q.parse(industrial, "/home/milano/course/nlp/data/","Clothing" );
		auto ivs = populate_vecs(industrial.rs);
		
		VecMap1 empty;

		ReviewModel<num_features> m2(feature_selector, *std::get<0>(ivs), empty, true );
		std::ofstream out("/tmp/clothing-vector.txt");
		out << fixed;
		out << m2.getProblem();
		{
			std::ofstream rrmap("/tmp/clothing-reviewerid-userid.txt");
			for (auto &rr : industrial.rrs) rrmap << rr->id.asInt() << " " << rr->userID << " " << rr->profileName << std::endl;
		}
		{
			std::ofstream pmap("/tmp/clothing-proecut-userid.txt");
			for (auto &p : industrial.ps) pmap << p->id.asInt() << " " << p->productID << " " << p->title << std::endl;
		}
	}

	return 0;

}


int main2() {


	static const std::string prefix = "/home/milano/course/nlp/data/traintest/";
	static const std::string conf = "-funny.txt";
	static const std::string nope = "-normal.txt";
	static constexpr int features = 10;
	auto model_fun = [](const FirstVector &v, int pos) -> double{
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
			}};

	NLTKInstance nltk;
	ReviewDB db("AmazonReviewsAllCpp");

	Questions<features> q(nltk,db);

	ReviewParser<istream>::sets funnytrain;
	ReviewParser<istream>::sets normaltrain;
	ReviewParser<istream>::sets funnytest;
	ReviewParser<istream>::sets normaltest;

	q.try_from_db(funnytrain,normaltrain,prefix,str_add("first",conf), str_add("first",nope));
	q.try_from_db(funnytest,normaltest,prefix,str_add("second",conf), str_add("second",nope));

	auto funny_train_vecs = populate_vecs(funnytrain.rs);
	auto normal_train_vecs = populate_vecs(normaltrain.rs);

	auto funny_test_vecs = populate_vecs(funnytest.rs);
	auto normal_test_vecs = populate_vecs(normaltest.rs);


	auto &vm1 = *std::get<0>(funny_train_vecs);
	auto &vm2 = *std::get<0>(normal_train_vecs);

	auto &vm1_t = *std::get<0>(funny_test_vecs);
	auto &vm2_t = *std::get<0>(normal_test_vecs);

	std::cout << "Vectors assembled" << std::endl;
	
	q.init_model(model_fun, vm1, vm2);
	
	//std::cout << "overall funny for funny training: " << q.overall_funny(funnytrain.rrs) << std::endl;
	//std::cout << "overall funny for normal training: " << q.overall_funny(normaltrain.rrs) << std::endl;
	{
		auto p = q.funniest_reviewer(5, funnytrain.rrs);
		std::cout << "Funniest (>5) reviewer has " << p.first << " reviews, " << p.second * 100 << "% of which are funny." <<std::endl;
	}

	/*
	{
		auto p = q.prolific_funny(funnytrain.rrs);
		std::cout << "prolific funny " << p << std::endl;

	} //*/

	std::cout << "kinda count: " << q.count_kindafunny(funnytrain.rrs) << std::endl;
	// 103
	std::cout << "kinda serious count: " << q.count_kindaserious(funnytrain.rrs) << std::endl;

	
	//std::cout << m.getProblem() << std::endl;
	return 0;
} //*/

int main() { return main2(); }
