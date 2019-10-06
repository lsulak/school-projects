package ija;
import ija.tape.Tape;
import ija.tape.TapeHead;

import java.io.BufferedReader;
import java.io.File;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Scanner;
import java.io.FileNotFoundException;
import java.net.*;
import java.io.*;
import java.nio.*;

/** 
 * 3.Ukol - Implementacia aplikacie Bludisko do predmetu Java
 * @author Ladislav Sulak & Marek Bertovic
 * Loginy: xsulak04 , xberto00
 * Datum: 9.4.2014
**/

/** Game represents class in which is main method, so it is a 'core' of the game itself. */
public class Game implements Serializable {
	public int gameID; 
	private String fileName;
	private String map;
	public int height;
	public int width;
	public Tape table;
	private int players_ID=1;
	public boolean gameFinished = false;
	public int winnerID = 0; 

	
	/** Main Method, Firstly a /examples folder, with all txt files if printed,for choosing a map
	 *  then open a map according to command 'game' with parameter - name of file
	 */
    public static void main(String[] args) { 
    	//printAvailableMaps();
	//	Game game = new Game();
		//Scanner input = game.openFile();
/*    	game.readMapFile(input);
    	game.prepareTable();
    	TapeHead player = game.preparePlayer();
    	//game.print();
    	while(true) {
    		game.GameRunning(player);
    	}  */
    }
    /** This method is called only once - at the very begin of a game. 
     * 	Content of directory /examples is printed - there are availible maps
     */
	public static void printAvailableMaps(ObjectOutputStream writeBuffer,ArrayList<String> maps) throws IOException{
      //  try {
    	File folder = new File("./examples/");
    	File[] listOfFiles = folder.listFiles();
        Path currentRelativePath = Paths.get("");
        String s = currentRelativePath.toAbsolutePath().toString();
        System.out.println("Current relative path is: " + s); 

        for (int i = 0; i < listOfFiles.length; i++) {
    	      if (listOfFiles[i].isFile()) {
    	      //  writeBuffer.writeObject("\t" + listOfFiles[i].getName());
    	        maps.add(listOfFiles[i].getName());
                System.out.println("\t"+listOfFiles[i].getName());
    	      } 
        }
        writeBuffer.writeObject(maps);
        writeBuffer.flush();
        writeBuffer.writeObject("OK");
        writeBuffer.flush();
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
    public Scanner openFile(ObjectInputStream readBuffer,ObjectOutputStream writeBuffer) throws IOException,ClassNotFoundException {
        boolean flag = false;
        Scanner input;
        while(!flag) {
	        if((readBuffer.readObject()).equals("game")) {
	        	this.fileName = (String)readBuffer.readObject();
	            try
	            {
	              File file = new File("./examples/"+this.fileName);
	              input = new Scanner(file);
	              flag = true;
                  writeBuffer.writeObject("OK");
                  System.out.println("Map will be loaded");
	              return input;
	            }
	            catch (FileNotFoundException e)
	            {
	            	System.err.println("File was not found, try again");
                    writeBuffer.writeObject("EXC");
	            }        	
	        }
	        else {
                System.out.println("Dubkacik je programator");
	        	System.err.println("First command must be game, try again");
	        }
        }
        return null;
    }
	   
    /** Method, which creates object Tape - with dimensions of labyrinth, number of players, and actual map of labyrinth */
    public void prepareTable() {
    	this.table = new Tape(this.height,this.width,4,this.map);
    }
    
    /** Method, which creates object Head - a player */
    public TapeHead preparePlayer() {
    	TapeHead tmp = this.table.createHead(this.players_ID);
    	this.players_ID += 1;
    	return tmp;
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
            System.out.println("Map is loaded");
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
    public boolean GameRunning(TapeHead player,String command) {
        //String command = "";
        
	   /* Scanner scanned = new Scanner(System.in  
	    command = scanned.next(); */
	    switch(command)
	    {
          case "game":
              System.out.println("Firstly, close your actual game and then you can start new game ");
              return false;
          case "left":
        	  player.rotate(false);
        	  System.out.println("Rotation left was succesful");
        	  return false;
          case "right":
        	  player.rotate(true);
        	  System.out.println("Rotation right was succesful");
        	  return false;
          case "step":
        	  if(player.move()) {
        		  System.out.println("Move succesful");
        	  	  if(player.checkIfNotFinished()) {
        	  		  System.out.println("Game Finished, you WON");
        	  		  return true;
        	  	  }
        	      return false;
        	  }
        	  else 
        		  System.out.println("There is wall or closed gate or key or you are trying to move out of field");
        	  	  return false;
          case "take":
        	  if(player.tryTakeKey()) { 
        		  System.out.println("Key has been taken");
        	  	  return false;
        	  }
        	  else {
        		  System.out.println("Key could not been taken");
        	      return false;
        	  }
          case "open":
        	  if(player.tryOpenGate()) {
        		  System.out.println("Gate sucessfully opened");
        		  return false;
        	  }
        	  else {
        		  System.out.println("Key missing or gate is already opened");
        	  }
          case "keys":
        	  System.out.println("Pocet klucov : "+player.getKeyCount());  
        	  return false;
          case "show":
              this.print();
              return false;
          case "nope":
              return false;
          case "go":
        	  while(player.move()) {
        	  	  if(player.checkIfNotFinished()) {
        	  		  System.out.println("Game Finished, you WON");
        	  		  return true;
        	  	  } 
        	  }
        	  System.out.println("Movement stopped");
        	  return false;
          case "close":
             // scanned.close();
              //System.exit(0);
        	  return false;
	      default:
              System.err.println("Neexistujuci prikaz");
              return false;
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
