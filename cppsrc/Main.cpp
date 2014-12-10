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

void initial_parse(NLTKInstance &nltk, ReviewParser<std::istream>::sets &funny, ReviewParser<std::istream>::sets &normal, const std::string &prefix, const std::string &conf, const std::string &nope){
	auto &funny_reviews = funny.rs;
	NLTKInstance::Word_Tokenizer wt(nltk);
	NLTKInstance::Stemmer stemmer(nltk);
	{
		NLTKInstance::Sentence_Tokenizer tok(nltk);
		Reviewer::builder rrb;
		Product::builder pb;
		Review::builder rb(tok);
		ReviewParser<std::istream>::parse(prefix,conf,rrb,pb,rb,funny);
		ReviewParser<std::istream>::parse(prefix,nope,rrb,pb,rb,normal);
		std::cout << "parsing done" << std::endl;
	}
}

void try_from_db(ReviewParser<istream>::sets &funny, ReviewParser<istream>::sets &normal, NLTKInstance& nltk, ReviewDB &db, 
				 const std::string &prefix, const std::string &conf, const std::string &nope){
	{
		NLTKInstance::Sentence_Tokenizer tok(nltk);
		std::unique_ptr<Review::builder> rb(new Review::builder(tok));
		std::unique_ptr<Product::builder> pb(new Product::builder());
		std::unique_ptr<Reviewer::builder> rrb(new Reviewer::builder());

		
		if (! (ReviewParser<istream>::readFromFile(*rb,*pb,*rrb, prefix + conf, funny) 
			   && ReviewParser<istream>::readFromFile(*rb, *pb, *rrb, prefix + nope, normal)) ) 
		{
			rb.reset();
			pb.reset();
			rrb.reset();
			{
				funny.clearAll();
				normal.clearAll();
				ReviewParser<istream>::sets oldfunny;
				ReviewParser<istream>::sets oldnormal;
				initial_parse(nltk,oldfunny,oldnormal, prefix, conf, nope);
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


int main() {


	static const std::string prefix = "/home/milano/course/nlp/data/";
	static const std::string conf = "-funny-confirmed.txt";
	static const std::string nope = "-funny-nope.txt";


	ReviewParser<istream>::sets funny1;
	ReviewParser<istream>::sets normal1;
	ReviewParser<istream>::sets funny2;
	ReviewParser<istream>::sets normal2;
	NLTKInstance nltk;
	ReviewDB db("AmazonReviewsAllCpp");
	try_from_db(funny1,normal1,nltk,db,prefix,str_add("first",conf), str_add("first",nope));
	try_from_db(funny2,normal2,nltk,db,prefix,str_add("second",conf), str_add("second",nope));

	auto funny_train_vecs = populate_vecs(funny1.rs);
	auto normal_train_vecs = populate_vecs(normal1.rs);

	auto &vm1 = *std::get<0>(funny_train_vecs);
	auto &vm2 = *std::get<0>(normal_train_vecs);
	std::cout << "Vectors assembled" << std::endl;

	ReviewModel m(vm1, vm2);
	std::cout << "Model trained" << std::endl;

	
}

