#pragma once


typedef std::map<std::string, int> wordset;

void wordset_insert(wordset& ws, const std::string &s){
	++(ws[s]);
}

const std::string wordset_find(wordset& ws, const std::string &s){
	return (ws.find(s)->first);
}

typedef std::unique_ptr<wordset > wordset_p;
typedef std::map<const std::string, int> obsmap_t;
typedef std::pair < obsmap_t, wordset*> SecondVector_np;


typedef std::unique_ptr<SecondVector_np > SecondVector_nc;
typedef SecondVector_nc SecondVector;

typedef std::unordered_map<Review_p, SecondVector> VecMap2;
typedef std::unique_ptr<VecMap2 > VecMap2_p;

void normalize(int thresh, VecMap2& vm2){
	
	const static std::string other = "<OTHER>";
	
	bool firsttime = true;

	for (auto &rv : vm2){
		Review_pp review = rv.first.lock();
		SecondVector_np &vec = *(rv.second);
		wordset* ws = vec.second;
		if (firsttime) {
			(*ws)[other] = thresh;
			firsttime = false;
		}
		obsmap_t obsmap = vec.first;
		for (auto &wc : obsmap){
			auto word = wc.first;
			if (ws->find(word) == ws->end() || (ws->at(word) < thresh) ) {
				++(vec.first[other]);
				vec.first.erase(word);
				ws->erase(word);
			}
		}
	}
}

std::ostream& pv2(std::ostream& os, const SecondVector_np &v ){
	int cnt = 0;
	os << "<";
	for (auto &wm : *(v.second)){
		auto &w = wm.first;
		if (cnt != 0) os << ",";
		os << w << ": " << ((v.first.find(w) != v.first.end() ) ? v.first.at(w) : 0);
		++cnt;
	}
	os << ">";
	return os;
}

std::ostream& operator<<(std::ostream& os, const SecondVector_np &v )
{ return pv2(os,v); }
