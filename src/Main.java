import java.util.TreeSet;

class Main {

	public static void main(String[] args) throws Exception{

		TreeSet<Reviewer> tsrr = new TreeSet<Reviewer>();
		TreeSet<Product> tsp = new TreeSet<Product>();
		new TreeSet<Review> tsr= new TreeSet<Review>();		

		ReviewParser.parse("/home/milano/course/nlp/data/Electronics.txt", 
				   tsrr, tsp, tsr);
	}
}
