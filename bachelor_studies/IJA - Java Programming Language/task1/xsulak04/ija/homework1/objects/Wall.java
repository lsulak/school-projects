package ija.homework1.objects;

public class Wall extends ija.homework1.tape.TapeObject
{
    protected boolean locked;
    public Wall(String meno)
    {
        super(meno);
    }
    
    @Override
    public boolean canSeize()
    {
        return false;
    }
            
    @Override 
    public boolean open()
    {
        return false;
    }
     
    @Override
    protected boolean is_locked()
    {
        return this.locked;
    }
}
