package ija.tape;

import ija.tape.TapeField;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/** 
 * This class represents object Head (player)
 * This object has some operations:
 * 		moving - we actually always move forward
 * 		rotating - left, or right
 * 		taking a key - a key must be in front of Head
 * */
public class TapeHead {
	private int id;
	private TapeField field;
	private int keyCount;
	/** Variable direction is for determining a direction - 1 for Up, 2 for Right, 3 for Down, 4 for left */
	public int direction; 
	
	/** Overriden method, returns id of current Head  */
	public int hashCode() {
		return id;
	}
	
	/** Overriden method, compares 2 instances of TapeHead(this class) */
	public boolean equals(Object o) {
		if(o instanceof TapeHead) {
			TapeHead c = (TapeHead)o;
			if(this.id == c.id) {
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
	
	/** Contructor, ID for player, f for information about seized field(type) */
	public TapeHead(int id, TapeField f) {
		this.id = id;
		this.field = f;	
		this.keyCount = 0;
		this.direction = 1;
	}
	/** This method simply returns actual field, which is seized by Head(player) */
	public TapeField seizedField() {
		return field;
	}
	/** Player can take key in front of him. We need to remember how many keys player have, this method is for this purpose. */ 
	public void addKeys(int n) {
		this.keyCount += n;
	}
	/** This method implements moving up - but firstly checks if that field can be seized */
	public boolean moveUp() {
		TapeField upField = field.upField();
		if(upField != null && upField.canSeize()) { // ked prve neprejde tak nebude sa sahat tam kam nema - skratene vyhodnocovanie
			field.leave();
			upField.seize(this);
			field = upField; /// premiestni policko na ktorom som na aktualne policko
			return true;
		}
		else {
			return false;
		}
	}
	/** This method implements moving down - but firstly checks if that field can be seized */
	public boolean moveDown() {
		TapeField downField = field.downField();
		if(downField != null && downField.canSeize()) { // ked prve neprejde tak nebude sa sahat tam kam nema
			field.leave();
			downField.seize(this);
			field = downField; /// premiestni policko na ktorom som na aktualne policko
			return true;
		}
		else {
			return false;
		}
	}
	/** This method implements moving left - but firstly checks if that field can be seized */
	public boolean moveLeft() {
		TapeField leftField = field.leftField();
		if(leftField != null && leftField.canSeize()) { // ked prve neprejde tak nebude sa sahat tam kam nema
			field.leave();
			leftField.seize(this);
			field = leftField; /// premiestni policko na ktorom som na aktualne policko
			return true;
		}
		else {
			return false;
		}
	}
	/** This method implements moving right - but firstly checks if that field can be seized */
	public boolean moveRight() {
		TapeField rightField = field.rightField();
			if(rightField != null && rightField.canSeize()) { // ked prve neprejde tak nebude sa sahat tam kam nema
				field.leave();
				rightField.seize(this);
				field = rightField; /// premiestni policko na ktorom som na aktualne policko
				return true;
			}
			else {
				return false;
			}
	}
	
	/** This method calls another method (according to variable 'direction') for moving forward */
    public boolean move() {
		switch (this.direction) {
		case 1:
			return this.moveUp();
		case 2:
			return this.moveRight();
		case 3:
			return this.moveDown();
		case 4:
			return this.moveLeft();
		default:
			return false;
		}
	}
    /** This method is for rotating a Head(player), if parameter 'boolean right' is true, then turn right. if is false, then turn left */
    public void rotate(boolean right) {
    	if(right) {
    		if(this.direction < 4) {
    			this.direction++;
    		}
    		else {
    			this.direction=1;
    		}
    	}
    	else {
    		if(this.direction > 1) {
    			this.direction--;
    		}
    		else {
    			this.direction=4; //Number 4, because there exist 4 direction (Up, Right, Down, left), and we can rotate how many times we want
    		}
      	}
    }
	/** This method returns ID of Head - for recognition of player */
	public int id() {
		return this.id;
	}
	/** Method, which will find out actual type of field */ 
	public TapeField getFieldFromActualDirection() {
		switch (this.direction) {
		case 1:
			return field.upField();
		case 2:
			return field.rightField();
		case 3:
			return field.downField();
		case 4:
			return field.leftField();
		}
		return null;
	}
	
	/** Method, we are trying to take a key, if successful, then a message about number of keys is printed */
	public boolean tryTakeKey() {
		TapeField tmp=getFieldFromActualDirection();
		if(tmp != null && tmp.obj != null && tmp.obj.name.equals("k")) {
			this.keyCount++;
			System.out.println("Pocet klucov : "+this.keyCount);
			tmp.obj = null;
			return true;
		}
		else {
			return false;
		}
	}
	/** Method, just returns number of keys */
	public int getKeyCount() {
		return this.keyCount;
	}

	/** Purpose of this method is to open Gate.
	 * Gate is opened only if a player has at least 1 key
	 */
	public boolean tryOpenGate() {
		TapeField tmp=getFieldFromActualDirection();
		
		if(this.keyCount > 0) {
			if(tmp != null && tmp.obj != null && tmp.obj.name.equals("g") && tmp.obj.canBeOpen() ) {
				this.keyCount--;
				tmp.obj.open();
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

	/** Boolean method for final field, True if head(player) seize field 'f' */
	public boolean checkIfNotFinished() {
		if(this.field.obj != null && this.field.obj.name.equals("f"))		
			return true;
		
		return false;
	}
	
}