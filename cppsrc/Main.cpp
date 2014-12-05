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

	NLTKInstance nltk;

	{
		NLTKInstance::Sentence_Tokenizer tok(nltk);
		Reviewer::builder rrb;
		Product::builder pb;
		Review::builder rb(tok);		
		
		std::string prefix = "/home/milano/course/nlp/data/fragmented/";
		auto names = {
			"xaa",
			"xab",
			"xac",
			"xad",
			"xae",
			"xaf",
			"xag",
			"xah",
			"xai",
			"xaj",
			"xak",
			"xal",
			"xam",
			"xan",
			"xao",
			"xap",
			"xaq",
			"xar",
			"xas",
			"xat",
			"xau",
			"xav",
			"xaw",
			"xax",
			"xay",
			"xaz",
			"xba",
			"xbb",
			"xbc",
			"xbd",
			"xbe",
			"xbf",
			"xbg",
			"xbh",
			"xbi"
		};
		
		std::atomic_int running(0);

		std::vector<std::future<bool> > futures;

		for (auto &n : names) {
			
			std::shared_ptr<ReviewParser<istream>::sets> s(new ReviewParser<istream>::sets);
			
			//don't overload RAM
			while (running > 2) sleep(2);
			
			ReviewParser<istream>::parse(prefix, n, rrb,pb,rb,*s);

			//if we're lazy rather than async for some reason, launch a task.
			if (futures.size() > 0 && running == 0) futures.back().get();

			std::cout << "we managed to get through: " << n << std::endl;

			std::shared_ptr<ReviewDB<istream> > db(new ReviewDB<istream>(std::string("AmazonReviews") + n));
			futures.emplace_back(std::async(std::launch::async, [&running,db,s] (){ ++running; auto ret = db->writeToDB(db,s)(); --running; return ret;}));
			
			//poor man's synchronization
			sleep(1);
		}
		

		//don't exit until the last task is done!
		for (auto &v : futures){
			v.wait();
		}
	}
	
}
