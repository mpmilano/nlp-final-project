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
	std::set<Product_p> &funny_prods = funny.ps;
	{
		Reviewer::builder rrb;
		Product::builder pb;
		Review::builder rb;
		
		vector<string> names = {
/*			"Sports_&_Outdoors.txt",
			"Tools_&_Home_Improvement.txt",
			"Toys_&_Games.txt",
			"Health.txt",
			"Electronics.txt", 
			"Clothing_&_Accessories.txt", 
			"Gourmet_Foods.txt", //*/
			"all-head.txt" ,
			"Home_&_Kitchen.txt" /*,
			"Video_Games.txt",
			"Baby.txt",
			"Automotive.txt"//*/
		};
		
		std::string prefix = "/home/milano/course/nlp/data/";
		
		for (auto& endfix : names){
			ReviewParser<ifstream>::parse(prefix + endfix,rrb,pb,rb,s);
		}

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
	std::set<Review_pp> &funny_reviews = funny.rs;
	std::set<Reviewer_p> &funny_reviewers = funny.rrs;
	{
		for (auto &e : funny_prods) {
			for (auto &r : e.lock()->reviews){
				funny_reviews.insert(r.lock());
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
		frevs = funny_reviews.size();
		for (auto &r : funny_reviews){
			funny_reviewers.insert(r->reviewer);
		}
	}

	{
		std::set<Review_pp> normal_reviews;
		{
			int count = 0;
			for (auto &e : s.rs) {
				++count;
				normal_reviews.insert(e);
				if (count == frevs ) break;
			}
		}

		for (auto &e : funny_reviews) normal_reviews.erase(e);
		
		
		VecMap1_p vec1n;
		auto vecs = populate_vecs(normal_reviews);
		vec1n = std::move(std::get<0>(vecs));
		
		
		{
			std::ofstream v1fo("/home/milano/course/nlp/vec1-normal");
			
			for (auto &v : *vec1n){
				v1fo << v.second << std::endl;
			}
			
			v1fo.close();
		}
	}
}
