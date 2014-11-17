import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;
import java.util.*;
import java.io.*;
import java.lang.reflect.*;


class Reviewer implements Mementoable<Reviewer>, Comparable<Reviewer>{
	public static final long serialVersionUID = 18968l;
	public final String profileName;
	public final String userID;
	public final int id;
	public final Set<Review> reviews = new TreeSet<Review>();

	private static int idr = 0;
	private static HashMap<String, Reviewer> lookup = new HashMap<String, Reviewer>();

	public RMemento pack(){
		return new RMemento(this);
	}
	
	public static class RMemento implements Memento<Reviewer> {
		private final String profileName;
		private final String userID;
		public RMemento(Reviewer r){
			profileName = r.profileName;
			userID = r.userID;

		}
		public Reviewer unpack(){
			return Reviewer.build(profileName,userID);
		}
	};


	private Reviewer(String profilename, String userid){
		profileName = profilename;
		userID = userid;
		id = idr++;
	}
	

	
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
