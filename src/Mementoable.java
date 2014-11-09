import java.io.Serializable;

interface Mementoable<T> {
	public interface Memento<T> extends Serializable {
		public T unpack();
	}
	public Memento<T> pack();

}
