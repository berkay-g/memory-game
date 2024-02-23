import processing.sound.*;
SoundFile[] music = new SoundFile[4];

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
int level = 9;

boolean turn = false;
boolean pause = true;

boolean wait = false;
double wait_counter = 0.0;

double time_interval = 0.8;

boolean[] lit = { false, false, false, false };
double[] lit_counter = { 0, 0, 0, 0 };

boolean key_hold = false;

int sound;
boolean play = false;

boolean overStart = false;
boolean overReset = false;

void setup() {
  size(640, 360);
  noStroke();
  f = createFont("Arial", 16, true);

  music[0] = new SoundFile(this, "data/1.wav");
  music[1] = new SoundFile(this, "data/2.wav");
  music[2] = new SoundFile(this, "data/3.wav");
  music[3] = new SoundFile(this, "data/4.wav");

  fillNumbers(numbers);
  for (int i = 0; i < 10; i++)
    print(numbers[i] + " ");
  print('\n');
}

void draw() {
  frameRate(60);
  deltaTime = 1 / frameRate;

  updateButtons();
  update();

  background(#161616);
  fill(255, 255, 0, states[0] ? 255 : 100);
  arc(X, Y, SIZE, SIZE, PI, PI + HALF_PI);
  fill(0, 0, 255, states[1] ? 255 : 100);
  arc(X, Y, SIZE, SIZE, PI + HALF_PI, TWO_PI);
  fill(255, 0, 0, states[2] ? 255 : 100);
  arc(X, Y, SIZE, SIZE, HALF_PI, PI);
  fill(0, 255, 0, states[3] ? 255 : 100);
  arc(X, Y, SIZE, SIZE, 0, HALF_PI);

  fill(overStart ? #08aaf9 : #282828);
  rect(30, 100, 120, 30, 10);
  fill(overReset ? #08aaf9 : #282828);
  rect(30, 150, 120, 30, 10);

  textFont(f, 20);
  fill(255);
  text(level, 550, 40);

  textFont(f, 16);
  fill(overStart ? #ffffff : #c5c5c5);
  text("Start", 70, 121);
  fill(overReset ? #ffffff : #c5c5c5);
  text("Reset", 67, 171);
}

void mousePressed() {
  if (overStart) {
    windowTitle("Memory Game");
    pause = false;
  }
  if (overReset) {
    windowTitle("Memory Game");
    reset();
  }
}

void updateButtons()
{
  if ( overRect(30, 100, 120, 30) ) {
    overStart = true;
    overReset = false;
  } else if ( overRect(30, 150, 120, 30) ) {
    overReset = true;
    overStart = false;
  } else {
    overReset = overStart = false;
  }
}

void update()
{
  if (wait)
    wait_counter += deltaTime;
  if (wait_counter >= 1.2)
  {
    wait_counter -= 1.2;
    wait = false;
    turn = false;
  }

  for (int i = 0; i < guesses.size(); i++)
  {
    if (guesses.get(i) != numbers[i])
    {
      windowTitle("You Lost!");
      reset();
      break;
    }

    if (i == level)
    {
      guesses.clear();
      wait = true;
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
  }

  if (keyPressed && !key_hold)
  {
    key_hold = true;
    if (key == ' ')
    {
      windowTitle("Memory Game");
      pause = false;
    }
    if (turn && !wait)
    {
      if (key == 'q' || key == 'Q')
      {
        lit[0] = true;
        guesses.append(0);
      } else if (key == 'w' || key == 'W')
      {
        lit[1] = true;
        guesses.append(1);
      } else if (key == 'a' || key == 'A')
      {
        lit[2] = true;
        guesses.append(2);
      } else if (key == 's' || key == 'S')
      {
        lit[3] = true;
        guesses.append(3);
      } else if (key == 'v')
      {
        for (int i = 0; i < 10; i++)
          print(numbers[i] + " ");
        print('\n');
      }
    }
  }


  for (int i = 0; i < 4; i++)
  {
    if (lit[i])
      lit_counter[i] += deltaTime;
    if (lit_counter[i] >= 0.20)
    {
      lit_counter[i] -= 0.20;
      lit[i] = false;
    }
  }

  for (int i = 0; i < 4; i++)
  {
    if (lit[i])
    {
      states[i] = true;
    } else
      states[i] = false;
  }

  if (!turn && !pause)
    start(time_interval, level);
}

void fillNumbers(int[] numbers)
{
  for (int i = 0; i < 10; i++)
    numbers[i] = (int)random(0, 4);
}

void setState(boolean[] states, int idx)
{
  for (int i = 0; i < 4; i++)
  {
    states[i] = false;
  }
  if (idx == -1)
    return;
  states[idx] = true;
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

void keyReleased()
{
  key_hold = false;
}
