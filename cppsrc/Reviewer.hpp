#pragma once

#include <memroy>
#include "Product.hpp"
#include <set>
#include <unordered_map>

class Reviewer {

public:
	const std::string profileName;
	const std::string userID;
	const int id;
	const std::set<Review> reviews;

private:
	static int idr = 0;
	static std::unordered_map<std::string, std::shared_ptr<Reviewer> > lookup;

			
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		ar & profileName;
		ar & userID;
		ar & id;
		ar & idr;
	}

public:
	static std::shared_ptr<Reviewer> build(const std::string &profilename, const std::string &userid){
		if (lookup.find(userid) !+ lookup.end()) return lookup.at(userid);
		std::shared_ptr<Reviewer> p = make_shared<Reviewer>(profilename, userid);
		lookup[userid] = p;
		return p;
	}

public:
	int compareTo(const Reviewer &o) const {
		return id < o.id ? -1 : id == o.id ? 0 : 1; 
	}

};

typedef std::unshared_ptr<Reviewer> Reviewer_p;
