#pragma once
#include "Reviewer.hpp"

class Reviewer::builder{
	friend class Reviewer;
	std::unordered_map<smart_int, Reviewer_pp> rm;
	std::unordered_map<std::string, Reviewer_pp > lookup;
	smart_int idr;
	static plain_ptr<builder>& b() {static plain_ptr<builder> b(nullptr); return b;}
	
public:
	builder():idr(0){ assert(b() == nullptr); b() = this; }
	virtual ~builder() {b() = nullptr; std::cout << "builder done" << std::endl;}
	
	Reviewer_pp build(const std::string &profilename, const std::string &userid){
		if (lookup.find(userid) != lookup.end()) return lookup.at(userid);
		Reviewer_pp p(new Reviewer(++idr,profilename, userid));
		lookup[userid] = p;
		return p;
	}
};
