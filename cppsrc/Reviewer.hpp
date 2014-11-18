#pragma once

#include <memory>
#include "Product.hpp"
#include <set>
#include <unordered_map>
#include "Memoize.hpp"
#include "StringUtils.hpp"
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>

class Review;

class Reviewer;
typedef std::shared_ptr<Reviewer> Reviewer_p;

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
public:	
	class Memo : public ::Memo<std::shared_ptr<Reviewer> > {
		
		bool serialize_called = false;
		const bool from_const = false;
		int id = -1;
	    	std::string profileName;
		std::string userID;
		
		friend class Reviewer;
		friend class boost::serialization::access;
		Memo(int id, std::string pn,std::string u)
			:from_const(true),id(id),profileName(pn),userID(u){}
		
		template<class Archive>
		void serialize(Archive &ar, const unsigned int /*version*/) {
  			ar & boost::serialization::base_object<::Memo<Reviewer_p> > (*this);
			ar & id;
			ar & profileName;
			ar & userID;
			assert (id != -1);
			serialize_called = true;
		}
	public:

		bool operator<(const Memo &o) const {return id < o.id; }

		bool from_archive() const {return serialize_called && (!from_const); }
		bool from_pack() const {return from_const;}

		std::shared_ptr<Reviewer> unpack() const {
			assert(serialize_called);
			assert (id != -1);
			static std::unordered_map<int, Reviewer_p> rm;

			std::cout << "name: " << profileName << std::endl;
			
			if (rm.find(id) != rm.end()){
				return rm.at(id);
			}
			if (failnow){ 
				std::cout << id << std::endl;
				assert(false && "Reviewer not in set?");
			}
			{ std::cout << profileName << ":" << id << std::endl; }
			Reviewer_p ret(new Reviewer(id,profileName,userID));
			lookup()[userID] = ret;
			rm[id] = ret;
			return ret;
		}
		Memo(){}
		
	};
	
private:

	bool generated_id = false;

	Reviewer(int id, std::string profileName, std::string userID):profileName(profileName),userID(userID),id(id){
		assert ( (!generated_id) || (id > idr()));
		idr() += (id - idr() + 1);
	}
	Reviewer(std::string profileName, std::string userID):profileName(profileName),userID(userID),id(++(idr())){
		{ if (id == 11) std::cout << profileName << std::endl;}
		generated_id = true;
	}
	
public:
	
  	Memo_p pack() const {
		return Memo_p(new Memo(id, profileName,userID));
	}
	Memo pod_pack() const { return Memo(id, profileName,userID);}
	
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


BOOST_CLASS_EXPORT_GUID(Reviewer::Memo, "reviewermemo")
