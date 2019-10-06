package ija;
import ija.tape.Tape;
import ija.tape.TapeHead;

import java.io.File;
import java.util.Scanner;
import java.io.FileNotFoundException;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/** Game represents class in which is main method, so it is a 'core' of the game itself. */
public class Game {    
	private String fileName;
	private String map;
	private int height;
	private int width;
	private Tape table;

	
	/** Main Method, Firstly a /examples folder, with all txt files if printed,for choosing a map
	 *  then open a map according to command 'game' with parameter - name of file
	 */
    public static void main(String[] args) { 
    	printAvailableMaps();
    	
		Game game = new Game();
		Scanner input = game.openFile();
    	game.readMapFile(input);
    	game.prepareTable();
    	TapeHead player = game.preparePlayer();
    	//game.print();
    	while(true) {
    		game.GameRunning(player);
    	}  
    }
    /** This method is called only once - at the very begin of a game. 
     * 	Content of directory /examples is printed - there are availible maps
     */
	public static void printAvailableMaps() {
    	File folder = new File("./examples/");
    	File[] listOfFiles = folder.listFiles();
    	System.out.println("Available maps (choose one and start game with command 'game map_name' ) : ");

    	    for (int i = 0; i < listOfFiles.length; i++) {
    	      if (listOfFiles[i].isFile()) {
    	        System.out.println("\t"+listOfFiles[i].getName());
    	      } 
    	    }
    }
	
    /** Constructor, which fills Name of file, and every instance variable we need: height, width of map, and object Tape*/
	public Game() {
    	this.height = 0;
    	this.width = 0;
    	this.map="";
    	this.table=null;
    	this.fileName="";
    }
	/** Method, for opening a file with map */
   @SuppressWarnings("resource")
    public Scanner openFile() {
        boolean flag = false;
        Scanner input;

        while(!flag) {
			Scanner scanned = new Scanner(System.in);  
	        if((scanned.next()).equals("game")) {
	        	this.fileName = scanned.next();
	            try
	            {
	              File file = new File("./examples/"+this.fileName);
	              input = new Scanner(file);
	              flag = true;
	              return input;
	            }
	            catch (FileNotFoundException e)
	            {
	            	System.err.println("File was not found, try again");
	            }        	
	        }
	        else {
	        	System.err.println("First command must be game, try again");
	        }
        }
        return null;
    }
	   
    /** Method, which creates object Tape - with dimensions of labyrinth, number of players, and actual map of labyrinth */
    public void prepareTable() {
    	this.table = new Tape(this.height,this.width,2,this.map);
    }
    
    /** Method, which creates object Head - a player */
    public TapeHead preparePlayer() {
    	return this.table.createHead(1);	
    }
    
    /** Once, a file is opened, we will save his content into instance variables */
    public void readMapFile(Scanner input) {
        this.height = input.nextInt();
        this.width = input.nextInt();
        if((this.height >= 20 && this.height <= 50) && (this.width >= 20 && this.width <= 50)) 
        {	 
	        while(input.hasNext())
	        {
	            this.map += input.next();
	        }
	        input.close();
        }
        else
        {
        	System.err.println("Map you have chosen is corrupted, please report this map to your neareast administrator");
        	System.exit(1);
        }
        	
    }
    
    /** Method, is called in main method, in infinite cycle
     *  Receives commands and dealing with them - so the game itself runs here */
    public void GameRunning(TapeHead player) {
        String command = "";
        
	    Scanner scanned = new Scanner(System.in);  
	    command = scanned.next();
	    switch(command)
	    {
          case "game":
              System.out.println("Firstly, close your actual game and then you can start new game ");
              break;
          case "left":
        	  player.rotate(false);
        	  System.out.println("Rotation left was succesful");
              break;
          case "right":
        	  player.rotate(true);
        	  System.out.println("Rotation right was succesful");
              break;
          case "step":
        	  if(player.move()) {
        		  System.out.println("Move succesful");
        	  	  if(player.checkIfNotFinished()) {
        	  		  System.out.println("Game Finished, you WON");
        	  		  scanned.close();
        	  		  System.exit(0);
        	  	  }
        	  }
        	  else 
        		  System.out.println("There is wall or closed gate or key or you are trying to move out of field");
              break;
          case "take":
        	  if(player.tryTakeKey()) 
        		  System.out.println("Key has been taken");
        	  else 
        		  System.out.println("Key could not been taken");
              break;
          case "open":
        	  if(player.tryOpenGate())
        		  System.out.println("Gate sucessfully opened");
        	  else 
        		  System.out.println("Key missing or gate is already opened");
              break;
          case "keys":
        	  System.out.println("Pocet klucov : "+player.getKeyCount());  
              break;
          case "show":
              this.print();
              break;
          case "close":
              scanned.close();
              System.exit(0);
	      default:
              System.err.println("Neexistujuci prikaz");
              break;
	    }
    }
    
    /** Print a labyrinth with the player - player is represented as arrow*/
    public void print() {
    	for (int i=0; i < this.table.height; i++) {
    		for (int j=0; j < this.table.width; j++) {
    			String tmp = "e";
    			if(this.table.field[i][j].head != null) {
    				tmp = "";
    				switch (this.table.field[i][j].head.direction) {
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
    			else if(this.table.field[i][j].obj != null) {
    				tmp = this.table.field[i][j].obj.name;
    			}
    			System.out.print(" " + tmp);
    		}
    			System.out.println("");
    	}
    } 
    
}
