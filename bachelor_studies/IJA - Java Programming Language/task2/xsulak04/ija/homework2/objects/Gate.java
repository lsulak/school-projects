package ija.homework2.objects;

public class Gate extends TapeObject
{ 
  protected boolean locked;
    
  public Gate(String name) 
  {
    super(name);
    this.locked = false;
  }
  
  public boolean canBeOpen() 
  {
    return (!locked);
  }
  
  public boolean canSeize() 
  {
    if(this.locked) 
       return true;
    else 
       return false;
  }
  public boolean open() 
  {
    if(this.locked) 
      return false;
    else 
    {
      this.locked = true;
      return true;
    }
  }
}