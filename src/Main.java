import java.util.TreeSet;

class Main {

	public static void main(String[] args) throws Exception{
		ReviewParser.parse("/home/milano/course/nlp/data/all.txt", 
				   new TreeSet<Reviewer>(), 
				   new TreeSet<Product>(), 
				   new TreeSet<Review>() );
	}

}
