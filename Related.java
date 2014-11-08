import java.util.*;
import java.io.*;

class Related {
	
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

	static class WorkList<T> implements Queue<T> {
		private Set<T> seen = new HashSet<T>();
		private Queue<T> todo = new LinkedList<T>();

	}

	public static void main(String[] args) throws Exception{
		SetMap<String, String> sm;
		try {
			sm = (SetMap<String, String>) new ObjectInputStream(new FileInputStream(new File(args[0] + "-javaclass"))).readObject();
		}
		catch (Exception e){
			sm = buildFile(args[0], args[0] + "-javaclass");
		}

		final String uranium_ore = "B000796XXM";
		final HashSet<String> uranium_closure = new HashSet<String>();
		String curr = uranium_ore;
		Queue<String> pending = new WorkList<String>();
		for (String s : sm.get(pending.remove())) {
			uranium_closure.add(s);
			pending.add(s);
		}
	}
}
