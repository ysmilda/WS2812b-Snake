#include <Adafruit_NeoPixel.h>

//WS2812B configuration
#define data_pin 11
#define num_pixels 144

//LEDmatrix configuration
#define numx 12
#define numy 12
#define backwards 1 //1 for Z, 0 for S configuration

//Change these to the pins on which you have the buttons
#define up 5
#define down 6
#define right 4
#define left 7

//Refreshrate screen
#define framerate 5
#define velocity 1
#define boot_del 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(num_pixels, data_pin, NEO_GRBW + NEO_KHZ800);

//Used colors
uint32_t black = strip.Color(0, 0, 0, 0);
uint32_t white = strip.Color(0, 0, 0, 255);
uint32_t red = strip.Color(255, 0, 0, 0);

//Variables containing the entirety of the tail
int tailX[numx * numy];
int tailY[numx * numy];

//Initial position
int px = numx / 2;
int py = numy / 2;

int prevX;
int prevY;
int prev2X;
int prev2Y;

//Velocity
int xv;
int yv;

//Coordinate Fruit
byte ax;
byte ay;

//Coordinate to black out
byte pox;
byte poy;

byte points;
bool gameover;

unsigned long last;

void setup()
{
  //Declare all pins with pullup's
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);

  //Start the ledstrip and set its brightness
  strip.begin();
  strip.setBrightness(75);
  strip.show();

  Serial.begin(9600);
}

void loop()
{
  strip.clear();
  strip.show();

  //Generates a random number based on floating pin values
  randomSeed(analogRead(0) + analogRead(1) + analogRead(2));

  //Resets game variables
  gameover = false;
  points = 1;

  //boot up animation, comment it out if you don't want it
  for (int i = 0; i < numx; i++)
  {
    for (int j = 0; j < numy; j++)
    {
      p(i, j, red);
      strip.show();
      delay(boot_del);
    }
  }
  
  for (int i = 0; i < numx; i++)
  {
    for (int j = 0; j < numy; j++)
    {
      p(i, j, black);
      strip.show();
      delay(boot_del);
    }
  }

  //spawns food on random position
  spawnFood();

  while (!gameover)
  {
    last = millis();

    //delay to slow down the game while not missing any inputs
    while ((millis() - last) < (1000 / framerate)) // Waits for 1/framerate seconds while still reading the button input
    {
      //Reads if a button is pressed and changes the velocity accordingly
      if (digitalRead(up) == LOW)
      {
        xv = velocity;
        yv = 0;
      }
      if (digitalRead(down) == LOW)
      {
        xv = -velocity;
        yv = 0;
      }
      if (digitalRead(right) == LOW)
      {
        xv = 0;
        yv = -velocity;
      }
      if (digitalRead(left) == LOW)
      {
        xv = 0;
        yv = velocity;
      }
    }

    if ((px == ax) && (py == ay))
    {
      points++;
      spawnFood();
      Serial.println(points);
    }

    pox = tailX[points - 1];
    poy = tailY[points - 1];

    px += xv;
    py += yv;

    wrap(); 

    //Moves the tail through the array
    prevX = tailX[0];
    prevY = tailY[0];
    tailX[0] = px;
    tailY[0] = py;

    for (int i = 1; i < points; i++)
    {
      prev2X = tailX[i];
      prev2Y = tailY[i];
      tailX[i] = prevX;
      tailY[i] = prevY;
      prevX = prev2X;
      prevY = prev2Y;
    }

    for (int i = 1; i < points; i++)
    {
      if ((px == tailX[i]) && (py == tailY[i]))
      {
        gameover = true;
      }
    }

    p(pox, poy, black); //Makes last position black

    for (int i = 0; i < points; i++)
    {
      p(tailX[i], tailY[i], white);
    }

    strip.show();
  }

}

void spawnFood()
{
  bool spawn = false;
  while ( !spawn) {
    ay = random(0, numy);
    ax = random(0, numx);

    for (int i = 0; i < points; i++) {
      if ((tailX[i] == ax) & (tailY[i] == ay)) {
        ay = random(0, numy);
        ax = random(0, numx);
      }
      else {
        spawn = true;
      }
    }
  }
  p(ax, ay, red);
}

//Turns pixel (x,y) on with the given color
void p(int x, int y, uint32_t color)
{
  int a = (numy - 1 - y) * numx;
  if ((y % 2) == backwards) // y%2 is false when y is an even number - rows 0,2,4,6.
  {
    a += x;
  }
  else // y%2 is true when y is an odd number - rows 1,3,5,7.
  {
    a += numx - 1 - x;
  }
  a %= num_pixels;
  strip.setPixelColor(a, color);
}

//makes the snake wrap around the canvas
void wrap()
{
  if (py < 0)
  {
    py = numy - 1;
  }
  else if (py >= numy)
  {
    py = 0;
  }

  if (px < 0)
  {
    px = numx - 1;
  }
  else if (px >= numx)
  {
    px = 0;
  }
}

