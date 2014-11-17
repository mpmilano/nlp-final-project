#pragma once

#include <memory>
#include "Product.hpp"
#include <set>
#include <unordered_map>

class Review;
typedef std::shared_ptr<Review> Review_p;

class Reviewer {

public:
	const std::string profileName;
	const std::string userID;
	const int id;
	std::set<Review_p> reviews;

private:
	 static std::unordered_map<std::string, std::shared_ptr<Reviewer> >& lookup() {
	      static std::unordered_map<std::string, std::shared_ptr<Reviewer> > lookup;
	      return lookup;
	  }

	static int& idr() {static int idr = 0; return idr;}
	
			
	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/) {
		ar & profileName;
		ar & userID;
		ar & id;
		ar & idr;
	}
	
	Reviewer(std::string profileName, std::string userID):profileName(profileName),userID(userID),id(idr()++){}

public:
	static std::shared_ptr<Reviewer> build(const std::string &profilename, const std::string &userid){
		if (lookup().find(userid) != lookup().end()) return lookup().at(userid);
		std::shared_ptr<Reviewer> p(new Reviewer(profilename, userid));
		lookup()[userid] = p;
		return p;
	}

public:
	int compareTo(const Reviewer &o) const {
		return id < o.id ? -1 : id == o.id ? 0 : 1; 
	}

};

typedef std::shared_ptr<Reviewer> Reviewer_p;
