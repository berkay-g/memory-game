class Delay
{
  protected float counter;

  float seconds;
  boolean start;
  Delay(float seconds)
  {
    this.seconds = seconds;
    this.start = false;
    this.counter = 0.f;
  }
  
  boolean Update(float deltaTime)
  {
    if (this.start)
    {
      this.counter += deltaTime;
      if (this.counter >= seconds)
      {
        counter -= seconds;
        this.start = false;
        return true;
      }
    }
    return false;
  }
};
