package ija.objects;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/** 
 * Class which represents object on Tape - field for finishing the game 
 * Body of methods, which are here contained, are modificated.
 * This field cannot be open, but we can seize it (player wins after that)
 * */
public class Finish extends TapeObject {

	/** Constructor, fills variable name with variable with same name from superior class */
	public Finish(String name) {
		super(name);
	}
	/** This field cannot be open */
	@Override
	public boolean canBeOpen() {
		return false;
	}
	/** We can seize this field (player wins after that) */
	@Override
	public boolean canSeize() {
		return true;
	}
	/** This field cannot be open */
	@Override
	public boolean open() {
		return false;
	}


}
