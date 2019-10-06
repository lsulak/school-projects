package ija.homework2.tape;

public class TapeHead 
{
    private int id;
    private TapeField field_obj;
    private int kluce;
    
    public int hashCode() 
    {
        return this.id;
    }
    public boolean equals(Object obj) 
    {
        if(obj instanceof TapeHead) 
        {
            TapeHead next = (TapeHead)obj;
            if(this.id != next.id) 
                return false;
            else 
                return true;
        }
        else 
            return false;
    }
    public TapeHead(int id, TapeField f) 
    {
        this.id = id;
        this.field_obj = f; 
    }
    public int id() 
    {
        return this.id;
    }
    public TapeField seizedField() 
    {
        return field_obj;
    }
    
    public void addKeys(int n) 
    {
        this.kluce += n;
    }
    public boolean moveRight() 
    {
        TapeField rightField = field_obj.rightField();
        field_obj.leave();
        while(rightField != null) 
        {
            if(rightField.canSeize()) 
            {
                rightField.seize(this);
                field_obj = rightField;
                return true;
            }
            else 
                if(kluce > 0 && rightField.canBeOpen()) 
                {
                    rightField.open();
                    kluce--;   
                    rightField.seize(this);
                    field_obj = rightField;
                    return true;
                }
            rightField = rightField.rightField();
        }
        field_obj.seize(this);
        return false;
    }
}