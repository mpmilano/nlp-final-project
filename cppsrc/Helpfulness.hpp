#pragma once
#include <utility>

class Helpfulness : public std::pair<int,int> {

public:
	Helpfulness():std::pair<int,int>(){}
	Helpfulness(int a,int b):std::pair<int,int>(a,b){}
	int votes() const {return this->first;}
	int total() const {return this->second;}
	static Helpfulness build(int a, int b){
	    return Helpfulness(a,b);
	}

	template<typename Archive>
	void serialize(Archive &a, const unsigned int ){
		a & this->first;
		a & this->second;
	}

	bool operator<(const Helpfulness& h) const {
		return this->first < h.first ? true 
				     : this->second < h.second;
	}
};
