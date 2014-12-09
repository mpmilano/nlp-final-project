#pragma once

#include <memory>
#include "Product.hpp"
#include <set>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <unordered_map>
#include "Memoize.hpp"
#include "StringUtils.hpp"

class Review;

class Reviewer;
typedef std::weak_ptr<Reviewer> Reviewer_p;
typedef std::shared_ptr<Reviewer> Reviewer_pp;

class Reviewer : public Memoizeable<Reviewer_pp > {
	
public:
	const std::string profileName;
	const std::string userID;
	const smart_int id;
	std::set<Review_p> reviews;
;
	
private:

	Reviewer(smart_int id, std::string profileName, std::string userID):profileName(profileName),userID(userID),id(id){}
	
public:

	class Memo;
  	Memo_p pack() const;
	Memo pod_pack() const;

	Reviewer(const Reviewer &) = delete;

	class builder;
	friend class builder;

	int compareTo(const Reviewer &o) const {
		return id < o.id ? -1 : id == o.id ? 0 : 1; 
	}
	
};

