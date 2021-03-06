import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.*;
import java.util.Set;

class ReviewParser {

	class OutOfTextException extends IOException {
		/**
		 * 
		 */
		private static final long serialVersionUID = -5624253573956415820L;
		public String p;
		public OutOfTextException(String s){
			super(s);
			p = s;
		}
	}

	private BufferedReader r;
	private ReviewParser(BufferedReader f){
		this.r = f;
	}

	
	private String overflow = "";
	private String rangeFromOverflow(String prefix, String done){
		if (overflow.contains(prefix)){
			if (overflow.endsWith(prefix)) return prefix;
			String[] tmp = overflow.split(prefix)[1].split(done);
			if (tmp.length > 1) overflow = done + tmp[1];
			return prefix + tmp[0];
		}
		return "";
	}

	private String reallyRead(String prefix, String done) throws IOException{
		String acc = rangeFromOverflow(prefix, done);
		if (overflow.contains(done)) return acc.split(prefix)[1];
		while (r.ready()){
			acc += r.readLine();
			if (acc.contains(done)){
				String[] split = acc.split(done);
				overflow = done + (split.length > 1 ? split[1] : "");
				try { return split[0].split(prefix)[1]; }
				catch (ArrayIndexOutOfBoundsException e) { return "";}
				}
			}
		
		throw new OutOfTextException(acc.split(prefix)[1]);
	}

	/**
	   Give me a filename and three empty sets, and I'll populate the sets
	   with all of the stuff in the file. 
	 * @throws FileNotFoundException 
	 */

	private static void addAll(Set<T> s, Collection<Memento<T> > c){
		for (T t : c){
			s.add(t.unpack());
		}
	}

	private static void readFromFile(String filename, Set<Reviewer> cs, Set<Product> ps, Set<Review> rs) throws IOException{
		String cachefileprefix = "/tmp/" + filename.replace('/','%');
		String rprr = cachefileprefix + "rp-rr.obj";
		String rpp = cachefileprefix + "rp-p.obj";
		String rpr = cachefileprefix + "rp-r.obj";

		addAll(cs, ((Set<Memento<Reviewer> >) new ObjectInputStream(new FileInputStream(new File(rprr))).readObject());
		ps.addAll((Set<Product>) new ObjectInputStream(new FileInputStream(new File(rpp))).readObject());
		rs.addAll((Set<Review>) new ObjectInputStream(new FileInputStream(new File(rpr))).readObject());
		return;
	}

	private static void writeToFile() {
		
	}
	
	public static void parse(String filename, Set<Reviewer> cs, Set<Product> ps, Set<Review> rs ) throws IOException{
		BufferedReader f = new BufferedReader(new FileReader(new File(filename)));
		ReviewParser rp = new ReviewParser(f);
		int count = 0;

		String cachefileprefix = "/tmp/" + filename.replace('/','%');
		String rprr = cachefileprefix + "rp-rr.obj";
		String rpp = cachefileprefix + "rp-p.obj";
		String rpr = cachefileprefix + "rp-r.obj";
	
		try {
			cs.addAll((Set<Reviewer>) new ObjectInputStream(new FileInputStream(new File(rprr))).readObject());
			ps.addAll((Set<Product>) new ObjectInputStream(new FileInputStream(new File(rpp))).readObject());
			rs.addAll((Set<Review>) new ObjectInputStream(new FileInputStream(new File(rpr))).readObject());
			return;
		}
		catch (Exception ignoreme){
			try {
				while (f.ready()){
					++count;
					String productId = rp.reallyRead("product/productId: ","product/title: ");
					productId.intern();
					String productTitle = rp.reallyRead("product/title: ","product/price: ");
					productTitle.intern();
					String productPrice = rp.reallyRead("product/price: ", "review/userId: ");
					productPrice.intern();
					String reviewUserId = rp.reallyRead("review/userId: ", "review/profileName: ");
					reviewUserId.intern();
					String reviewProfileName = rp.reallyRead("review/profileName: ", "review/helpfulness: ");
					reviewProfileName.intern();
					String reviewHelpfulness = rp.reallyRead("review/helpfulness: ", "review/score: ");
					String reviewScore = rp.reallyRead("review/score: ", "review/time: ");
					String reviewTime = rp.reallyRead("review/time: ", "review/summary: ");
					String reviewSummary = rp.reallyRead("review/summary: ", "review/text: ");
					String reviewText = "";
					try {
						reviewText = rp.reallyRead("review/text: ", "product/productId: ");
					}
					catch (OutOfTextException e){
						reviewText = e.p;
					}
					double price = -1; 
					try {
						price = Double.parseDouble(productPrice);
					}
					catch (NumberFormatException e){
						//ignore

					}
					Product p = Product.build(productId, productTitle, price);
					Reviewer c = Reviewer.build(reviewProfileName, reviewUserId);
					String[] hlp = reviewHelpfulness.split("/");
					Helpfulness h = Helpfulness.build(Integer.parseInt(hlp[0]), Integer.parseInt(hlp[1]));
					Review r = Review.build(reviewSummary,Double.parseDouble(reviewScore), Integer.parseInt(reviewTime),c,h,p,reviewText);
					cs.add(c);
					ps.add(p);
					rs.add(r);
				}
				new ObjectOutputStream(new FileOutputStream(new File(rprr))).writeObject(cs);
				new ObjectOutputStream(new FileOutputStream(new File(rpr))).writeObject(rs);
				new ObjectOutputStream(new FileOutputStream(new File(rpp))).writeObject(ps);
			}
			catch (RuntimeException e){
				System.out.println("Got this far: " + count);
				throw e;
			}
		}
	}
}
