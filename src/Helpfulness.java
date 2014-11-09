import java.util.HashMap;
import java.io.Serializable;

public class Helpfulness implements Serializable{

	public static final long serialVersionUID = 25461l;

	public final int votes;
	public final int total;
	private Helpfulness(int votes, int total){
		this.votes = votes; this.total = total;
	}
	
	private transient static HashMap< Long,  Helpfulness> lookup = new HashMap<Long, Helpfulness>();
	public static Helpfulness build(int votes, int total){
		if (lookup == null) lookup = new HashMap<Long, Helpfulness>(); 
		long p = (((long) votes) << 32) + total;
		if (lookup.containsKey(p)) return lookup.get(p);
		Helpfulness ret = new Helpfulness(votes,total); 
		lookup.put(p,ret);
		return ret;
	}
}
