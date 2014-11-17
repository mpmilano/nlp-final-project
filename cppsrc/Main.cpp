#include <set>
#include <string>
#include "Review.hpp"
#include "Product.hpp"
#include "Reviewer.hpp"
#include "ReviewParser.hpp"
#include <fstream>

using namespace std;
int main() {
		
	std::set<Reviewer_p> tsrr;
	std::set<Product_p> tsp;
	std::set<Review_p> tsr;
	
	std::string prefix = "/home/milano/course/nlp/data/";
	
	ReviewParser<ifstream>::parse(prefix + "Electronics.txt", 
			   tsrr, tsp, tsr);
	ReviewParser<ifstream>::parse(prefix + "Clothing_&_Accessories.txt",tsrr,tsp,tsr);
	ReviewParser<ifstream>::parse(prefix + "Gourmet_Foods.txt",tsrr,tsp,tsr);
}
