package ija;
import ija.tape.TapeHead;

import java.net.*;
import java.io.*;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Scanner;
import java.io.FileNotFoundException;

/**
 * Created by mb on 5/6/14.
 */
public class Handler extends Thread {
    Socket dataSocket;
    private ObjectInputStream readBuffer = null;
    private ObjectOutputStream writeBuffer = null;
    private Game game = null;
    private TapeHead player;
    private int ID;
    private ArrayList<Game> openedGames;
    public ArrayList<String> maps = new ArrayList<String>();
    private boolean won = false;

    public Handler(Socket s,ObjectInputStream rB,ObjectOutputStream wB,ArrayList<Game> list_of_games) throws IOException,ClassNotFoundException {
        this.dataSocket = s;
        this.readBuffer = rB;
        this.writeBuffer = wB;
        this.openedGames = list_of_games;
    }

    public void run() {
        try {
			this.chooseGameMode(this.openedGames);
		} catch (ClassNotFoundException | IOException e) {
			e.printStackTrace();
		}
        try {
	        this.player = game.preparePlayer();
	        this.ID = this.player.id();
	        writeBuffer.writeObject(this.player.id());
	        writeBuffer.flush();
	        System.out.println("Handler ended");
	        writeBuffer.writeObject(game);
	        writeBuffer.flush();
        } catch ( IOException e ) {};
        

        while(true) {
        	try {
	            String command = (String) readBuffer.readObject();
            	if(game.GameRunning(this.player,command) == true) {
            		game.gameFinished = true;
            		game.winnerID = this.ID;
            	}
            	writeBuffer.reset();
            	writeBuffer.writeObject(this.game);
            	writeBuffer.flush();
        	} catch (IOException | ClassNotFoundException e) {};
        }
       // dataSocket.close();
    }
    
    public void newGame(ArrayList<Game> list_of_games) throws ClassNotFoundException, IOException {
        this.game = new Game();
        list_of_games.add(this.game);
        Scanner input = this.game.openFile(readBuffer,writeBuffer);
        this.game.readMapFile(input);
        this.game.prepareTable();
    }

    public void chooseGameMode(ArrayList<Game> list_of_games) throws IOException,ClassNotFoundException {
        String reader;

        if((reader = (String) readBuffer.readObject())!= null) {
            System.out.println(reader);
            if(reader.equals("new\r\n")) {
                Game.printAvailableMaps(writeBuffer,maps);
                this.newGame(list_of_games);
            }
            else if(reader.equals("connectto\r\n")) {
                // here will be code for connect to existing games
            	if(!list_of_games.isEmpty()) {
					for (int i = 0; i < list_of_games.size(); i++) {
					   if(!list_of_games.get(i).table.isFull()) {
						   this.game = list_of_games.get(i);
					   }
					}
            	}
            	else {
            		System.err.println("No running game");
            	}
            }
            else {
                System.out.println("programator je hlupy");
            }
        }
    }

}
