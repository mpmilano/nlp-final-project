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
	const int id;
	std::set<Review*> reviews;

public:	
	class Memo : public ::Memo<Reviewer_pp > {		

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
			ar & id;
			ar & profileName;
			ar & userID;
			assert (id != -1);
			serialize_called = true;
		}
	public:
		int gid() { return id;}

		bool operator<(const Memo &o) const {return id < o.id; }

		bool from_archive() const {return serialize_called && (!from_const); }
		bool from_pack() const {return from_const;}

		Reviewer_pp unpack() const {
			assert(serialize_called);
			assert (id != -1);	
			if (builder::b()->rm.find(id) != builder::b()->rm.end()){
				return builder::b()->rm.at(id);
			}
			Reviewer_pp ret(new Reviewer(id,profileName,userID));
			builder::b()->lookup[userID] = ret;
			builder::b()->rm[id] = ret;
			return ret;
		}
		Memo(){}

		Memo(int id):serialize_called(true),id(id){}
		
	};
	
private:

	bool generated_id = false;

	Reviewer(int id, std::string profileName, std::string userID):profileName(profileName),userID(userID),id(id){
		assert ( (!generated_id) || (id > builder::b()->idr));
		builder::b()->idr += (id - builder::b()->idr + 1);
	}
	Reviewer(std::string profileName, std::string userID):profileName(profileName),userID(userID),id(++(builder::b()->idr)){
		generated_id = true;
	}
	
public:
	
  	Memo_p pack() const {
		return Memo_p(new Memo(id, profileName,userID));
	}
	Memo pod_pack() const { return Memo(id, profileName,userID);}
	
	
public:
	friend class builder;
	class builder{
		friend class Reviewer;
		std::unordered_map<int, Reviewer_pp> rm;
		std::unordered_map<std::string, Reviewer_pp > lookup;
		int idr = 0;
		static plain_ptr<builder>& b() {static plain_ptr<builder> b(nullptr); return b;}

	public:
		builder(){ assert(b() == nullptr); b() = this; }
		virtual ~builder() {b() = nullptr; std::cout << "builder done" << std::endl;}

		Reviewer_pp build(const std::string &profilename, const std::string &userid){
			if (lookup.find(userid) != lookup.end()) return lookup.at(userid);
			Reviewer_pp p(new Reviewer(profilename, userid));
			lookup[userid] = p;
			return p;
		}
	};

	int compareTo(const Reviewer &o) const {
		return id < o.id ? -1 : id == o.id ? 0 : 1; 
	}
	
};

BOOST_CLASS_EXPORT_GUID(Reviewer::Memo, "reviewermemo")
