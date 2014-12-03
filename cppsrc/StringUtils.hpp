#pragma once
#include <string>
#include <cstddef>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>
#include <math.h>

class Review;
typedef std::weak_ptr<Review> Review_p;

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

template<typename Collection> 
Collection inline StringSplit(const std::string &source, const char *delimiter = " ", bool keepEmpty = false)
{
	Collection results;

	size_t prev = 0;
	size_t next = 0;

	while ((next = source.find_first_of(delimiter, prev)) != std::string::npos)
	{
		if (keepEmpty || (next - prev != 0))
		{
			results.push_back(source.substr(prev, next - prev));
		}
		prev = next + 1;
	}

	if (prev < source.size())
	{
		results.push_back(source.substr(prev));
	}

	return results;
}

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

	auto asInt() const {return i;}

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
		if (i >= o.i) return i - o.i; 
		assert(safe_add(i,-1 * o.i));
		return i - o.i;
	}

	auto operator=(const smart_int& o){
		return i = o.i;
	}

	auto operator+(int o){ 
		assert(safe_add(i,o));
		return i + o;
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

	template <typename T>
	struct hash<std::weak_ptr<T> >
	{
		std::size_t operator()(const std::weak_ptr<T>& i) const
			{
				return std::hash<std::shared_ptr<T> >().operator()(i.lock());
			}
	};

}

template<typename T>
bool operator==(const std::weak_ptr<T> &a, const std::weak_ptr<T> &b){
	return a.lock() == b.lock();
}

template<typename T>
std::shared_ptr<T> lock(const std::weak_ptr<T> &p){
	return p.lock();
}

template<typename T>
std::shared_ptr<T> lock(const std::shared_ptr<T>& t){
	return t;
}

namespace bip = boost::interprocess;

struct membuf : std::streambuf
{
    membuf(char const* const buf, std::size_t const size)
		{
			char* const p = const_cast<char*>(buf);
			this->setg(p, p, p + size);
		}
};

struct imemstream : virtual membuf, std::istream
{
    imemstream(char const* const buf, std::size_t const size)
		: membuf(buf, size),
		  std::istream(static_cast<std::streambuf*>(this))
		{ }

	imemstream(const imemstream&) = delete;
};

class mmapStream {

private:
	bip::file_mapping mapping;
	bip::mapped_region rgn;
	char const* const mmaped_data;
	std::size_t const mmap_size;

public:

	imemstream s;

	mmapStream(const std::string &filename)
		:mapping(filename.c_str(), bip::read_only),
		 rgn(mapping, bip::read_only),
		 mmaped_data(static_cast<char*>(rgn.get_address())),
		 mmap_size(rgn.get_size()),
		 s(mmaped_data, mmap_size){}

};

