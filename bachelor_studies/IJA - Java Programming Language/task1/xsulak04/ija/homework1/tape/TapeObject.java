package ija.homework1.tape;
import java.util.Objects;

public abstract class TapeObject 
{
    public String meno;
    
    public TapeObject(String name)
    {
        this.meno = name;
    }
    public abstract boolean canSeize();
    public abstract boolean open();
    protected abstract boolean is_locked();
    
  @Override
    public int hashCode() 
    {
        int hash = 2;
        hash = hash * 17 + Objects.hashCode(this.meno);
        return hash;
    }
    
    @Override
    public boolean equals(Object obj_eq) 
    {
        if(obj_eq == null)
            return false;
        if(!(obj_eq instanceof TapeObject))
                return false;
        if(this.getClass() != obj_eq.getClass())
            return false;
        
        TapeObject nova = (TapeObject) obj_eq;
        if(this.is_locked() != nova.is_locked())
            return false;
        return (this.meno == nova.meno);
    }
}

