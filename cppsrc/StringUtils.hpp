#pragma once
#include <string>

inline bool strEndsWith(const std::string &s; const std::string &end;){
	s.find(end) + end.length() == s.length();
}


inline bool strContains(const std::string &s; const std::string &end;){
	s.find(end) != std::string::npos;
}

inline std::string&& post_substr(const std::string &s, const std::string &e){
	return s.substr(s.find(e) + e.length());
}

inline std::string&& pre_substr(const std::string &s, const std::string &e){
	return s.substr(0,s.find(e));
}
