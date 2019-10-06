package ija.homework1.tape;
import java.util.Objects;

public class TapeField 
{
    private int position;
    private int status;
    private TapeObject objekt = null;
    private TapeHead hlava;
    
    public TapeField(int p)
    {
        this.position = p;
    }

    public TapeField(int p, TapeObject obj)
    {
        this.position = p;
        this.objekt = obj;
    }
    
    public int position()
    {
        return this.position;
    }
    
    public boolean seize(TapeHead head)
    {
        if(this.canSeize())
        {
            this.hlava = head;
            status = 1;
            return true;
        }
        else 
            return false;
    }
    public TapeHead leave()
    {
        if(objekt!= null)
        {
            objekt = null;
            status = 0;
            return hlava;
        }
        if(status == 1)
        {
            objekt = null;
            status = 0;
            return hlava;
        }
        else return null;
    }
    
    public boolean canSeize()
    {
        if(this.status == 0 && (this.objekt == null || objekt.canSeize() == true))
            return true;
        else 
            return false;
    }
    
    public boolean open()
    {
        if(this.objekt == null)
            return false;
        else
            return objekt.open();
    }
    
    @Override
    public int hashCode() 
    {
        int hash = 2;
        int hash2 = 0;
        hash = hash * 17 + Objects.hashCode(this.position);
        hash2 = hash* 17 + Objects.hashCode(this.status);
        return hash+hash2;
    }
    
    @Override
    public boolean equals(Object obj_eq) 
    {
        if(obj_eq == null)
            return false;
        if(this.getClass() != obj_eq.getClass())
            return false;
        
        TapeField nova = (TapeField) obj_eq;
        if(!Objects.equals(this.position ,nova.position))
            return false;
            
        if(!Objects.equals(this.objekt, nova.objekt))
            return false;
            
        else
            return true;
    }
}
