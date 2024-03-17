import processing.sound.*;
import gifAnimation.*;

Gif fireworks_gif;
SoundFile[] music = new SoundFile[4];
SoundFile lost;

Wait wait_end_turn = new Wait(1.2f);
Wait lit[] = new Wait[4];

int X = 360;
int Y = 180;
int SIZE = 300;

PFont f;

float deltaTime;

int[] numbers = new int[10];
IntList guesses = new IntList();

boolean[] states = { false, false, false, false };
boolean[] prev_states = { false, false, false, false };

double count = 0;
int idx = 0;
int level = 0;

boolean turn = false;
boolean pause = true;

double time_interval = 0.8;

boolean[] keyPressedArray = new boolean[256]; // Array to store the state of each key

int sound;
boolean play = false;

boolean hoveringStart = false;
boolean clickedStart = false;
boolean hoveringReset = false;
boolean clickedReset = false;

Wait victory_wait = new Wait(0.35f);
int[] victory_song = { 0, 1, 2, 3, 2, 1, 0, 3, 2 };
int victory_index = 0;
void victory()
{
  setState(states, victory_song[victory_index]);
  if (victory_wait.Update(deltaTime))
  {
    victory_index++;
    setState(states, -1);
    if (victory_index >= victory_song.length)
    {
      victory_index = 0;
      victory_wait.start = false;
    }
    else
      victory_wait.start = true;
  }
}

void setup() {
  size(640, 360);
  noStroke();
  f = createFont("Arial", 16, true);

  for (int i = 0; i < 4; i++)
  {
    lit[i] = new Wait(0.20f);
    music[i] = new SoundFile(this, "data/" + (i + 1) + ".wav");
  }
  lost = new SoundFile(this, "data/error.wav");

  fillNumbers(numbers);
  
  fireworks_gif = new Gif(this, "data/fireworks.gif");
  fireworks_gif.ignoreRepeat();
}

void draw() {
  frameRate(60);
  deltaTime = 1 / frameRate;

  updateButtons();
  update();
  
  if (victory_wait.start)
    victory();

  background(#161616);
  
  if (lost.isPlaying())
  {
    fill(#FF0000);
    circle(X, Y, SIZE + 3);
    fill(#161616);
    circle(X, Y, SIZE);
  }
  
  fill(255, 255, 0, states[0] ? 255 : 100);
  arc(X, Y, SIZE, SIZE, PI, PI + HALF_PI);
  fill(0, 0, 255, states[1] ? 255 : 100);
  arc(X, Y, SIZE, SIZE, PI + HALF_PI, TWO_PI);
  fill(255, 0, 0, states[2] ? 255 : 100);
  arc(X, Y, SIZE, SIZE, HALF_PI, PI);
  fill(0, 255, 0, states[3] ? 255 : 100);
  arc(X, Y, SIZE, SIZE, 0, HALF_PI);
  
  if ((mousePressed && hoveringStart) || (keyPressed && key == ' '))
    clickedStart = true;
  else
    clickedStart = false;  
    
  if (clickedStart)
    fill(#0586c7);
  else if (hoveringStart)
    fill(#08aaf9);
  else 
    fill(#282828);
  //fill(overStart ? #08aaf9 : #282828);
  rect(30, 100, 120, 30, 8);
  
  if ((mousePressed && hoveringReset) || (keyPressed && (key == 'r' || key == 'R')))
    clickedReset = true; 
  else
    clickedReset = false;  
    
  if (clickedReset)
    fill(#0586c7);
  else if (hoveringReset)
    fill(#08aaf9);
  else 
    fill(#282828);
  //fill(overReset ? #08aaf9 : #282828);
  rect(30, 150, 120, 30, 8);

  textFont(f, 20);
  fill(255);
  text(level, 550, 40);

  textFont(f, 16);
  fill(hoveringStart || (keyPressed && key == ' ') ? #ffffff : #c5c5c5);
  text("Start", 70, 121);
  fill(hoveringReset || (keyPressed && (key == 'r' || key == 'R')) ? #ffffff : #c5c5c5);
  text("Reset", 67, 171);


  if (overCircle(X, Y, SIZE / 2))
  {
    //fill(#282828);
    //rect(X + 5, Y + 5, 15, 15, 3);
    //rect(X - 20, Y + 5, 15, 15, 3);
    //rect(X - 20, Y - 20, 15, 15, 3);
    //rect(X + 5, Y - 20, 15, 15, 3);
    
    fill(199, 199, 222);
    textFont(f, 12);
    text("S", X + 8, Y + 17);
    text("A", X - 17, Y + 17);
    text("Q", X - 17, Y - 8);
    text("W", X + 7, Y - 8);
  }
  
  if (fireworks_gif.isPlaying())
    image(fireworks_gif, 230, 60);
}

void update()
{

  if (wait_end_turn.Update(deltaTime))
    turn = false;

  for (int i = 0; i < guesses.size(); i++)
  {
    if (guesses.get(i) != numbers[i])
    {
      windowTitle("You Lost!");
      reset();
      lost.play();
      break;
    }

    if (i == level)
    {
      guesses.clear();
      wait_end_turn.start = true;
      setState(states, -1);
      level++;
      time_interval -= 0.05;
    }
  }

  for (int i = 0; i < 4; i++)
  {
    if (states[i] && states[i] != prev_states[i])
    {
      sound = i;
      play = true;
      prev_states[i] = states[i];
    }
  }
  for (int i = 0; i < 4; i++)
    prev_states[i] = states[i];
  if (play)
  {
    music[sound].play();
    play = false;
  }


  if (level == 10)
  {
    windowTitle("You Won!");
    reset();
    fireworks_gif.play();
    victory_wait.start = true;
  }

  for (int i = 0; i < 4; i++)
    lit[i].Update(deltaTime);

  for (int i = 0; i < 4; i++)
    states[i] = lit[i].start;

  if (!turn && !pause)
    start(time_interval, level);
}

void fillNumbers(int[] numbers)
{
  for (int i = 0; i < 10; i++)
    numbers[i] = (int)random(0, 4);
}

void setState(boolean[] states, int index)
{
  for (int i = 0; i < 4; i++)
    states[i] = false;

  if (index == -1)
    return;
  states[index] = true;
}


void start(double time_interval, int level)
{
  setState(states, numbers[idx]);
  count += deltaTime;
  if (count >= time_interval)
  {
    count -= time_interval;
    idx++;
    setState(states, -1);
    if (idx > level)
    {
      count = 0;
      idx = 0;
      turn = true;
    }
  }
}

void reset()
{
  pause = true;
  count = 0;
  idx = 0;
  turn = false;
  time_interval = 0.8;
  level = 0;
  fillNumbers(numbers);
  guesses.clear();
}

boolean overRect(int x, int y, int width, int height) {
  if (mouseX >= x && mouseX <= x+width &&
    mouseY >= y && mouseY <= y+height) {
    return true;
  } else {
    return false;
  }
}

boolean overCircle(float x, float y, float radius)
{
  if (sqrt(sq(mouseX - x) + sq(mouseY - y)) <= radius)
    return true;
  else
    return false;
}

void updateButtons()
{
  if ( overRect(30, 100, 120, 30) ) {
    hoveringStart = true;
    hoveringReset = false;
  } else if ( overRect(30, 150, 120, 30) ) {
    hoveringStart = false;
    hoveringReset = true;
  } else {
    hoveringStart = hoveringReset = false;
  }
}

void mousePressed() {
  if (hoveringStart) {
    windowTitle("Memory Game");
    pause = false;
  }
  if (hoveringReset) {
    windowTitle("Memory Game");
    reset();
  }
}

void keyPressed() {
  // Get the keycode of the pressed key
  if (!keyPressedArray[keyCode]) {
    keyPressedArray[keyCode] = true;

    if (key == ' ')
    {
      windowTitle("Memory Game");
      pause = false;
    }
    else if (key == 'r' || key == 'R')
    {
      windowTitle("Memory Game");
      reset();
      print("reset");
    }
    else if (key == 'v')
    {
      for (int i = 0; i < 10; i++)
        print(numbers[i] + " ");
      print('\n');
    }

    if (turn && !wait_end_turn.start)
    {
      if (key == 'q' || key == 'Q')
      {
        lit[0].start = true;
        guesses.append(0);
      } else if (key == 'w' || key == 'W')
      {
        lit[1].start = true;
        guesses.append(1);
      } else if (key == 'a' || key == 'A')
      {
        lit[2].start = true;
        guesses.append(2);
      } else if (key == 's' || key == 'S')
      {
        lit[3].start = true;
        guesses.append(3);
      }
    }
  }
}

void keyReleased() {
  // Get the keycode of the released key
  keyPressedArray[keyCode] = false;
}
