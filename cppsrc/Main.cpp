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

		std::map<const std::string, bool> done;
		done["xaa"] = true;
		done["xab"] = true;
		done["xac"] = true;
		done["xad"] = true;
		done["xae"] = true;
		done["xaf"] = true;
		done["xag"] = true;
		done["xah"] = true;
		done["xai"] = true;
		done["xaj"] = true;
		done["xak"] = true;
		done["xal"] = true;
		done["xam"] = true;
		done["xan"] = true;

		
		std::atomic_int running(0);

		std::vector<std::future<bool> > futures;

		for (auto &n : names) {
			
			std::shared_ptr<ReviewParser<istream>::sets> s(new ReviewParser<istream>::sets);
			

			ReviewParser<istream>::parse(prefix, n, rrb,pb,rb,*s);

			if (done[n]) continue;

			
			PrintOnce po("waiting for some tasks to finish....");
			while (running > 3){ po.print(std::cout); sleep(2); }

			//if we're lazy rather than async for some reason, launch a task.
			if (futures.size() > 0 && running == 0) futures.back().wait();

			std::cout << "we managed to get through: " << n << std::endl;

			futures.emplace_back(
				std::async
				(std::launch::async, [&running,s,n] ()
				 { 
					 try {
						 std::shared_ptr<ReviewDB<istream> > db(new ReviewDB<istream>(std::string("AmazonReviews") + n));
						 ++running; 
						 auto ret = db->writeToDB(db,s).get(); 
						 --running; 
						 return ret; 
					 }
					 catch(SAUserException e){
						 ReviewDB<istream>::printException(e);
						 std::cout << "exception encountered while working on: " << n << std::endl;
						 --running;
						 return false;
					 }
					 
					 s->clearAll();
					 
				 }
					));
			
			//poor man's synchronization
			sleep(1);
		}
		

		//don't exit until the last task is done!
		for (auto &v : futures){
			v.wait();
		}
	}
	
}
