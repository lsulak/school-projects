package ija;


import ija.graphics.Basic;
import ija.graphics.GameField;
import ija.tape.TapeHead;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.Scanner;

import javax.swing.JButton;

/**
 * Created by hovnoteam, all rights reserved
 */
public class Client {
    public static final int PORT = 62579; // Our unicate port number, where will server run
    protected Socket clientSocket;
    public Game game;
    private ObjectInputStream getMessage;
    private ObjectOutputStream sendMessage;
    private boolean gameStarted = false;
    public String serverName = "localhost";
    private boolean newField = false;
    private int ID;
    public int WON = 0;
    public ArrayList<String> maps = new ArrayList<String>();

    public Client(int PORT)
    {
        try{
            this.clientSocket = new Socket(serverName, PORT);
            sendMessage = new ObjectOutputStream(clientSocket.getOutputStream());
            sendMessage.flush();
            getMessage = new ObjectInputStream(clientSocket.getInputStream());
        }
        catch (NoRouteToHostException e) {
            System.err.println("Host is not availible" + e);
            System.exit(1);
        }
        catch (ConnectException e) {
            System.err.println("Connection denied" + e);
            System.exit(1);
        }
        catch (java.io.IOException e) {
            System.err.println("There was an error during connecting to server" + e);
            System.exit(1);
        }
    }
    public static void main(String [] argv) throws IOException, ClassNotFoundException {	
    	Client client = new Client(PORT);
    	final Basic frame = new Basic(client);
		frame.setVisible(true);	
    }

    public void updateGame() throws IOException,ClassNotFoundException {
        this.game = (Game)getMessage.readObject();
        this.gameStarted = true;
        if(game.gameFinished) {
        	if(game.winnerID == this.ID) {
        		this.WON = 1;
        	}
        	else {
        		this.WON = 2;
        	}
        }
      //  this.game.print();
    }
    
    public void gameID() {
    	try {
			this.ID = (int) getMessage.readObject();
		} catch (ClassNotFoundException | IOException e) {
			e.printStackTrace();
		}
    }


    public void gameMode(int i) throws IOException, ClassNotFoundException {
	  String sendtoServer;
	  String fromServer = "";
	  boolean flag = true;
	  try {
	  	if(i == 1){
	  		sendtoServer = "new\r\n";	
	  	}
	  	else {
	  		sendtoServer = "connectto\r\n";
	  		flag = false;
	  	}
	      fromServer = (String)getMessage.readObject();   // confirmation that client is accepted by server
	      if(fromServer.equals("OKlisten")) {
	          sendMessage.writeObject(sendtoServer); //send new or connectto  -> new game,or connect to already started game
	          sendMessage.flush();
	      }
	      if(flag) {
			  maps = ((ArrayList<String>)getMessage.readObject());
			  //System.err.println("ABCDEFGHIJKLMNOP");
			 // System.err.println(getMessage.readObject());
			//  System.err.println("ABCDEFGHIJKLMNOP");
			  System.err.println(getMessage.readObject());
	      }
	  }
	  catch (NoRouteToHostException e) {
	      System.err.println("Host is not availible" + e);
	      System.exit(1);
	  }
	  catch (ConnectException e) {
	      System.err.println("Connection denied" + e);
	      System.exit(1);
	  }
	  catch (java.io.IOException e) {
	      System.err.println("There was an error during connecting to server" + e);
	      System.exit(1);
	  }
	}

    public void sendMap(String mapName) { 
		String fromServer = "";
		int tmp = 0;
		try {
	
	  // ask for map and send which map
	     sendMessage.writeObject("game");
	     sendMessage.flush();
	     sendMessage.writeObject(mapName);
	     sendMessage.flush();
	     if((fromServer = (String)getMessage.readObject()).equals("OK")) {
	         System.out.println("Map is loading");
	         this.gameStarted = true;
	     }
	     else if(fromServer.equals("EXC")) {
	         System.out.println("Map was not found, try again");
	     }
		} catch (ClassNotFoundException | IOException e) {
			e.printStackTrace();
		}
    }

    public void gameCommands(String command) throws IOException {

       /* Scanner scanned = new Scanner(System.in);
        command = scanned.next();*/
        switch(command)
        {
            case "game":
                System.out.println("Firstly, close your actual game and then you can start new game ");
                break;
            case "left":
                sendMessage.writeObject("left");
                System.out.println("Rotation left was succesful");
                break;
            case "right":
                sendMessage.writeObject("right");
                System.out.println("Rotation right was succesful");
                break;
            case "step":
                sendMessage.writeObject("step");
                break;
            case "take":
                sendMessage.writeObject("take");
                break;
            case "open":
                sendMessage.writeObject("open");
                break;
            case "keys":
                sendMessage.writeObject("keys");
                break;
            case "nope":
                sendMessage.writeObject("nope");
                break;
            case "go":
                sendMessage.writeObject("go");
                break;
            case "close":
                sendMessage.writeObject("close");
                //scanned.close();
                System.exit(0);
            default:
                sendMessage.writeObject("BAD");
                System.err.println("Neexistujuci prikaz");
                break;
        }
    }
    
    public boolean gameStarted() {
    	return this.gameStarted;
    }
    
    public boolean newField() {
    	return this.newField;
    }
    
    public int getID() {
    	return this.ID;
    }
}