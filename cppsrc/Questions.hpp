#pragma once
#include <utility>

typedef ReviewParser<std::istream>::sets sets;

template<int features>
class Questions{
private:
	NLTKInstance &nltk;
	ReviewDB &db;
	std::unique_ptr<ReviewModel<features> > model;
	NLTKInstance::Sentence_Tokenizer tok;
	Review::builder rb;
	Product::builder pb;
	Reviewer::builder rrb;

public:

	void parse(ReviewParser<std::istream>::sets &funny, const std::string &prefix, const std::string &conf){
		ReviewParser<std::istream>::parse(prefix,conf,rrb,pb,rb,funny);
	}

	void initial_parse(ReviewParser<std::istream>::sets &funny, ReviewParser<std::istream>::sets &normal, const std::string &prefix, const std::string &conf, const std::string &nope){
		auto &funny_reviews = funny.rs;
		NLTKInstance::Word_Tokenizer wt(nltk);
		NLTKInstance::Stemmer stemmer(nltk);
		{
			parse(funny, prefix, conf);
			parse(normal, prefix, nope);
			std::cout << "parsing done" << std::endl;
		}
	}
	
	void try_from_db(ReviewParser<std::istream>::sets &funny, ReviewParser<std::istream>::sets &normal, 
					 const std::string &prefix, const std::string &conf, const std::string &nope){
		{
			if (! (ReviewParser<std::istream>::readFromFile(rb,pb,rrb, prefix + conf + "-db", funny) 
				   && ReviewParser<std::istream>::readFromFile(rb, pb, rrb, prefix + nope + "-db", normal)) ) 
			{
				{
					
					funny.clearAll();
					normal.clearAll();
					rb.reset();
					rrb.reset();
					pb.reset();
					ReviewParser<std::istream>::sets oldfunny;
					ReviewParser<std::istream>::sets oldnormal;
					initial_parse(oldfunny,oldnormal, prefix, conf, nope);
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
					
					ReviewParser<std::istream>::writeToFile(prefix + conf + "-db", funny);
					ReviewParser<std::istream>::writeToFile(prefix + nope + "-db", normal);
				}
			}
		}
	}
	
	
	Questions(NLTKInstance &nltk, ReviewDB &db)
		:nltk(nltk),db(db),tok(nltk),rb(tok){}

	double overall_funny(auto &rrs){
		long agg = 0;
		double aggsum = 0;
		for (auto &r : rrs){
			if (r->reviews.size() > 1) continue;
			++agg;
			aggsum += percent_funny(*r);
		}
		return aggsum / agg;
	}

	int count_kindafunny(const auto &rrs){
		int num = 0;
		for (auto &r : rrs){
			auto d = percent_funny(*r);
			if ((d < .95) && (d > .6) ) ++num;
		}
		return num;
	}

	int count_kindaserious(const auto &rrs){
		int num = 0;
		for (auto &r : rrs){
			auto d = percent_funny(*r);
			if ((d < .45) && (d > .05) ) ++num;
		}
		return num;
	}


	int prolific_funny(const auto &rrs){
		uint size = 0;
		for (auto &r : rrs){
			if (percent_funny(*r) > 0.60){
				std::cout << r->reviews.size() << std::endl;
				if (r->reviews.size() > size){
					size = r->reviews.size();
				}
			}
		}
		return size;
	}

	decltype(std::make_pair(0,1.0)) funniest_reviewer(uint threshold, const auto &rrs) {
		double percent = 0;
		uint size = 0;
		std::cout << rrs.size() << std::endl;
		for (auto &r : rrs) {
			if (r->reviews.size() > threshold ) {
				double perc = percent_funny(*r);
				if (perc > percent || (( abs(perc - percent) < .0001) && r->reviews.size() > size)){
					std::cout << "bam!" << perc << " " << percent << std::endl;
					size = r->reviews.size();
					percent = perc;
				}
			}
			else assert(r->reviews.size() < 6);
		}
		
		return std::make_pair(size,percent);
	}
	
	template<typename... Args>
	void init_model(Args... a){
		model.reset(new ReviewModel<features>(a...));
	}

	double percent_funny(const Reviewer &r) {
		assert(model);
		//assert(r.userID != "unknown");
		if (!r.reviews.size()){
			std::cout << "failed on one!" << std::endl;
			return 0;
		}
		sets s;
		db.getAllReviews(rb,pb,rrb,r,s, {" and not reviewer = 5"} );
		long long funny = 0;
		long long normal = 0;
		auto vecs = populate_vecs(s.rs);
		auto &vm = *std::get<0>(vecs);
		for (auto &p : vm){
			auto h = model->predict(p.second).h;
			switch(h){
			case humor::funny : 
				++funny;
				break;
			case humor::normal : 
				++normal;
				break;
			}
		}

		assert((normal + funny) != 0);
		
		return (normal == 0 ? 1 : ((long double) funny) / (funny + normal));
	}
};
