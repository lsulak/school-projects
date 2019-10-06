package ija.objects;

import ija.objects.TapeObject;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/** 
 * Class which represents object on Tape - Gate
 * This field can be open, and we can seize it (after open, with key)
 **/
public class Gate extends TapeObject{	
	protected boolean opened;
		
	public Gate(String name) {
		super(name);
		this.opened = false;
	}
	
	/** Overriden method, compares 2 instances of Gate(this class) */
	@Override
	public boolean equals(Object o) {
		if(o instanceof Gate) {
			Gate c = (Gate)o;
			if(c.name.equals(this.name) && (c.opened == this.opened)) {
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
	/** Overriden method, returns hashCode (integer number) which depends on opened/closed Gate  */
	@Override
	public int hashCode() {
		if(opened) {
			return this.name.hashCode();
		}	
		else {
			return this.name.hashCode()+1;
		}
	}	
	
	/** Find out, if the Gate is opened or not */
	public boolean canBeOpen() {
		return (!opened);
	}

	/** If the Gate is opened, we can go through her, cannot otherwise */
	public boolean canSeize() {
		if(this.opened) {
			return true;
		}
		else {
			return false;
		}
	}
	/** If the Gate is already opened, returns false, If is locked, open it. */
	public boolean open() {
		if(this.opened) {
			return false;
		}
		else {
			this.opened = true;
			return true;
		}
	}
}