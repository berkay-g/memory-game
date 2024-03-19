import processing.sound.*;
import gifAnimation.*;

Gif fireworksGif;
SoundFile[] notes = new SoundFile[4];
SoundFile lostSound;

Delay turnEndDelay = new Delay(1.2f);
Delay lit[] = new Delay[4];

int X = 360;
int Y = 180;
int SIZE = 300;

PFont f;

float deltaTime;

int[] numbers = new int[10];
IntList guesses = new IntList();

boolean[] states = { false, false, false, false };
boolean[] prevStates = { false, false, false, false };

double count = 0;
int idx = 0;
int level = 0;

boolean turn = false;
boolean pause = true;

double timeInterval = 0.8;

boolean[] keyPressedArray = new boolean[256]; // Array to store the state of each key

int note;
boolean play = false;

Button startButton, resetButton;

Delay victorySongDelay = new Delay(0.35f);
int[] victorySong = { 0, 1, 2, 3, 2, 1, 0, 3, 2 };
int victorySongIndex = 0;
void victory()
{
  setState(states, victorySong[victorySongIndex]);
  if (victorySongDelay.Update(deltaTime))
  {
    victorySongIndex++;
    setState(states, -1);
    if (victorySongIndex >= victorySong.length)
    {
      victorySongIndex = 0;
      victorySongDelay.start = false;
    } else
      victorySongDelay.start = true;
  }
}

void setup() {
  frameRate(60);
  size(640, 360);
  noStroke();
  f = createFont("Arial", 16, true);

  for (int i = 0; i < 4; i++)
  {
    lit[i] = new Delay(0.20f);
    notes[i] = new SoundFile(this, "data/" + (i + 1) + ".wav");
  }
  lostSound = new SoundFile(this, "data/error.wav");

  fillNumbers(numbers);

  fireworksGif = new Gif(this, "data/fireworks.gif");
  fireworksGif.ignoreRepeat();

  startButton = new Button(90, 115, 120, 30, 8, #282828, #08aaf9, #0586c7, "Start", #c5c5c5, #ffffff, ' ');
  resetButton = new Button(90, 165, 120, 30, 8, #282828, #08aaf9, #0586c7, "Reset", #c5c5c5, #ffffff, 'r');
}

void draw() {
  deltaTime = 1 / frameRate;

  update();

  if (victorySongDelay.start)
    victory();

  background(#161616);

  if (lostSound.isPlaying())
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

  textFont(f, 20);
  fill(#c5c5c5);
  text(level, 550, 40);

  textFont(f, 16);
  startButton.update();
  startButton.display();
  resetButton.update();
  resetButton.display();

  if (overCircle(X, Y, SIZE / 2))
  {
    fill(199, 199, 222);
    textFont(f, 12);
    text("S", X + 11, Y + 11);
    text("A", X - 11, Y + 11);
    text("Q", X - 11, Y - 11);
    text("W", X + 11, Y - 11);
  }

  if (fireworksGif.isPlaying())
    image(fireworksGif, 230, 60);
}

void update()
{

  if (turnEndDelay.Update(deltaTime))
    turn = false;

  for (int i = 0; i < guesses.size(); i++)
  {
    if (guesses.get(i) != numbers[i])
    {
      windowTitle("You Lost!");
      reset();
      lostSound.play();
      break;
    }

    if (i == level)
    {
      guesses.clear();
      turnEndDelay.start = true;
      setState(states, -1);
      level++;
      timeInterval -= 0.05;
    }
  }

  for (int i = 0; i < 4; i++)
  {
    if (states[i] && states[i] != prevStates[i])
    {
      note = i;
      play = true;
      prevStates[i] = states[i];
    }
  }
  for (int i = 0; i < 4; i++)
    prevStates[i] = states[i];
  if (play)
  {
    notes[note].play();
    play = false;
  }


  if (level == 10)
  {
    windowTitle("You Won!");
    reset();
    fireworksGif.play();
    victorySongDelay.start = true;
  }

  for (int i = 0; i < 4; i++)
    lit[i].Update(deltaTime);

  for (int i = 0; i < 4; i++)
    states[i] = lit[i].start;

  if (!turn && !pause)
    start(timeInterval, level);
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


void start(double timeInterval, int level)
{
  setState(states, numbers[idx]);
  count += deltaTime;
  if (count >= timeInterval)
  {
    count -= timeInterval;
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
  timeInterval = 0.8;
  level = 0;
  fillNumbers(numbers);
  guesses.clear();
}

boolean overCircle(float x, float y, float radius)
{
  return sqrt(sq(mouseX - x) + sq(mouseY - y)) <= radius;
}

void mousePressed() {
  if (startButton.hovering) {
    windowTitle("Memory Game");
    pause = false;
  }
  if (resetButton.hovering) {
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
    } else if (key == 'r' || key == 'R')
    {
      windowTitle("Memory Game");
      reset();
      print("reset");
    } else if (key == 'v')
    {
      for (int i = 0; i < 10; i++)
        print(numbers[i] + " ");
      print('\n');
    }

    if (turn && !turnEndDelay.start)
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
