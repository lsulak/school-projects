package ija.graphics;

import ija.Client;

import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.Timer;
import javax.swing.border.EmptyBorder;
import javax.swing.JButton;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.JTextField;

import java.io.IOException;

public class Basic extends JFrame {

	private JPanel contentPane, panel;
	public JTextField edit;
	public JLabel label;
	public JComboBox<String> chooseMap;
  	public JButton Button1, Button2, Button3, Button4;
  	public int gameHeight, gameWidth;
	public GameField gameField;

	
	public Basic(final Client client) {
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setSize(300,400);
		setLocationRelativeTo(null);
		
		contentPane = new JPanel();
		gameField = new GameField(client);
		panel = new JPanel();
		
		contentPane.setBorder(new EmptyBorder(5,5,5,5));
		setContentPane(contentPane);
		contentPane.setLayout(null);
		contentPane.add(panel);
		
		 panel.setLayout(null);
		 panel.setBounds(0, 0, 1124, 1100);
		 label = new JLabel("Choose a map");
		 label.setBounds(105, 40, 150, 25);
		
		 Button1 = new JButton("New Game");	
     	 Button1.setBounds(87, 50, 125, 25);
		 panel.add(Button1);
      
		 Button2 = new JButton("Join Game");
         Button2.setBounds(87, 100, 125, 25);
		 panel.add(Button2);
      
		 Button3 = new JButton("Exit Game");
         Button3.setBounds(87, 300, 125, 23);
	     panel.add(Button3);
      
		 Button4 = new JButton("Start");
         Button4.setBounds(87, 250, 125, 23);
		
		 chooseMap = new JComboBox<String>();
		 chooseMap.setBounds(87, 65, 125, 25);
		
	     edit = new JTextField();
	     
			final Timer timer = new Timer( 1000, new ActionListener(){
				  @Override
				  public void actionPerformed( ActionEvent e ){
	                 try {
	                	client.gameCommands("nope");
						client.updateGame();
						gameField.repaint();
					} catch (ClassNotFoundException | IOException e1) {
						e1.printStackTrace();
					} 
				  }
				} );
				timer.setRepeats( true );
	     
		 Button3.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				System.exit(0);
			}
		});
		
		
		
		Button2.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				Button1.setVisible(false);
				Button2.setVisible(false);
				try {
					client.gameMode(2);
					client.gameID();
					panel.add(edit);
					int id = client.getID();
					Basic.this.setTitle("Player "+String.valueOf(id));
					client.updateGame();
					gameHeight = client.game.height*15;
			        gameWidth = client.game.width*15;
					setSize(gameWidth+15,gameHeight+100);
					setLocationRelativeTo(null);
					edit.setBounds(gameWidth/5, gameHeight+25, gameWidth/2, 30);
					panel.add(edit);
					gameField.setBounds(0 ,0, gameWidth, gameHeight);
					panel.add(gameField);
					gameField.repaint();	
					timer.start();
				} catch (ClassNotFoundException | IOException e1) {
					e1.printStackTrace();
				}
			}
		});
				
		
		Button1.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				try {
					Button1.setVisible(false);
					Button2.setVisible(false);
					panel.add(label);
					panel.add(chooseMap);
					panel.add(Button4);
					setSize(300, 500);
	                setLocationRelativeTo(null);
	                client.gameMode(1);
					for (int i = 0; i < client.maps.size(); i++)
					{
						//System.err.println("vo stvorici po opici");
					//	System.out.println(client.maps.get(i));
						chooseMap.addItem(client.maps.get(i));
					}
                  
	
				} catch (ClassNotFoundException | IOException e1) {
					e1.printStackTrace();
				}	
			}
		});
			

		Button4.addMouseListener(new MouseAdapter() {
		@Override
		public void mouseClicked(MouseEvent e) {
			try {
				
				String command = (String) chooseMap.getSelectedItem();
				client.sendMap(command);
				client.gameID();
				int id = client.getID();
				Basic.this.setTitle("Player "+String.valueOf(id));
				client.updateGame();
				gameField.repaint();
				timer.start();
				
				panel.remove(chooseMap);
				panel.invalidate();
				
				panel.remove(label);
				panel.invalidate();
				
				
				panel.remove(Button4);
				panel.invalidate();
				
				gameHeight = client.game.height*15;
		        gameWidth = client.game.width*15;
				setSize(gameWidth+15,gameHeight+100);
				setLocationRelativeTo(null);
				
				edit.setBounds(gameWidth/5, gameHeight+25, gameWidth/2, 30);
				panel.add(edit);
				
				gameField.setBounds(0 ,0, gameWidth, gameHeight);
				panel.add(gameField);
             

			} catch (ClassNotFoundException | IOException e1) {
				e1.printStackTrace();
			}
		}
		});
		
		edit.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				String command = edit.getText();
				try {
					client.gameCommands(command);
                    client.updateGame();
                    gameField.repaint();
                    edit.setText("");
				} catch (IOException | ClassNotFoundException e) {
					e.printStackTrace();
				}
			}
		});
		
		
	} 
}