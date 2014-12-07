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
			"xbc",

		};

		std::map<const std::string, bool> done;		
		
		std::atomic_int running(0);

		std::vector<std::future<bool> > futures;

		for (auto &n : names) {
			
			std::shared_ptr<ReviewParser<istream>::sets> s(new ReviewParser<istream>::sets);
			

			ReviewParser<istream>::parse(prefix, n, rrb,pb,rb,*s);

			if (done[n]) continue;

			PrintOnce po("waiting for some tasks to finish....");	
			while (CurrentMemUsage::getValue() > 13500000l && running > 0) { po.print(std::cout); sleep(2); }

			//if we're lazy rather than async for some reason, launch a task.
			if (futures.size() > 0 && running == 0) futures.back().wait();

			std::cout << "we managed to get through: " << n << std::endl;

			std::shared_ptr<BeginEndMessage> msg(new BeginEndMessage(std::string("launching thread for ") + n, 
																	 std::string("destroying thread for ")+n));

			futures.emplace_back(
				std::async
				(std::launch::async, [&running,s,n,msg] () mutable
				 {
					 auto sl = s;
					 auto msgl = msg;
					 s.reset();
					 msg.reset();
					 {
						 auto s = sl;
						 auto msg = msgl;
						 sl.reset();
						 msgl.reset();
						 msg->printFirst();
						 try {
							 std::shared_ptr<ReviewDB<istream> > db(new ReviewDB<istream>(std::string("AmazonReviews") + n));
							 ++running; 
							 DecrOnDelete<decltype(running)> dod(running);
							 auto ret = db->writeToDB(db,s).get();
							 s->clearAll();
							 return ret; 
						 }
						 catch(SAUserException e){
							 ReviewDB<istream>::printException(e);
							 std::cout << "exception encountered while working on: " << n << std::endl;
							 s->clearAll();
							 return false;
						 }
					 }
				 }
					));
			
			//poor man's synchronization
			sleep(1);
		}

		std::cout << "Waiting on some last threads..." << std::endl;
		

		//don't exit until the last task is done!
		for (auto &v : futures){
			v.wait();
		}
	}
	
}
