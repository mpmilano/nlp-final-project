#pragma once

class Review {
public:
	const std::string summary;
	const double score;
	const int time;
	const Reviewer reviewer;
	const Helpfulness help;
	const Product product;
	const std::string text;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		ar & summary;
		ar & score;
		ar & time;
		ar & reviewer;
		ar & help;
		ar & product;
		ar & text;
	}
	
private: 
	Review(const std::string summary, 
	       double score, 
	       int time, 
	       const Reviewer& reviewer, 
	       const Helpfulness& help, 
	       const Product& product, 
	       const std::string& text):
		summary(summary),
		score(score),
		time(time),
		reviewer(reviewer),
		help(help),
		product(product),
		text(text){}
	
public: 
	static std::shared_ptr<Review> build(const std::string &summary, 
			    double score, 
			    int time, 
			    Reviewer &reviewer, 
			    const Helpfulness &help, 
			    Product &product, 
			    const std::string &text){
		auto ret = make_shared<Review>(summary, score, time, reviewer, help, product, text);
		reviewer.reviews.add(ret);
		product.reviews.add(ret);
		return ret;
	}

	int compareTo(const Review &o) const {
		return (this == o) ? 0 : score == o.score ? 
			(time == o.time ? 
			 (help.total == o.help.total ? 
			  (help.votes == o.help.votes ? text.compareTo(o.text) : (help.votes < o.help.votes ? -1 : 1))
			  : (help.total < o.help.total ? -1 : 1))
			 : (time < o.time ? -1 : 1))
			: (score < o.score ? -1 : 1);
		
	}
	
};

typedef std:shared_ptr<Review> Review_p;
