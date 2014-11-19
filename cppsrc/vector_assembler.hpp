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
		
};

typedef std::map<const std::string, int> SecondVector_np;
typedef std::unique_ptr<const SecondVector_np > SecondVector_nc;
typedef SecondVector_nc SecondVector;

typedef std::unordered_map<Review*, SecondVector> VecMap2;
typedef std::unique_ptr<VecMap2 > VecMap2_p;

SecondVector word_counts(const std::string &s){
	auto retp = new SecondVector_np();
	SecondVector_nc ret(retp);
	auto &map = *retp;
	std::istringstream i(s);
	std::string word;
	while (i >> word){
		++(map[word]);
		word.clear();
	}
	return std::move(ret);
}

typedef std::unordered_map<Review*, FirstVector> VecMap1;
typedef std::unique_ptr<VecMap1 > VecMap1_p;

typedef std::pair<VecMap1_p,VecMap2_p> vecpair;

typedef bool (*pf) (int);

template<pf op>
int count_repeats(const std::list<int> &l){
	int rcounter = 0;
	int prevnum = 0;
	int numreps = 0;
	for (auto &e : l) {
		if (prevnum + 1 == e) ++rcounter;
		else {
			if (op(rcounter) ) ++numreps;
			rcounter = 0;
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

vecpair populate_vecs(const std::set<Review_p> &s){
	VecMap1_p rret(new VecMap1());
	auto& map = *rret;
	VecMap2_p rret2(new VecMap2());
	auto& map2 = *rret2;

	for (auto &rp : s){
		
		const auto &rtext = rp->text;
		auto countres = count_chars<',','.','"','\'',' '>(rtext);
		auto numchars = rtext.size();
		auto numellipse = count_repeats<isthree>(countres['.'].first);
		auto numwords = count_repeats<gtone >(countres[' '].first);
		//this is a rough estimate
		auto numsent = count_repeats<isone>(countres['.'].first) +
				count_repeats<gtone>(countres['!'].first) +
				count_repeats<gtone>(countres['?'].first);
		auto capsp = capscount(rtext);
		auto num2caps = capsp.first;
		auto num3caps = capsp.second;
		map.emplace(rp.get(),FirstVector(countres[','].second / numchars,
						 countres[','].second / numwords,
						 countres[','].second / numsent,
						 countres['.'].second / numchars,
						 countres['.'].second / numwords,
						 countres['.'].second / numsent,
						 countres['"'].second / numchars,
						 countres['"'].second / numwords,
						 countres['"'].second / numsent,
						 countres['\''].second / numchars,
						 countres['\''].second / numwords,
						 countres['\''].second / numsent,
						 numellipse / numchars,
						 numellipse / numwords,
						 numellipse / numsent,
						 numchars,
						 numwords,
						 numsent,
						 num2caps / numwords,
						 num3caps / numwords
				    ));
		map2.emplace(rp.get(),std::move(word_counts(rtext)));
	}

	return std::make_pair(std::move(rret),std::move(rret2));
}
