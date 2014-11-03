import java.util.Set;
import java.util.TreeSet;


class Reviewer {
	public final String profileName;
	public final String userID;
	public final Set<Review> reviews = new TreeSet<Review>();

	public Reviewer(String profilename, String userid){
		profileName = profilename;
		userID = userid;
	}

}