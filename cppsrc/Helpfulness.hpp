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
		return ( ((double) h.first) / h.second) - (((double) this->first) / this->second) > .001  ? true
			: ( ((double) this->first) / this->second) - (((double) h.first) / h.second) > .001 ? false
			: this->first < h.first ? true 
					: this->second < h.second;
	}

	operator double() const { return ((double )votes()) / total();}

	friend std::ostream& operator<<(std::ostream&, const Helpfulness& );
};

std::ostream& operator<<(std::ostream& os, const Helpfulness& h){

	return (os << h.first << "/" << h.second);

}

class Score {

private:
	int s;

public:
	Score(int s):s(s*10){}
	Score(double s):s(s*10){}
	Score():s(0){}
	operator double() const {return s/10;}
	operator int() const {return s/10;}
	friend std::ostream& operator<<(std::ostream&, const Score& );

	template<typename Archive>
	void serialize(Archive &a, const unsigned int ){
		a & s;
	}
	
};

std::ostream& operator<<(std::ostream& os, const Score& s){
	double d = s;
	return (os << d);
}
