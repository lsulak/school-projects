package ija.homework2.objects;

public abstract class TapeObject 
{
    public String name;
    public abstract boolean canBeOpen();
    public abstract boolean canSeize();
    public abstract boolean open();

    public static TapeObject create(String format) 
    {
        if (format.equals("g")) {
            Gate gate_object = new Gate("g");
            return gate_object;
        }
        else if(format.equals("w")) {
            Wall wall_obj = new Wall("w");
            return wall_obj;
        }
        else 
            return null;
    }

    public TapeObject(String name) 
    {
        this.name = name ;
    }
    
    public int hashCode() 
    {
        return name.hashCode();
    }
}