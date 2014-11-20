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
	std::set<Product_p> funny_prods;
	{
		Reviewer::builder rrb;
		Product::builder pb;
		Review::builder rb;
		
		vector<string> names = {
			"Sports_&_Outdoors.txt",
			"Tools_&_Home_Improvement.txt",
			"Toys_&_Games.txt",
			"Health.txt",
			"Electronics.txt", 
			"Clothing_&_Accessories.txt", 
			"Gourmet_Foods.txt",
			"all-head.txt",
			"Home_&_Kitchen.txt",
			"Video_Games.txt",
			"Baby.txt",
			"Automotive.txt"
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

	int numfunny = 0;
	for (auto &e : funny_prods) {
		for (Review *r : e.lock()->reviews){
			++numfunny;
		}
	}

	std::cout << numfunny << std::endl;
	return 0;

	VecMap1_p vec1;
	VecMap2_p vec2;
	wordset_p words;
	{
		//new scope because we don't want to accidentally use vecs.
		auto vecs = populate_vecs(s.rs);
		vec1 = std::move(std::get<0>(vecs));
		vec2 = std::move(std::get<1>(vecs));
		words = std::move(std::get<2>(vecs));
	}

}
