import java.util.*;

class Main {
	
	SetMap<Product,Product> convert (HashMap<String,Set<String> > old) {
		SetMap<Product,Product> sm = new SetMap<Product,Product>();
		for (String k : old.keySet()){
			Product p = Product.build(k);
			if (p != null){
				TreeSet<Product> ts = new TreeSet<Product>();
				for (String v : old.get(k)){
					Product pv = Product.build(v);
					if (pv != null) ts.add(pv);
				}
				sm.put(p,ts);
			}
		}
		return sm;
	}

	public static void main(String[] args) throws Exception{

		TreeSet<Reviewer> tsrr = new TreeSet<Reviewer>();
		TreeSet<Product> tsp = new TreeSet<Product>();
		TreeSet<Review> tsr = new TreeSet<Review>();

		String prefix = "/home/milano/course/nlp/data/";

		ReviewParser.parse(prefix + "Electronics.txt", 
				   tsrr, tsp, tsr);
		ReviewParser.parse(prefix + "Clothing_&_Accessories.txt",tsrr,tsp,tsr);
		ReviewParser.parse(prefix + "Gourmet_Foods.txt",tsrr,tsp,tsr);

		

		
	}
}
