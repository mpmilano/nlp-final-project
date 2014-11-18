#include <set>
#include <string>
#include "Review.hpp"
#include "Product.hpp"
#include "Reviewer.hpp"
#include "ReviewParser.hpp"
#include <fstream>
#include <unistd.h>
#include <vector>

using namespace std;
int main() {
	
	ReviewParser<ifstream>::sets s;
	
	vector<string> names = {
			"Electronics.txt", 
			"Clothing_&_Accessories.txt", 
			"Gourmet_Foods.txt",
			"all-head.txt"
		};
	
	std::string prefix = "/home/milano/course/nlp/data/";
	
	for (auto& endfix : names){
		ReviewParser<ifstream>::parse(prefix + endfix,s);
	}
	Product::constructionDone();
	Review::constructionDone();
	Reviewer::constructionDone();
	sleep(12);

}
