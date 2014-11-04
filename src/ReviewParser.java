import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
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
				overflow = done + split[1];
				return split[0].split(prefix)[1];
			}
		}
		
		throw new OutOfTextException(acc.split(prefix)[1]);
	}

	/**
	   Give me a filename and three empty sets, and I'll populate the sets
	   with all of the stuff in the file. 
	 * @throws FileNotFoundException 
	 */
	
	public static void parse(String filename, Set<Reviewer> c, Set<Product> p, Set<Review> r ) throws IOException{
		BufferedReader f = new BufferedReader(new FileReader(new File(filename)));
		ReviewParser rp = new ReviewParser(f);
		while (f.ready()){
			String productId = rp.reallyRead("product/productId: ","product/title: ");
			String productTitle = rp.reallyRead("product/title: ","product/price: ");
			String productPrice = rp.reallyRead("product/price: ", "review/userId: ");
			String reviewUserId = rp.reallyRead("review/userId: ", "review/profileName: ");
			String reviewProfileName = rp.reallyRead("review/profileName: ", "review/helpfulness: ");
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
			Product p = new Product(productId, productTitle);
			
			System.out.println(productId);
			System.out.println(productTitle);
		}
	}
}
