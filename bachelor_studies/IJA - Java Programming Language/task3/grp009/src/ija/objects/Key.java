package ija.objects;

import ija.objects.TapeObject;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/** 
 * Class which represents object on Tape - Key for opening the Gate 
 * Body of methods, which are here contained, are modificated.
 * This field cannot be open, and we can't seize it (actually we can seize this field, bude only after command 'Take' - and key is added to our inventory)
 * */
public class Key extends TapeObject {
	public Key(String name) {
		super(name);
	}
	/** This field cannot be open */
	public boolean canBeOpen() {
		return false;
	}
	/** This field cannot be seized */
	public boolean canSeize() {
		return false;
	}
	/** This field cannot be open */
	public boolean open() {
		return false;
	}
}
	