package ija.homework1.tape;
import java.util.Objects;

public class TapeHead 
{
    private int identif;
    public TapeHead(int id)
    {
        this.identif = id;
    }
    public int id()
    {
        return identif;
    }
    
    @Override
    public int hashCode() 
    {
        int hash = 2;
        hash = hash * 17 + Objects.hashCode(this.identif);
        return hash;
    }
    
    @Override
    public boolean equals(Object obj_eq) 
    {
        if(obj_eq == null)
            return false;
        if(obj_eq == this) 
                return true;
        if(!(obj_eq instanceof TapeHead))
                return false;
        if(this.getClass() != obj_eq.getClass())
            return false;
        
        TapeHead nova = (TapeHead) obj_eq;
        return (this.identif == nova.identif);
    }
    
}
