import java.util.Set;
import java.util.TreeSet;


class Product{
	public final String productID;
	public final String title;
	public final double price;
	public final Set<Review> reviews = new TreeSet<Review>();

	public Product(String productID, String title, double price){
		this.productID = productID;
		this.price = price;
		this.title = title;
	}
}