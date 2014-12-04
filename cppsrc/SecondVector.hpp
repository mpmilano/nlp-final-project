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
typedef std::vector< std::pair<int, std::string> > sorted_t;
typedef std::tuple < Review*, obsmap_t, wordset*, sorted_t*> SecondVector_np;


typedef std::unique_ptr<SecondVector_np > SecondVector_nc;
typedef SecondVector_nc SecondVector;

typedef std::unordered_map<Review_p, SecondVector> VecMap2;
typedef std::unique_ptr<VecMap2 > VecMap2_p;

void normalize(int thresh, int over, VecMap2& vm2){
	
	const static std::string other = "<OTHER>";
	
	bool firsttime = true;
	wordset* ws;
	std::set<std::pair<int, std::string> > sorted_counts;
	static sorted_t top_sorted_counts;
	top_sorted_counts.clear();

	for (auto &rv : vm2){
		Review_pp review = rv.first.lock();
		SecondVector_np &vec = *(rv.second);
		std::get<3>(vec) = &top_sorted_counts;
		ws = std::get<2>(vec);
		if (firsttime) {
			(*ws)[other] = thresh;
			firsttime = false;
		}
		obsmap_t obsmap = std::get<1>(vec);
		for (auto &wc : obsmap){
			auto word = wc.first;
			if (ws->find(word) == ws->end() || (ws->at(word) < thresh) || (ws->at(word) > over) ) {
				++(std::get<1>(vec)[other]);
				std::get<1>(vec).erase(word);
				ws->erase(word);
			}
		}
	}
	
	for (auto &wp : *ws) sorted_counts.insert(std::make_pair(-1 * wp.second, wp.first));
	int i = 0;
	for (auto &cnt : sorted_counts) {
		if (cnt.second.length() == 0) continue;
		if (i < 25)
			top_sorted_counts.push_back(std::make_pair(-1 * cnt.first, cnt.second));
		else break;
		++i;
	}

	std::cout << "Top sorted counts: <";
	for (auto &p : top_sorted_counts) std::cout << p.second << ": " << p.first << ", ";
	std::cout << ">";
}

std::ostream& pv2(std::ostream& os, const SecondVector_np &v ){
	int cnt = 0;
	os << "<";
	os << std::get<0>(v)->product->productID << ",";
	for (auto &wm : *(std::get<3>(v))){
		const std::string &w = wm.second;
		if (cnt != 0) os << ", ";
		os << ((std::get<1>(v).find(w) != std::get<1>(v).end() ) ? std::get<1>(v).at(w) : 0);
		++cnt;
	}
	os << ">";
	return os;
}

std::ostream& operator<<(std::ostream& os, const SecondVector_np &v )
{ return pv2(os,v); }
