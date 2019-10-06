package ija.graphics;

import ija.Client;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.nio.file.Path;
import java.nio.file.Paths;

import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JPanel;

public class GameField extends JPanel {
	Client client;
	
	public GameField(Client client) {
		this.client = client;
	}
	
	@Override
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		g.setColor(Color.green);
     //   Path currentRelativePath = Paths.get("");
     //   String s = currentRelativePath.toAbsolutePath().toString();
    //    System.out.println("Path: " + s); 
	//	Image image = new ImageIcon("koci.jpg").getImage();
	//	g.drawImage(image,3,4,this);
		g.fillRect(0, 0, client.game.width*15, client.game.height*15);	
		int x = 0,y = 0;
		
		if(client.gameStarted() == true){
			Image image;
			if(client.WON == 1) {	
				image = new ImageIcon("./img/ball.png").getImage();
				g.drawImage(image,0,0,this);
			}
			else if(client.WON == 2) {
				image = new ImageIcon("./img/loose.jpg").getImage();
				g.drawImage(image,0,0,this);	
			}
			else {
			    for (int i=0; i < client.game.height; i++) {
		    		for (int j=0; j < client.game.width; j++) {
		    			String tmp = "e";
		    			
		    			if(client.game.table.field[i][j].head != null) {
		    				tmp = "";
		    				switch (client.game.table.field[i][j].head.direction) {
							case 1:
			    				image = new ImageIcon("./img/head3T.png").getImage();
			    				g.drawImage(image,x,y,this);	
								break;
							case 2:
			    				image = new ImageIcon("./img/head3R.png").getImage();
			    				g.drawImage(image,x,y,this);	
								break;
							case 3:
			    				image = new ImageIcon("./img/head3D.png").getImage();
			    				g.drawImage(image,x,y,this);	
								break;
							case 4:
			    				image = new ImageIcon("./img/head3L.png").getImage();
			    				g.drawImage(image,x,y,this);	
								break;
							default:
								break;
							}
		    			}
		    			else if(client.game.table.field[i][j].obj != null) {
		    						tmp = client.game.table.field[i][j].obj.name;
		    			}
		    			switch(tmp){
		    			case "w":
		    				image = new ImageIcon("./img/wall.png").getImage();
		    				g.drawImage(image,x,y,this);	
		    				break;
		    			case "e":
		    				g.setColor(Color.green);
		    				g.fillRect(x, y, 15,15);
		    				break;
	    				case "g":
	    					image = new ImageIcon("./img/gateCLosed.png").getImage();
	    					g.drawImage(image,x,y,this);	
	    					break;
	    				case "f":
	    					image = new ImageIcon("./img/finish.png").getImage();
	    					g.drawImage(image,x,y,this);	
	    					break;
	    				case "k":
	    					image = new ImageIcon("./img/key.png").getImage();
	    					g.drawImage(image,x,y,this);	
	    					break;
		    			}	
		    			
		    			//System.out.print(" " + tmp);
		    			x += 15;
		    		}
		    			x = 0;
		    			y+=15;
		    		//	System.out.println("");
			    }
			}
		}
	}	
}
