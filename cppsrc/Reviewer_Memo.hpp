#pragma once
#include "Reviewer_builder.hpp"

	
class Reviewer::Memo : public ::Memo<Reviewer_pp > {		
	
	bool serialize_called = false;
	const bool from_const = false;
	std::string profileName;
	std::string userID;
	
	friend class Reviewer;
	friend class boost::serialization::access;
	Memo(smart_int id, std::string pn,std::string u)
		:from_const(true),profileName(pn),userID(u),id(id){}
	
	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/) {
		ar & id;
		ar & profileName;
		ar & userID;
		assert (id != -1);
		serialize_called = true;
	}
public:
	
	smart_int id;
	
	bool operator<(const Memo &o) const {return id < o.id; }
	
	bool from_archive() const {return serialize_called && (!from_const); }
	bool from_pack() const {return from_const;}
	
	Reviewer_pp unpack() const {
		assert(serialize_called);
		assert (id != -1);	
		auto b = builder::b();
		if (b->rm.find(id) != b->rm.end()){
			return b->rm.at(id);
		}
		Reviewer_pp ret(new Reviewer(id,profileName,userID));
		b->lookup[userID] = ret;
		b->rm[id] = ret;
		if (id > b->idr) b->idr = id;
		return ret;
	}
	Memo():id(-1){}
	
	Memo(smart_int id):serialize_called(true),id(id){}
	
};

Reviewer::Memo_p Reviewer::pack() const {
	return Memo_p(new Memo(id, profileName,userID));
}
Reviewer::Memo Reviewer::pod_pack() const { return Memo(id, profileName,userID);}


BOOST_CLASS_EXPORT_GUID(Reviewer::Memo, "reviewermemo")
