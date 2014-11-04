import java.util.HashMap;

public class Helpfulness {
	public final int votes;
	public final int total;
	private Helpfulness(int votes, int total){
		this.votes = votes; this.total = total;
	}
	
	private static HashMap< Long,  Helpfulness> lookup = new HashMap<Long, Helpfulness>();
	public static Helpfulness build(int votes, int total){
		long p = (((long) votes) << 32) + total;
		if (lookup.containsKey(p)) return lookup.get(p);
		Helpfulness ret = new Helpfulness(votes,total); 
		lookup.put(p,ret);
		return ret;
	}
}
