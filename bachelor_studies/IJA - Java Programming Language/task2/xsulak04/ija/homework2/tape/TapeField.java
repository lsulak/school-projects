package ija.homework2.tape;

import ija.homework2.objects.TapeObject;

public class TapeField {
    protected TapeObject object;
    protected TapeHead head;
    protected Tape tape;
    private int pozicia;

    public TapeField(Tape tape, int p, String type) 
    {
        this.pozicia = p;
        this.object= TapeObject.create(type);
        this.tape = tape;
        
    }
    public TapeField rightField() 
    {
        return tape.fieldAt(pozicia+1);
    }
    
    public boolean canBeOpen() 
    {
        if (object!= null) 
            return object.canBeOpen();
        return false;
    }
    
    public int hashCode() 
    {
        return pozicia;
    }
    
    public int pozicia() 
    {
        return this.pozicia;
    }
    
    public boolean canSeize() 
    {
        if(this.head == null) 
            if(this.object== null) 
                return true;
            return false;
    }
    public TapeHead leave() 
    {
        TapeHead tmp;
        
        if(this.head == null) 
            return null;
        else 
        {
            tmp = this.head;
            this.head = null;
            return tmp;
        }
    }
    
    public boolean seize(TapeHead hlava) 
    {
        if(this.canSeize()) 
        {
            this.head = hlava;
            return true;
        }
        else 
            return false;
    }
    
    public boolean open() 
    {
        if(this.object== null) 
            return false;
        else 
            return object.open();
    }

    public boolean equals(Object obj) 
    {
        if(obj instanceof TapeField) 
        {
            TapeField next = (TapeField)obj;
            if(this.object== null && next.object== null) 
                if(this.head == null && next.head == null)     
                    if(this.pozicia == next.pozicia) 
                        return true;
                    else 
                        return false;
                else if(this.head != null & next.head != null) 
                    if((this.pozicia == next.pozicia)&&(this.head.equals(next.head))) 
                        return true;
                    else 
                        return false;
                else 
                    return false;
            else if(this.object!= null && next.object!= null )
                if(this.head == null && next.head == null)
                    if((object.equals(next.object) &&(pozicia == next.pozicia))) 
                        return true;
                    else 
                        return false;
                else 
                    return false;
            else      
                return false;
        }
        else 
            return false;
    }
}