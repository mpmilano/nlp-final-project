import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;


class Reviewer {
	public final String profileName;
	public final String userID;
	public final Set<Review> reviews = new TreeSet<Review>();

	private Reviewer(String profilename, String userid){
		profileName = profilename;
		userID = userid;
	}
	
	private static HashMap<String, Reviewer> lookup = new HashMap<String, Reviewer>();
	
	public static Reviewer build(String profilename, String userid){
		if (lookup.containsKey(userid)) return lookup.get(userid);
		Reviewer p = new Reviewer(profilename, userid);
		lookup.put(userid, p);
		return p;		
	}

}