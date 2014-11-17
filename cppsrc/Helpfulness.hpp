#pragma once
#include <utility>

class Helpfulness : public std::pair<int,int> {

public:
	Helpfulness(int a,int b):std::pair<int,int>(a,b){}
	int votes() const {return this->first;}
	int total() const {return this->second;}
	static Helpfulness build(int a, int b){
	    return Helpfulness(a,b);
	}
};
