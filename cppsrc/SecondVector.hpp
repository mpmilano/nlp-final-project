#pragma once


typedef std::set<std::string> wordset;
typedef std::unique_ptr<std::set<std::string > > wordset_p;

typedef std::pair < std::map<const std::string* const, int>, wordset*> SecondVector_np;


typedef std::unique_ptr<const SecondVector_np > SecondVector_nc;
typedef SecondVector_nc SecondVector;

typedef std::unordered_map<Review_p, SecondVector> VecMap2;
typedef std::unique_ptr<VecMap2 > VecMap2_p;

std::ostream& pv2(std::ostream& os, const SecondVector_np &v ){
	int cnt = 0;
	os << "<";
	for (auto &w : *(v.second)){
		if (cnt != 0) os << ",";
		os << w << ": " << ((v.first.find(&w) != v.first.end() ) ? v.first.at(&w) : 0);
		++cnt;
	}
	os << ">";
	return os;
}

std::ostream& operator<<(std::ostream& os, const SecondVector_np &v )
{ return pv2(os,v); }
