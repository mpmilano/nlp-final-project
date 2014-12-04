#pragma once

#include <string>
#include "Review.hpp"
#include <utility>
#include <map>
#include <set>
#include <sstream>
#include <functional>
#include "FirstVector.hpp"
#include "SecondVector.hpp"
#include <array>

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
	std::array<std::pair<std::list<int>, int>, 256 > ret;
	int pos = 0;
	for (auto &c : s){
		auto mp = any_matches<char,m...>(c);
		if (mp.first){
			auto &e = ret[mp.second];
			++(e.second);
			e.first.push_back(pos);
		}
		else ++(ret['o'].second);
		if (ispunct(c)) ++(ret['p'].second);
		if (isupper(c)) ++(ret['u'].second);
		++pos;
	}
	return std::move(ret);
}

SecondVector word_counts(Review* rp, NLTKInstance::Stemmer &stemmer, NLTKInstance::Word_Tokenizer& wt,  wordset *words, const std::string &s){

	auto retp = new SecondVector_np();
	std::get<0>(*retp) = rp;
	assert(std::get<0>(*retp) == rp);
	std::get<2>(*retp) = words;
	SecondVector_nc ret(retp);
	auto &map = std::get<1>(*retp);
	for (const auto &wp : wt.tokenize<std::list<std::string>, std::string>(s) ){
		const auto w = stemmer.stem(wp);
		wordset_insert(*words,w);
		auto word = wordset_find(*words,w);
		++(map[word]);
	}
	return std::move(ret);
}

typedef std::unordered_map<Review_p, FirstVector> VecMap1;
typedef std::unique_ptr<VecMap1 > VecMap1_p;


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

std::pair<int,int> capscount(const std::string &s){
	auto count_caps = [](const std::string &s){
		std::list<std::string> ret;
		std::string acc = "";
		for (auto &c : s){
			if (isupper(c)){
				acc += c;
			}
			else if (!isalnum(c)){
				if (acc != "") ret.push_back(acc);
				acc = "";
			}
			else acc = "";
		}
		return ret;
	};
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
vec_tuple populate_vecs(NLTKInstance::Word_Tokenizer &wt, NLTKInstance::Stemmer &stemmer, const T &s){
	VecMap1_p rret(new VecMap1());
	auto& map = *rret;
	VecMap2_p rret2(new VecMap2());
	auto& map2 = *rret2;
	wordset_p rwords(new wordset());
	auto& words = *rwords;

	for (auto &rp : s) {
		
		const auto &rtext = rp->text;
		auto countres = count_chars<',','.','"','\'',' ','?','!'>(rtext);
		auto numchars = rtext.get().size();
		auto numellipse = count_repeats<isthree>(countres['.'].first);
		auto numwords = 1 + count_repeats<gtz >(countres[' '].first);

		auto numsent = rp->sentences.size();
		{
			if (numwords == 0 || numsent == 0) {
				std::cout << *rp << std::endl;
				std::cout << countres['!'].first.size() << std::endl;
			}
			assert(numchars != 0);
			assert (numwords != 0);
			assert(numsent > 0);
		}

		auto numcaps = countres.at('u').second;
		auto allpunct = ((double) (countres['p'].second - (numellipse * 3))) / numchars;

		//	token-stemmer? 
		//	stop-words? 
			
		
		map.emplace(rp,FirstVector(*rp,
						((double) countres[','].second) / numchars,
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
					   ((double) numcaps) / numchars,
					   fabs( (rp->help.operator double()) - rp->score),
					   rp->help,
					   rp->score, 
								   allpunct,
								   rp->product->productType
				    ));
		map2.emplace(rp,std::move(word_counts(rp.get(),stemmer, wt, &words,rtext)));
	}

	normalize(20,500, map2 );

	return std::make_tuple(std::move(rret),std::move(rret2),std::move(rwords));
}
