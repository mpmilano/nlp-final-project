#include <pair>

class Helpfulness : public std::pair<int,int> {

public:
	int votes(){return this->first;}
	int total(){return this->second;}
};
