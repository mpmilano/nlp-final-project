import java.util.*;
import java.io.*;

class Related {

	public final static String uranium_ore = "B000796XXM";
	public final static String unicorn_meat = "B004CRYE2C";
	public final static String horse_head = "B003G4IM4S";
	public final static String images = "0399536493";
	public final static String huge = "1594743061";
	public final static String steeringWheelDesk = "B000IZGIA8";
	public final static String threewolf = "B002HJ377A";
	public final static String hasselhoff = "B0000070S1";
	public final static String ethernet = "B000I1X6PM";
	public final static String rabbit = "B00012182G";
	public final static String deer = "B004DNIXEK";
	public final static String repellant = "B004Y6JFU2";
	public final static String lube = "B005MR3IVO";
	public final static String grappling = "B000HKSSLQ";
	public final static String pants = "B004VMQPFM";
	public final static String quantum = "B008BV9LK0";
	public final static String bacon = "B000SSV8AA";
	public final static String catface = "B0077RT3ZM";
	public final static String pickle = "B003BW67TW";
	public final static String unicat = "B00BPMMQDG";
	public final static String airshark = "B005FYEAJ8";
	public final static String squirrelpants = "B004I03BCM";
	public final static String otherpickle = "B0010VS078";
	public final static String bigships = "0870334336";

	public final static List<String> all = new LinkedList<String>() {{
			
			add(uranium_ore);
			add(unicorn_meat);
			add(horse_head);
			add(images);
			add(huge);
			add(steeringWheelDesk);
			add(threewolf);
			add(hasselhoff);
			add(ethernet);
			add(rabbit);
			add(deer);
			add(repellant);
			add(lube);
			add(grappling);
			add(pants);
			add(quantum);
			add(bacon);
			add(catface);
			add(pickle);
			add(unicat);
			add(airshark);
			add(squirrelpants);
			add(otherpickle);
			add(bigships);

		}};
	
	static class SetMap<T, V> extends HashMap<T, Set<V> > {
		public static final long serialVersionUID = 42L;
		void putk(T t, V v){
			if (containsKey(t))
				get(t).add(v);
			else {
				HashSet<V> vs = new HashSet<V>();
				vs.add(v);
				put(t, vs);
			}
		}
	}

	public static SetMap<String,String> buildFile(String plaintext, String classfile) throws IOException {
		SetMap<String,String> related = new SetMap<String,String>();
		File f = new File(plaintext);
		BufferedReader r = new BufferedReader(new FileReader(f));
		while (r.ready()){
			String[] batch = r.readLine().split(" ");
			for (String s1 : batch){
				for (String s2 : batch){
					related.putk(s1, s2);
					related.putk(s2,s1);
				}
			}
		}
		ObjectOutputStream fos = new ObjectOutputStream(new FileOutputStream(new File(classfile)));
		fos.writeObject(related);
		return related;
	}

	public static void main(String[] args) throws Exception{
		SetMap<String, String> sm;
		try {
			sm = (SetMap<String, String>) new ObjectInputStream(new FileInputStream(new File(args[0] + "-javaclass"))).readObject();
		}
		catch (Exception e){
			sm = buildFile(args[0], args[0] + "-javaclass");
		}

		Set<String> alls = new HashSet<String>();
		sm.containsKey("1");
		

		for (String s : all){
			if (sm.containsKey(s))
				for (String s1 : sm.get(s))
					alls.add(s1);
			alls.add(s);
		}
		
		for (String s : alls) System.out.println(s);

	}
}
