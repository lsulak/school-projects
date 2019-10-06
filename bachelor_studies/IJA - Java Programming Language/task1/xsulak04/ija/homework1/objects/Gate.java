package ija.homework1.objects;

public class Gate extends ija.homework1.tape.TapeObject 
{
    protected boolean locked;
    public Gate(String meno)
    {
        super(meno);
        this.locked = true;
    }
    
   @Override
    protected boolean is_locked()
    {
        return this.locked;
    }
   
   @Override 
   public boolean canSeize()
   {
       if(this.locked)
            return false;
       else
            return true;
   }
   
   @Override 
   public boolean open()
   {
       if(this.locked)
       {
           this.locked = false;
           return true;
       }
       else
           return false;
    }
}
