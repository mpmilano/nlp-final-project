#pragma once
#include <string>
#include <iostream>
#include <math.h>

inline bool strEndsWith(const std::string &s, const std::string &end){
	return s.find(end) + end.length() == s.length();
}


inline bool strContains(const std::string &s, const std::string &end){
	return s.find(end) != std::string::npos;
}

inline std::string post_substr(const std::string &s, const std::string &e){
	if (s.find(e) + e.length() > s.length()){
		std::cerr << s << std::endl;
		std::cerr << e << std::endl;
		assert(false && "post_substr called when there was nothing post-substr...");
	}
	return s.substr(s.find(e) + e.length());
}

inline std::string pre_substr(const std::string &s, const std::string &e){
	return s.substr(0,s.find(e));
}

inline std::string strReplace (const std::string &s, char o, char n){
	std::string ret = s;
	std::string::size_type pos = 0;
	while (pos != std::string::npos){
		pos = ret.find_first_of(o,pos == 0? pos : pos + 1);
		ret[pos] = n;
		
	}
	return ret;
}

template<typename T>
struct plain_ptr {
	T* p;
	plain_ptr(T *t):p(t){}
	plain_ptr operator=( T* t){return p = t;}
	operator T* () {return p;}
	T* operator -> (){return p;}

};


struct smart_int {
	typedef unsigned long long t;
	t i;

	template<typename Archive>
	void serialize(Archive &a, const unsigned int ){
		a & i;
	}

	smart_int(t i):i(i){}
	smart_int(const smart_int& j):i(j.i){}

	static auto safe_add(const t &a, const t &b ){
		auto ab = log2l(a), bb = log2l(b);
		return (ab < 32 && bb < 32);
	}

	auto operator++(){
		auto tmp = i; auto ret = (++i); 
		assert(tmp < ret); return ret;
	}

	auto operator>(const smart_int &o) const{
		return i > o.i;
	}
	auto operator<(const smart_int &o) const{
		return i < o.i;
	}

	auto operator-(const smart_int &o) const{
		if (*this > o) return i - o.i; 
		assert(safe_add(i,-1 * o.i));
		return i - o.i;
	}

	auto operator+=(const smart_int& o){
		assert(safe_add(i,o.i));
		return i += o.i;
	}

	auto operator!=(const smart_int &o) const {
		return i != o.i;
	}

	auto operator==(const smart_int &o) const {
		return i == o.i;
	}

	auto operator!=(int o) const {
		t t = o;
		return i != t;
	}
};
namespace std {
	template <>
	struct hash<smart_int>
	{
		std::size_t operator()(const smart_int& i) const
			{
				return std::hash<smart_int::t>().operator()(i.i);
			}
	};
	
}
