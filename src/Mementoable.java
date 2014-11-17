interface Mementoable<T> {

	public Memento<T> pack();

}

import java.io.Serializable;

interface Memento<T> extends Serializable {
	public T unpack();
}
