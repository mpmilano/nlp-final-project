import java.io.*;
import java.util.*;

class Review implements Mementoable<Review>, Comparable<Review> {
	public final String summary;
	public final double score;
	public final int time;
	public final Reviewer reviewer;
	public final Helpfulness help;
	public final Product product;
	public final String text;

	public static class RMemento implements Memento<Review> {
		public final String summary;
		public final double score;
		public final int time;
		public final Memento<Reviewer> reviewer;
		public final Helpfulness help;
		public final Memento<Product> product;
		public final String text;
		public RMemento(Review r){
			summary = r.summary;
			score = r.score;
			time = r.time;
			reviewer = r.reviewer.pack();
			help = r.help;
			product = r.product.pack();
			text = r.text;
		}

		public Review unpack(){
			return Review.build(summary,score,time,reviewer.unpack(),help,product.unpack(),text);
		}
	}

	public Memento<Review> pack(){
		return new RMemento(this);
	}
	
	private Review(String summary, double score, int time, Reviewer reviewer, Helpfulness help, Product product, String text){
		this.summary = summary;
		this.score = score;
		this.time = time;
		this.reviewer = reviewer;
		this.help = help;
		this.product = product;
		this.text = text;
	}
	
	public static Review build(String summary, 
				   double score, 
				   int time, 
				   Reviewer reviewer, 
				   Helpfulness help, 
				   Product product, 
				   String text){
		Review ret = new Review(summary, score, time, reviewer, help, product, text);
		reviewer.reviews.add(ret);
		product.reviews.add(ret);
		return ret;
	}

	@Override
		public int compareTo(Review o) {
		// TODO Auto-generated method stub
		return (this == o) ? 0 : score == o.score ? 
			(time == o.time ? 
			 (help.total == o.help.total ? 
			  (help.votes == o.help.votes ? text.compareTo(o.text) : (help.votes < o.help.votes ? -1 : 1))
			  : (help.total < o.help.total ? -1 : 1))
			 : (time < o.time ? -1 : 1))
			: (score < o.score ? -1 : 1);
		
	}
	
}
