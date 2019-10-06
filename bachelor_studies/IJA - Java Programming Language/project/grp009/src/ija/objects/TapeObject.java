package ija.objects;

import ija.objects.Gate;
import ija.objects.Wall;

import java.io.Serializable;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/** 
 * Class which represents objects on Tape. This is abstract class, which means that every object will be implemented as separate class
 * Every object on tape extends from this class 
 * */
public abstract class TapeObject implements Serializable {
	public String name;
	
	/** Abstract boolean method, for determining if the Object can be opened */
	public abstract boolean canBeOpen();
	
	/** Static method, acording the name (in parameter) we will create a Wall/Gate/Key/Finish object */
	public static TapeObject create(String format) {
		if(format.equals("w")) {
			Wall object = new Wall("w");
			return object;
		}
		else if (format.equals("g")) {
			Gate object = new Gate("g");
			return object;
		}
		else if(format.equals("k")) {
			Key object = new Key("k");
			return object;
		}
		else if(format.equals("f")) {
			Finish object = new Finish("f");
			return object;
		}
		else {
			return null;
		}
	}

	public TapeObject(String name) {
		this.name = name ;
	}
	
	/** Overriden method, returns id(integer value) of current Object  */
	public int hashCode() {
		return this.name.hashCode();
	}
	
	/** Abstract boolean method, for determining if the object can be seized */
	public abstract boolean canSeize();
	
	/** Abstract boolean method, for determining if the object is open or not */
	public abstract boolean open();

}