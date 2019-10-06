package ija.tape;

import ija.objects.TapeObject;
import ija.tape.TapeHead;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/**
 * This class represends field on tape, there are operations:
 * 		for leaving field, 
 * 			opening (and another, for checking if opening is possible) 
 * 			seizing(and another, for checking if seizing is possible)
 * 			determining what type of field is up, down, right, left from actual position (this method are called from another methods for moving)
 */
public class TapeField {
	private int positionHeight;
	private int positionWidth;
	public TapeObject obj;
	public TapeHead head;
	protected Tape tape;
	
	/** Constructor, fills height, width of Tape , type of Field, for tape*/
	public TapeField(Tape tape, int height,int width, String type) {
		this.positionHeight = height;
		this.positionWidth = width;
		this.obj = TapeObject.create(type);
		this.tape = tape;
		
	}
	/** Method for determining what type of field is right */
	public TapeField rightField() {
		return tape.fieldAt(this.positionHeight,this.positionWidth+1);
	}
	/** Method for determining what type of field is left */
	public TapeField leftField() {
		return tape.fieldAt(this.positionHeight,this.positionWidth-1);
	}
	/** Method for determining what type of field is up */
	public TapeField upField() {
		return tape.fieldAt(this.positionHeight-1,this.positionWidth);
	}
	/** Method for determining what type of field is down */
	public TapeField downField() {
		return tape.fieldAt(this.positionHeight+1,this.positionWidth);
	}
	
	/** Boolean method for determining if object on field can be opened */ 
	public boolean canBeOpen() {
		if (obj == null) return false;
		return this.obj.canBeOpen();
	}
	
	/** Overriden method, returns integer number of current position of Field  */ 
	public int hashCode() {
		return positionHeight*positionWidth+positionWidth;
	}

	/** Method for determining position of Field (in which height if current Field) */
	public int positionHeight() {
		return this.positionHeight;
	}
	
	/** Method for determining position of Field (in which Width if current Field) */
	public int positionWidth() {
		return this.positionWidth;
	}
	/** This method returns null, or Head. Determined for leaving field (if we are moving) */ 
	public TapeHead leave() {
		TapeHead tmp;
		
		if(this.head == null) {
			return null;
		}
		else {
			tmp = this.head;
			this.head = null;
			return tmp;
		}
	}
	/** Boolean method, for seizing a field */
	public boolean seize(TapeHead hlava) {
		if(this.canSeize()) {
			this.head = hlava;
			return true;
		}
		else {
			return false;
		}
	}
	/** Boolean method, for checks whether field can be seized or not */
	public boolean canSeize() {
		if(this.head == null) {
			if(this.obj == null) {
				return true;
			}
			else {
				if(obj.canSeize()) {
					return true;
				}
				else {
					return false;
				}
			}
		}
		else {
			return false;
		}
	}
	/** Boolean Method, for opening (gate) */
	public boolean open() {
		if(this.obj == null) {
			return false;
		}
		else {
			return obj.open();
		}
	}
}