import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;


class Reviewer implements Comparable<Reviewer>{
	public final String profileName;
	public final String userID;
	public final int id;
	public final Set<Review> reviews = new TreeSet<Review>();

	
	private static int idr = 0;
	private Reviewer(String profilename, String userid){
		profileName = profilename;
		userID = userid;
		id = idr++;
	}
	
	private static HashMap<String, Reviewer> lookup = new HashMap<String, Reviewer>();
	
	public static Reviewer build(String profilename, String userid){
		if (lookup.containsKey(userid)) return lookup.get(userid);
		Reviewer p = new Reviewer(profilename, userid);
		lookup.put(userid, p);
		return p;		
	}

	@Override
	public int compareTo(Reviewer o) {
		return id < o.id ? -1 : id == o.id ? 0 : 1; 
	}

}