#pragma once

#include <memory>
#include "Product.hpp"
#include <set>
#include <unordered_map>
#include "Memoize.hpp"

class Review;

class Reviewer : public Memoizeable<std::shared_ptr<Reviewer> >{

public:
	const std::string profileName;
	const std::string userID;
	const int id;
	std::set<Review*> reviews;

private:
	 static std::unordered_map<std::string, std::shared_ptr<Reviewer> >& lookup() {
	      static std::unordered_map<std::string, std::shared_ptr<Reviewer> > lookup;
	      return lookup;
	  }

	static int& idr() {static int idr = 0; return idr;}
	
	  class Memo : public ::Memo<std::shared_ptr<Reviewer> > {
	    	std::string profileName;
		std::string userID;
		
		friend class Reviewer;
		Memo(std::string pn,std::string u):profileName(pn),userID(u){}
		
		  template<class Archive>
		  void serialize(Archive &ar, const unsigned int /*version*/) {
		  ar & profileName;
		  ar & userID;
		}
		
		std::shared_ptr<Reviewer> unpack() const {
		    return build(profileName,userID);
		}

	};
			
	
	Reviewer(std::string profileName, std::string userID):profileName(profileName),userID(userID),id(idr()++){}

public:
  
  	Memo_p pack() const {
	  return Memo_p(new Memo(profileName,userID));
	}
	
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
