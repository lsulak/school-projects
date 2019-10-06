package ija.homework2.objects;

public class Wall extends TapeObject
{
    public Wall(String name) 
    {
        super(name);
    }
    
    public boolean canBeOpen() 
    {
        return false;
    }
    
    @Override
    public boolean canSeize() {
        return false;
    }
    @Override
    public boolean open() {
        return false;
    }
}