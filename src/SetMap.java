import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
	
class SetMap<T, V> extends HashMap<T, Set<V> > {
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
