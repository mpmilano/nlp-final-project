

int main() {
		
	TreeSet<Reviewer> tsrr = new TreeSet<Reviewer>();
	TreeSet<Product> tsp = new TreeSet<Product>();
	TreeSet<Review> tsr = new TreeSet<Review>();
	
	String prefix = "/home/milano/course/nlp/data/";
	
	ReviewParser.parse(prefix + "Electronics.txt", 
			   tsrr, tsp, tsr);
	ReviewParser.parse(prefix + "Clothing_&_Accessories.txt",tsrr,tsp,tsr);
	ReviewParser.parse(prefix + "Gourmet_Foods.txt",tsrr,tsp,tsr);
}
