import java.util.HashMap;
import java.util.Set;
import java.util.*;
import java.io.*;
import java.lang.reflect.*;


class Product implements Mementoable<Product>, Comparable<Product>{
	public final String productID;
	public final int id;
	public final String title;
	public final double price;
	public final Set<Review> reviews = new TreeSet<Review>();

	private static int idr = 0;
	private static HashMap<String, Product> lookup = new HashMap<String, Product>();


	public PMemento pack() {
		return new MPemento(this);
	}

	public static class PMemento implements Memento<Product> {
		public static final long serialVersionUID = 7022l;
		
		private final String productID;
		private final String title;
		private final double price;

		public PMemento(Product p){
			productID = p.productID;
			title = p.title; 
			price = p.price;
		}

		public Product unpack(){
			return Product.build(productID, title, price);
		}
	}


	private Product(String productID, String title, double price){
		this.productID = productID;
		this.price = price;
		this.title = title;
		id = idr++;
	}

	public static Product build(String productID, String title, double price){
		if (lookup.containsKey(productID)) return lookup.get(productID);
		Product p = new Product(productID, title, price);
		lookup.put(productID, p);
		return p;
	}

	public static Product build(String productID){
		return lookup.get(productID);
	}


	@Override
	public int compareTo(Product o) {
		// TODO Auto-generated method stub
		return (this == o) ? 0 : (id < o.id ? -1 : id == o.id ? 0 : 1);
	}
}
