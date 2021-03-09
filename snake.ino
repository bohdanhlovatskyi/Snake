#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <stdlib.h>

// pins for joystick
int VRx = A0;
int VRy = A1;
int SW = 2;

// variables for joystick
int SW_state = 0;

// screen parameters
int screenWidth = 84;
int screenHeight = 48;
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);

int appleX = rand() % screenWidth;
int appleY = rand() % screenHeight;
bool eaten_verbose = false;

int snake[][2] = {{41, 23}, {39, 23}};
int snakeLen = 2;

// variables for convenience
int mainX = snake[0][0];
int mainY = snake[0][1];

int memoX = 1;
int memoY = 0;

void setup()   {
  Serial.begin(9600);
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 

  display.begin();
  display.setContrast(50);
  display.clearDisplay();   // clears the screen and buffer
}


void moveFirstPixel(int mapX, int mapY) {
  // moves the head pixel of the snake
  if ((mapX == 1 && memoX != -1) || (mapX == -1 && memoX != 1)) {
    memoX = mapX;
    mainX = (mainX == 0) ? screenWidth : (mainX + memoX) % screenWidth;
    memoY = 0;
  } else if ((mapY == 1 && memoY != -1) || (mapY == -1 && memoY != 1)) {
    memoY = mapY;
    mainY = (mainY == 0) ? screenHeight : (mainY + mapY) % screenHeight;
    memoX = 0;
  } else {
    mainX = (mainX == 0) ? screenWidth : (mainX + memoX) % screenWidth;
    mainY = (mainY == 0) ? screenHeight : (mainY + memoY) % screenHeight;
  };
  snake[0][0] = mainX;
  snake[0][1] = mainY;
  checkColisions();
}


void moveSnake(int mapX, int mapY) {
  for (int i = snakeLen - 1; i > 0; i--){
    snake[i][0] = snake[i - 1][0];
    snake[i][1] = snake[i - 1][1];
  };
  moveFirstPixel(mapX, mapY);
};


void showApple() {
  if (eaten_verbose == true) {
    appleX = rand() % screenWidth;
    appleY = rand() % screenHeight;
    eaten_verbose = false;
  }
  display.fillRect(appleX, appleY, 4, 4, BLACK);
}

void eatApple() {
  if (appleX == mainX & appleY == mainY) {
    eaten_verbose = true;
    // copies the last part of the snake
    snake[snakeLen][0] = snake[snakeLen - 1][0];
    snake[snakeLen][1] = snake[snakeLen - 1][1];
    snakeLen++;
  };
};


void checkColisions() {
  for (int i = 1; i < snakeLen; i++) {
    if (mainX == snake[i][0] || mainY == snake[i][1]) {
      return;
    }
  }
}


void displaySnake() {
  for (int i=0; i < snakeLen; i++) {
    display.fillRect(snake[i][0], snake[i][1], 4, 4, BLACK);
  };
};
  

void loop() {
  // SW_state = digitalRead(SW);
  int mapX = map(analogRead(VRx), 0, 1023, -1, 1);
  int mapY = map(analogRead(VRy), 0, 1023, -1, 1);
  
  moveSnake(mapX, mapY);

  eatApple(); // first apple is shown by default, therefore we should eat it at first to generate the next one
  showApple(); // next apple

  displaySnake();
  
  display.display();
  delay(100); // here we can create custom delay, that will become less, if the snake becomes bigger
  display.clearDisplay();
}