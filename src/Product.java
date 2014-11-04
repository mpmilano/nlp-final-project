import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;


class Product{
	public final String productID;
	public final String title;
	public final double price;
	public final Set<Review> reviews = new TreeSet<Review>();

	private Product(String productID, String title, double price){
		this.productID = productID;
		this.price = price;
		this.title = title;
	}
	
	private static HashMap<String, Product> lookup;
	public static Product build(String productID, String title, double price){
		if (lookup.containsKey(productID)) return lookup.get(productID);
		Product p = new Product(productID, title, price);
		lookup.put(productID, p);
		return p;
	}
}