#include <set>
#include <string>
#include <iostream>
#include "Review.hpp"
#include "Product.hpp"
#include "Reviewer.hpp"
#include "ReviewParser.hpp"
#include "vector_assembler.hpp"
#include <fstream>
#include <unistd.h>
#include <vector>

using namespace std;
int main() {
	
	ReviewParser<ifstream>::sets s;	
	ReviewParser<ifstream>::sets funny;
	auto &funny_prods = funny.ps;
	{
		NLTKInstance nltk;
		NLTKInstance::Sentence_Tokenizer tok(nltk);
		NLTKInstance::Stemmer stemmer(nltk);
		Reviewer::builder rrb;
		Product::builder pb;
		Review::builder rb(tok,stemmer);

		
		vector<string> names = { 
			"Sports_&_Outdoors.txt",
			"Tools_&_Home_Improvement.txt",
			"Toys_&_Games.txt",
			"Health.txt",
			"Electronics.txt", 
			"Clothing_&_Accessories.txt", 
			"Gourmet_Foods.txt", //*/
			"just-one.txt"  
			, "all-head.txt" ,
			"Home_&_Kitchen.txt" ,
			"Video_Games.txt",
			"Baby.txt",
			"Automotive.txt" //*/
		};
		
		std::string prefix = "/home/milano/course/nlp/data/";
		
		for (auto& endfix : names){
			ReviewParser<ifstream>::parse(prefix + endfix,rrb,pb,rb,s);
		}

		std::cout << "parsing done" << std::endl;

		long long hcntr = 0;
		std::map<Product*, bool> seenmap;
		Helpfulness threshold(95,100);
		int cutoff = 4000;
		for (auto& r : s.rs){
			if (r->help.first > cutoff && (!seenmap[r->product.get()]) && (r->help > threshold) ) {
				seenmap[r->product.get()] = true;
				++hcntr;
				std::cout << *r << std::endl;
			}
		}

		std::cout << "number of >" << cutoff <<" vote reviews:" << hcntr << std::endl;

		std::string word;
		std::ifstream i("/home/milano/course/nlp/related-to-funny");
		int counter = 0;
		while (i >> word){
			++counter;
			if (pb.interned(word)) funny_prods.insert(pb.build(word));
			word.clear();
		}
		std::cout << "of " << counter << " products, " << funny_prods.size() << " were in our current set" << std::endl;
	}

	int frevs;
	auto &funny_reviews = funny.rs;
	auto &funny_reviewers = funny.rrs;
	{
		for (auto &e : funny_prods) {
			for (auto &r : lock<Product>(e)->reviews){
				funny_reviews.insert(lock(r));
			}
		}
		
		std::cout << funny_reviews.size() << std::endl;
		
		VecMap1_p vec1;
		VecMap2_p vec2;
		wordset_p words;
		{
			//new scope because we don't want to accidentally use vecs.
			auto vecs = populate_vecs(funny_reviews);
			vec1 = std::move(std::get<0>(vecs));
			vec2 = std::move(std::get<1>(vecs));
			words = std::move(std::get<2>(vecs));
		}

		{
			std::ofstream v1fo("/home/milano/course/nlp/vec1-funny");
			
			for (auto &v : *vec1){
				v1fo << v.second << std::endl;
			}
			v1fo.close();
		}
		{

			std::ofstream v2fo("/home/milano/course/nlp/vec2-funny");

			for (auto &v : *vec2){
				v2fo << *(v.second) << std::endl;
			}
			
			v2fo.close();
		}
		frevs = funny_reviews.size();
		for (auto &r : funny_reviews){
			funny_reviewers.insert(r->reviewer);
		}
	}

	{
		std::set<Review_pp> normal_reviews;
		{
			int count = 0;
			for (auto &e : s.ps) {
				++count;
				normal_reviews.insert(lock(e)->reviews.begin()->lock());
				if (count == frevs ) break;
			}
		}

		for (auto &e : funny_reviews) normal_reviews.erase(e);
		
		
		VecMap1_p vec1n;
		auto vecs = populate_vecs(normal_reviews);
		vec1n = std::move(std::get<0>(vecs));
		auto vec2n = std::move(std::get<1>(vecs));
		
		{
			std::ofstream v1fo("/home/milano/course/nlp/vec1-normal");
			
			for (auto &v : *vec1n){
				v1fo << v.second << std::endl;
			}
			
			v1fo.close();
		}
		{

			std::ofstream v2fo("/home/milano/course/nlp/vec2-normal");

			for (auto &v : *vec2n){
				v2fo << *(v.second) << std::endl;
			}
			
			v2fo.close();
		}
	}

	for (auto e : s.rs){
		std::cout << *e << std::endl;
		break;
	}
}
