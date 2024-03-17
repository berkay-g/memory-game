class Button {
  int x, y; // Position of the button
  int w, h; // Size of the button
  int r;
  int buttonColor, hoverColor, clickColor; // Colors for different states of the button
  int textColor, textHoverColor;
  boolean hovering = false;
  boolean clicked = false;
  String buttonText;
  char activate_key = 0;

  Button(int x, int y, int w, int h, int r, int buttonColor, int hoverColor, int clickColor, String buttonText, int textColor, int textHoverColor, char activate_key) 
  {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
    this.r = r;
    this.buttonColor = buttonColor;
    this.hoverColor = hoverColor;
    this.clickColor = clickColor;
    this.buttonText = buttonText;
    this.textColor = textColor;
    this.textHoverColor = textHoverColor;
    this.activate_key = activate_key;
  }

  void update() {
    hovering = overButton();
    char keyLowerCase = Character.toLowerCase(key);
    if ((mousePressed && hovering) || ((keyPressed && keyLowerCase == activate_key) && key != 0)) {
      clicked = true;
    } else {
      clicked = false;
    }
  }

  void display() {
    if (clicked) {
      fill(clickColor);
    } else if (hovering) {
      fill(hoverColor);
    } else {
      fill(buttonColor);
    }
    rectMode(CENTER);
    rect(x, y, w, h, r);
    
    char keyLowerCase = Character.toLowerCase(key);
    fill(hovering || ((keyPressed && keyLowerCase == activate_key) && key != 0) ? textHoverColor : textColor);
    textAlign(CENTER, CENTER);
    text(buttonText, x, y);
  }

  boolean overButton() {
    return mouseX > x - w / 2 && mouseX < x + w / 2 && mouseY > y - h / 2 && mouseY < y + h / 2;
  }
}
