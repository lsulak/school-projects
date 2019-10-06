package ija.objects;

import ija.objects.TapeObject;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/** 
 * Class which represents object on Tape - Wall 
 * Body of methods, which are here contained, are modificated.
 * This field cannot be open, and we can't seize it
 * */
public class Wall extends TapeObject{
	
	public Wall(String name) {
		super(name);
	}
	/** This field cannot be open */
	public boolean canBeOpen() {
		return false;
	}
	
	/** Overriden method, compares 2 instances of Wall(this class) */
	@Override
	public boolean equals(Object o) {
		if(o instanceof Wall) {
			Wall c = (Wall)o;
			if(c.name.equals(this.name)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}
	/** This field cannot be seized */
	@Override
	public boolean canSeize() {
		return false;
	}
	/** This field cannot be open */
	@Override
	public boolean open() {
		return false;
	}
}