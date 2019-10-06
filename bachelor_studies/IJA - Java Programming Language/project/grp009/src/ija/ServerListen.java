package ija;
import java.net.*;
import java.util.ArrayList;
import java.io.*;

/**
 * Created by hovnoteam, all rights reserved
 */
public class ServerListen {
    protected ServerSocket socket;
    // Our unicate port number, where will server run
    public static final int PORT = 62579;
    
    public ArrayList<Game> list_of_games = new ArrayList<Game>();
    
    public ServerListen(int PORT) {
        try {
            this.socket = new ServerSocket(PORT);
        } catch (IOException e) {
            System.err.println(e);
            System.exit(1);
        }
    }

    protected void acceptConnection() throws IOException,ClassNotFoundException {
        Socket dataSocket = null;
        ObjectInputStream readBuffer = null;
        ObjectOutputStream writeBuffer = null;
        while( true ) {
            try{
                dataSocket = socket.accept();
                System.err.println("!!! Connection !!!");
                writeBuffer = new ObjectOutputStream(dataSocket.getOutputStream());
                writeBuffer.flush();
                readBuffer = new ObjectInputStream((dataSocket.getInputStream()));

                writeBuffer.writeObject("OKlisten");
                writeBuffer.flush();
                System.err.println("Client connected");
                new Handler(dataSocket,readBuffer,writeBuffer,list_of_games).start();
            } catch (IOException e) {
                System.err.println(e);
                System.exit(1);
            }
        }
    }

    public static void main(String [] argv) throws IOException,ClassNotFoundException {
        ServerListen server = new ServerListen(PORT);
        server.acceptConnection();
    }

}
