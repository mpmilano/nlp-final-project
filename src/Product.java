import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;


class Product implements Comparable<Product>{
	public final String productID;
	public final int id;
	public final String title;
	public final double price;
	public final Set<Review> reviews = new TreeSet<Review>();

	
	private static int idr = 0;
	private Product(String productID, String title, double price){
		this.productID = productID;
		this.price = price;
		this.title = title;
		id = idr++;
	}
	
	private static HashMap<String, Product> lookup = new HashMap<String, Product>();
	public static Product build(String productID, String title, double price){
		if (lookup.containsKey(productID)) return lookup.get(productID);
		Product p = new Product(productID, title, price);
		lookup.put(productID, p);
		return p;
	}
	@Override
	public int compareTo(Product o) {
		// TODO Auto-generated method stub
		return (this == o) ? 0 : (id < o.id ? -1 : id == o.id ? 0 : 1);
	}
}