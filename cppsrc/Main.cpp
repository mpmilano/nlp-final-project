#include <set>
#include <string>
#include "Review.hpp"
#include "Product.hpp"
#include "Reviewer.hpp"
#include "ReviewParser.hpp"
#include <fstream>

using namespace std;
int main() {
	
	ReviewParser<ifstream>::sets s;

	std::string prefix = "/home/milano/course/nlp/data/";
	
	ReviewParser<ifstream>::parse(prefix + "Electronics.txt", s);
	//ReviewParser<ifstream>::parse(prefix + "Clothing_&_Accessories.txt",s);
	//ReviewParser<ifstream>::parse(prefix + "Gourmet_Foods.txt",s);
}
