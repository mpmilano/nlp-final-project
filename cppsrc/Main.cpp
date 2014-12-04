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

using namespace std;
int main() {

	ReviewParser<ifstream>::sets funny;
	ReviewParser<ifstream>::sets normal;
	NLTKInstance nltk;
	NLTKInstance::Word_Tokenizer wt(nltk);
	NLTKInstance::Stemmer stemmer(nltk);

	{
		NLTKInstance::Sentence_Tokenizer tok(nltk);
		Reviewer::builder rrb;
		Product::builder pb;
		Review::builder rb(tok);		
		
		std::string prefix = "/home/milano/course/nlp/data/curated-data/";

		std::vector<std::string> names
			{
				"Books",
					"Clothing",
					"Grocery",
					"Movies",
					"Sports",
					"Toys",
					//first only
					/*
					"Industrial",
					"Music" //*/
					//second-only 
					
					"Electronics",
					"Tools" //*/
					};

		for (auto &w : names){
			ReviewParser<ifstream>::parse(prefix + "second-funny-confirmed/",w,rrb,pb,rb,funny);
			ReviewParser<ifstream>::parse(prefix + "second-funny-nope/",w,rrb,pb,rb,normal);
		}
		
		std::cout << "parsing done" << std::endl;
	}

	decltype(funny.rs) normal_reviews;
	decltype(funny.rs) funny_reviews;
	{
		auto it = normal.rs.begin();
		for (auto &r : funny.rs){
			if (it == normal.rs.end()) break;
			normal_reviews.insert(*it);
			++it;
		}
	}

	{
		auto it = funny.rs.begin();
		for (auto &r : normal_reviews){
			if (it == funny.rs.end()) break;
			funny_reviews.insert(*it);
			++it;
		}
	}

	{
		VecMap1_p vec1;
		VecMap2_p vec2;
		wordset_p words;
		{
			//new scope because we don't want to accidentally use vecs.
			auto vecs = populate_vecs(wt, stemmer, funny_reviews);
			vec1 = std::move(std::get<0>(vecs));
			vec2 = std::move(std::get<1>(vecs));
			words = std::move(std::get<2>(vecs));
		}
		
		VecMap1_p vec1n;
		auto vecs = populate_vecs(wt, stemmer, normal_reviews);
		vec1n = std::move(std::get<0>(vecs));
		auto vec2n = std::move(std::get<1>(vecs));


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
}
