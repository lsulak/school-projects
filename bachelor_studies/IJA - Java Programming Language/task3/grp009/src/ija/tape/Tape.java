package ija.tape;

import ija.tape.TapeField;
import ija.tape.TapeHead;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/**
 * Purpose of this class is to fill 2D array with objects (from *.txt file), creating Head (player) - to put her on empty field
 */
public class Tape {
	public TapeField[][] field;	
	public TapeHead[] head;
	public int iterator=0;
	public int height;
	public int width;

	/** A main constructor, fills 2D array with objects, creates object Head*/
	public Tape(int height,int width,int h, String format) { // upraveny konstruktor
		this.height = height;
		this.width = width;
		field = new TapeField[height][width];  // upravene na dvojrozmerne pole
		head = new TapeHead[h];
		
		String tmp ="";
		String objectString="";
		tmp = format.replaceAll("\\s","");
		
		for(int i=0;i<height;i++) {
			for(int j=0;j<width;j++) {
				char tmpCh = tmp.charAt(i*width+j);
				objectString = String.valueOf(tmpCh);
				field[i][j] = new TapeField(this,i,j,objectString);   // upravit TapeField
			}
		}
	}
	/** Method, which returns actual field. It is used for determining a type of field, for example in movement actions */
	public TapeField fieldAt(int i,int j) {
		if(i >= height || j>= width || i<0 || j<0 ) {
			return null;
		}
		else {
			return field[i][j]; 
		}
	}
	/** This method is for creating Head (player) - to put her on empty field */
	public TapeHead createHead(int id) {   // upravene to 2d
		for(int i=0;i<height;i++)
			for(int j=0;j<width;j++) {
				if(field[i][j].canSeize() && field[i][j].obj == null) {   // uprava ze to musi byt E
					TapeHead obj = new TapeHead(id,field[i][j]); 
					head[iterator] = obj;
					iterator++;
					field[i][j].seize(obj);
					return obj;
				}
			}
			return null;
	}

}
