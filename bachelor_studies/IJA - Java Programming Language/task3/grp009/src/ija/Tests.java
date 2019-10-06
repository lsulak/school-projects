package ija;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak a Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;
import ija.tape.*;

/** Test class, for movement on map, and testing commands on objects. There are 4 tests, for testing game logic */
public class Tests {

	/** Call this method before every test*/
    @Before
    public void setUp() {
    }

    /** Call this method after every test*/
    @After
    public void tearDown() {
    }
    
    /** Test method, for testing object Wall */
	@Test
	public void test_Wall() {
		System.out.println("Testing operations with field 'Wall'");
				
		int width = 4;
		int height = 2;
		TapeHead h1 = null;
		Tape t1 = new Tape(height, width, 2, "w e e e k f k g");
		h1 = t1.createHead(1);	
		h1.rotate(false); //rotate left
		
		print(t1, h1, 2, 4);
		assertFalse("Operation 'step' in front of field Wall is not supported", h1.move());
		assertFalse("Operation 'take' in front of field Wall is not supported", h1.tryTakeKey());
		assertFalse("Operation 'open' in front of field Wall is not supported", h1.tryOpenGate());
		System.out.println();
	}

	/** Test method, for testing objects Gate and Key - operations step, move, take.. */
	@Test
	public void test_Gate_Key() {
		System.out.println("Testing operations with field 'Gate'");
				
		int width = 4;
		int height = 2;
		TapeHead h1 = null;
		Tape t1 = new Tape(height, width, 2, "w g e e k f k g");
		h1 = t1.createHead(1);	
	
		h1.rotate(true); //rotate right, step 2 times, and rotate right
		h1.move();		
		h1.move();
		h1.rotate(true);
		
		print(t1, h1, 2, 4);
		assertFalse("Operation 'step' in front of field Gate is not supported if the Gate is still locked", h1.move());
		assertFalse("Operation 'take' in front of field Gate is not supported", h1.tryTakeKey());
		assertFalse("Operation 'open' in front of field Gate is not supported if Player have 0 keys", h1.tryOpenGate());
		
		h1.addKeys(1);   //Lets add 1 key to player
		assertTrue("Operation 'open' in front of field Gate is successful only if player have key(s)", h1.tryOpenGate());
		assertTrue("Operation 'step' in front of field Gate is successful only if the Gate is open", h1.move());
		assertEquals("After opening Gate with 1 key Player should have 0 keys",  0, h1.getKeyCount());
	
		h1.rotate(true); //Rotate right and take key
		assertTrue("Operation 'take' in front of field Key is supported", h1.tryTakeKey());
		assertEquals("After taking Key in front of player, key should be added to player's inventory", 1, h1.getKeyCount());
		assertTrue("If player just takes a Key, this field should be 'empty', so player can seized it", h1.move());
	
		h1.rotate(true); //rotate right, move, rotate left
		h1.move();
		h1.rotate(false);
	
		assertFalse("Operation 'step' through locked Gate is not supported even if player has a key", h1.move());
		assertTrue("Operation 'open' in front of locked Gate is supported, if player has at least 1 key", h1.tryOpenGate());
		assertTrue("Operation 'step' in front of opened Gate is supported", h1.move());
		System.out.println();
	}
	
	/** Test method, for testing object Final */
	@Test
	public void test_Final() {
		System.out.println("Testing operations with field 'Final'");
				
		int width = 4;
		int height = 2;
		TapeHead h1 = null;
		Tape t1 = new Tape(height, width, 2, "w e e e k f k g");
		h1 = t1.createHead(1);	
	
		h1.rotate(false); //rotate left 2 times
		h1.rotate(false);
	
		print(t1, h1, 2, 4);
		assertFalse("Operation 'take' in front of field Final is not supported", h1.tryTakeKey());
		assertFalse("Operation 'open' in front of field Final is not supported", h1.tryOpenGate());
		assertTrue("Operation 'step' through field Final is supported (end of the game actually)" , h1.move());
		System.out.println();
	}
	
	/** Test method, for testing object Empty, and testing a situation, when a Head(player) is somewhere on border, and we are trying to make operation step, or take or open */
	@Test
	public void test_Empty_Borders() {
		System.out.println("Testing operations with field 'Empty'");
		System.out.println("Testing operations if we stands on borders and our direction is out of labyrinth ");		
		int width = 4;
		int height = 2;
		TapeHead h1 = null;
		Tape t1 = new Tape(height, width, 2, "w e e e k f k g");
		h1 = t1.createHead(1);	

		print(t1, h1, 2, 4);
		assertFalse("Operation 'take' in front of empty field is not supported", h1.tryTakeKey());
		assertFalse("Operation 'open' in front of empty field  is not supported", h1.tryOpenGate());
		assertFalse("Operation 'step' in front of empty field is not supported" , h1.move());
		System.out.println();
	}

	/** Method for printing the whole labyrinth with player and his direction */
	public void print(Tape t, TapeHead h, int height, int width)
	{
		for (int i=0; i < height; i++) {
			for (int j=0; j < width; j++) {
				String tmp = "e";
				if(t.field[i][j].head != null) {
					tmp = "";
					switch (t.field[i][j].head.direction) {
					case 1:
						tmp+="^";
						break;
					case 2:
						tmp+=">";
						break;
					case 3:
						tmp+="V";
						break;
					case 4:
						tmp+="<";
						break;
					default:
						break;
					}
				}
				else if(t.field[i][j].obj != null) {
					tmp = t.field[i][j].obj.name;
				}
				System.out.print(" " + tmp);
			}
				System.out.println("");
			}
		}
}
	
