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
#include "linear.h"
#include "tron.h"

void initial_parse(NLTKInstance &nltk, ReviewParser<std::istream>::sets &funny, ReviewParser<std::istream>::sets &normal){
	auto &funny_reviews = funny.rs;
	NLTKInstance::Word_Tokenizer wt(nltk);
	NLTKInstance::Stemmer stemmer(nltk);
	{
		NLTKInstance::Sentence_Tokenizer tok(nltk);
		Reviewer::builder rrb;
		Product::builder pb;
		Review::builder rb(tok);
		std::string prefix = "/home/milano/course/nlp/data/";
		ReviewParser<std::istream>::parse(prefix,"first-funny-confirmed.txt",rrb,pb,rb,funny);
		ReviewParser<std::istream>::parse(prefix,"first-funny-nope.txt",rrb,pb,rb,normal);
		std::cout << "parsing done" << std::endl;
	}
}

using namespace std;
int main() {

	ReviewParser<istream>::sets funny;
	ReviewParser<istream>::sets normal;
	NLTKInstance nltk;	
	ReviewDB<std::istream> db("AmazonReviewsAllCpp");

	{

		ReviewParser<istream>::sets oldfunny;
		ReviewParser<istream>::sets oldnormal;

		initial_parse(nltk,funny,normal);
				
		NLTKInstance::Sentence_Tokenizer tok(nltk);
		Review::builder rb(tok);
		Product::builder pb;
		Reviewer::builder rrb;
		db.getAllReviews(rb, pb, rrb, oldfunny.rs, funny, [](auto &r){return r->text.get(); });
		

	}


}

