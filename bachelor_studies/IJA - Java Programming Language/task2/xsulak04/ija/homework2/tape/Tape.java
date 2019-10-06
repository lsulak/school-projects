package ija.homework2.tape;

public class Tape 
{
    public TapeField[] field_obj;   
    public TapeHead[] head_obj;
    public int id=0;

    public Tape(int f, int h, String format) 
    {
        field_obj = new TapeField[f];
        head_obj = new TapeHead[h];
        
        String pom ="";
        pom = format.replaceAll("\\s","");
        for(int i=0; i<f; i++) 
            field_obj[i] = new TapeField(this,i,String.valueOf(pom.charAt(i)));
    }
    
    public TapeField fieldAt(int i) 
    {
        if(i < field_obj.length) 
            return field_obj[i];
        else 
            return null;
    }
    
    public TapeHead createHead(int i) 
    {
        for(int k=0;k<field_obj.length;k++) 
            if(field_obj[k].canSeize()) 
            {
                TapeHead obj = new TapeHead(i,field_obj[k]);
                head_obj[id] = obj;
                id++;
                field_obj[k].seize(obj);
                return obj;
            }
        return null;
    }
}
