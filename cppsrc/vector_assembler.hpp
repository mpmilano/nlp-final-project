#pragma once

#include <string>
#include "Review.hpp"
#include <utility>
#include <map>
#include <set>
#include <sstream>

template<typename T>
constexpr std::pair<bool,T> any_matches(const T& t){
	return std::make_pair(false, t);
}

template<typename T, T m1, T... mo>
constexpr std::pair<bool,T> any_matches(const T& t){
	return t == m1 ? std::make_pair(true,m1) : any_matches<T,mo...>(t);
}

template<char ...m>
auto count_chars(const std::string &s) {
	std::map<char,std::pair<std::list<int>, int> > ret;
	int pos = 0;
	for (auto &c : s){
		auto mp = any_matches<char,m...>(c);
		if (mp.first){
			auto &e = ret[mp.second];
			++(e.second);
			e.first.push_back(pos);
		}
		else ++(ret['o'].second);
		++pos;
	}
	return std::move(ret);
}

struct FirstVector{
	const double commas_char; 
	const double commas_word;
	const double commas_sentence;
	const double periods_chars;
	const double periods_word;
	const double periods_sentence;
	const double quotes_chars;
	const double quotes_word;
	const double quotes_sentence;
	const double apostrophe_chars;
	const double apostrophe_word;
	const double apostrophe_sentence;
	const double ellipse_chars;
	const double ellipse_word;
	const double ellipse_sentence;
	const int numchars;
	const int numwords;
	const int numsent;
	const double percent_smallword_CAPS; //two-or-more characters
	const double percent_bigword_CAPS; //three-or-more characters
	
	FirstVector(	double commas_char, 
			double commas_word,
			double commas_sentence,
			double periods_chars,
			double periods_word,
			double periods_sentence,
			double quotes_chars,
			double quotes_word,
			double quotes_sentence,
			double apostrophe_chars,
			double apostrophe_word,
			double apostrophe_sentence,
			double ellipse_chars,
			double ellipse_word,
			double ellipse_sentence,
			int numchars,
			int numwords,
			int numsent,
			double percent_smallword_CAPS, 
			double percent_bigword_CAPS)
	:commas_char(commas_char), 
		commas_word(commas_word),
		commas_sentence(commas_sentence),
		periods_chars(periods_chars),
		periods_word(periods_word),
		periods_sentence(periods_sentence),
		quotes_chars(quotes_chars),
		quotes_word(quotes_word),
		quotes_sentence(quotes_sentence),
		apostrophe_chars(apostrophe_chars),
		apostrophe_word(apostrophe_word),
		apostrophe_sentence(apostrophe_sentence),
		ellipse_chars(ellipse_chars),
		ellipse_word(ellipse_word),
		ellipse_sentence(ellipse_sentence),
		numchars(numchars),
		numwords(numwords),
		numsent(numsent),
		percent_smallword_CAPS(percent_smallword_CAPS), 
		percent_bigword_CAPS(percent_bigword_CAPS){}

	friend std::ostream& operator<<(std::ostream&, const FirstVector& );
		
};

std::ostream& operator<<(std::ostream& os, const FirstVector& h){

	return (os << "<" << h.commas_char << "," <<
		h.commas_word << "," <<
		h.commas_sentence << "," << 
		h.periods_chars << "," << 
		h.periods_word << "," << 
		h.periods_sentence << "," << 
		h.quotes_chars << "," << 
		h.quotes_word << "," << 
		h.quotes_sentence << "," << 
		h.apostrophe_chars << "," << 
		h.apostrophe_word << "," << 
		h.apostrophe_sentence << "," << 
		h.ellipse_chars << "," << 
		h.ellipse_word << "," << 
		h.ellipse_sentence << "," << 
		h.numchars << "," << 
		h.numwords << "," << 
		h.numsent << "," << 
		h.percent_smallword_CAPS  << "," << 
		h.percent_bigword_CAPS << ">");

}

typedef std::map<const std::string* const, int> SecondVector_np;


typedef std::unique_ptr<const SecondVector_np > SecondVector_nc;
typedef SecondVector_nc SecondVector;

typedef std::unordered_map<Review_p, SecondVector> VecMap2;
typedef std::unique_ptr<VecMap2 > VecMap2_p;

std::ostream& operator<<(std::ostream& os, const SecondVector_np& ){
	return os;
}

SecondVector word_counts(std::set<std::string> &words, const std::string &s){
	auto retp = new SecondVector_np();
	SecondVector_nc ret(retp);
	auto &map = *retp;
	std::istringstream i(s);
	std::string w;
	while (i >> w){
		words.insert(w);
		auto *word = &(*(words.find(w)));
		{ ++(map[word]);
			w.clear();
		}
	}
	return std::move(ret);
}

typedef std::unordered_map<Review_p, FirstVector> VecMap1;
typedef std::unique_ptr<VecMap1 > VecMap1_p;

typedef std::set<std::string> wordset;
typedef std::unique_ptr<std::set<std::string > > wordset_p;

typedef std::tuple<VecMap1_p,VecMap2_p, wordset_p > vec_tuple;

typedef bool (*pf) (int);

template<pf op>
int count_repeats(const std::list<int> &l){
	if (l.size() == 0) return 0;
	int rcounter = 1;
	int prevnum = 0;
	int numreps = 0;
	for (auto &e : l) {
		if (prevnum + 1 == e) ++rcounter;
		else {
			if (op(rcounter) ) ++numreps;
			rcounter = 1;
		}
	}
	return numreps;
}

auto count_caps(const std::string &s){
	std::list<std::string> ret;
	std::string acc = "";
	for (auto &c : s){
		if (isupper(c)){
			acc += c;
		}
		else if (isspace(c)){
			if (acc != "") ret.push_back(acc);
			acc = "";
		}
		else acc = "";
	}
	return ret;
}

std::pair<int,int> capscount(const std::string &s){
	auto l = count_caps(s);
	int cnt3 = 0;
	int cnt2 = 0;

	for (auto &w : l){
		auto s = w.length();
		if (s > 2) ++cnt3;
		if (s > 1) ++cnt2;
	}
	return std::make_pair(cnt2,cnt3);
}

//stupid constexpr restrictions 

bool isone(int x) { return x == 1; }
bool isthree(int x) { return x == 3; }
bool gtone(int x) {return x > 1; }
bool gtz(int x) {return x > 0; }

template<typename T> 
vec_tuple populate_vecs(const T &s){
	VecMap1_p rret(new VecMap1());
	auto& map = *rret;
	VecMap2_p rret2(new VecMap2());
	auto& map2 = *rret2;
	wordset_p rwords(new wordset());
	auto& words = *rwords;

	for (auto &rp : s){
		
		const auto &rtext = rp->text;
		auto countres = count_chars<',','.','"','\'',' ','?','!'>(rtext);
		auto numchars = rtext.size();
		auto numellipse = count_repeats<isthree>(countres['.'].first);
		auto numwords = 1 + count_repeats<gtz >(countres[' '].first);
		//this is a rough estimate
		auto numsent = count_repeats<isone>(countres['.'].first) +
				count_repeats<gtz>(countres['!'].first) +
				count_repeats<gtz>(countres['?'].first) + 1;
		{
			if (numwords == 0 || numsent == 0) {
				std::cout << *rp << std::endl;
				std::cout << countres['!'].first.size() << std::endl;
			}
			assert(numchars != 0);
			assert (numwords != 0);
			assert(numsent > 0);
		}

		auto capsp = capscount(rtext);
		auto num2caps = capsp.first;
		auto num3caps = capsp.second;
		
		map.emplace(rp,FirstVector(((double) countres[','].second) / numchars,
					   ((double) countres[','].second) / numwords,
					   ((double) countres[','].second) / numsent,
					   ((double) countres['.'].second) / numchars,
					   ((double) countres['.'].second) / numwords,
					   ((double) countres['.'].second) / numsent,
					   ((double) countres['"'].second) / numchars,
					   ((double) countres['"'].second) / numwords,
					   ((double) countres['"'].second) / numsent,
					   ((double) countres['\''].second) / numchars,
					   ((double) countres['\''].second) / numwords,
					   ((double) countres['\''].second) / numsent,
					   ((double) numellipse) / numchars,
					   ((double) numellipse) / numwords,
					   ((double) numellipse) / numsent,
					   ((double) numchars),
					   numwords,
					   numsent,
					   ((double) num2caps) / numwords,
					   ((double) num3caps) / numwords
				    ));
		map2.emplace(rp,std::move(word_counts(words,rtext)));
	}

	return std::make_tuple(std::move(rret),std::move(rret2),std::move(rwords));
}
