/*
 * Square.ino
 * Created by Fabio de Albuquerque Dela Antonio
 * fabio914 at gmail.com
 * 
 * An OpenGL example that draws a rotated and scaled square.
 */
#include "SPI.h"
#include "ST7789_t3.h"
#include <ArduinoGL.h> 

// For the Adafruit shield, these are the default.
#define TFT_DC  9
#define TFT_CS 10

Arduino_OpenGL tft = Arduino_OpenGL(TFT_CS, TFT_DC, 8);

// Color definitions
#define BLACK       0x0000      /*   0,   0,   0 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */

uint8_t use_fb = 1;
  /* Read the rotation angle from a potentiometer attached to pin A0 */
  float angleMinutes = 0.f;
  
  static float scale = 3.8, angleInc = 1.0;
  const float maxAngle = 359.0, minAngle = 0.0;

int16_t clockPos[3] = {0, 0, 50}; //x,y,r
uint8_t currentTime[3] = {0, 0, 0}; //hh,mm,ss

//background,face,hh,mm,ss
const uint16_t clockColors[5] = {ST77XX_BLACK, ST77XX_BLACK, ST77XX_CYAN, ST77XX_BLUE, ST77XX_RED};

//houX,houY,minX,minY,secX,secY
uint16_t oldPos[6] = {0, 0, 0, 0, 0, 0};
unsigned long targetTime = 0;
float hx, mx, sx;

extern const unsigned short drake_head_320_320[];

void setup() {
  tft.init(240, 320);
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.println("Waiting for Arduino Serial Monitor...");

  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("OpenGL Test!"); 

  delay(1000);
  tft.useFrameBuffer(use_fb);

  /* Pass the canvas to the OpenGL environment */
  tft.glClear(BLACK);

    
  tft.glMatrixMode(GL_PROJECTION);
  tft.glLoadIdentity();
  tft.gluOrtho2D(-5.0, 5.0, -6.0, 6.0);
    
  tft.glMatrixMode(GL_MODELVIEW);


  clockPos[0] = tft.width() / 2;
  clockPos[1] = tft.height() / 2;
  clockPos[2] = (tft.height() / 2) - 20;
  //clockPos[2] = 128;
  drawGauge(clockPos, 0, 360);
  oldPos[0] = clockPos[0]; oldPos[1] = clockPos[1];
  oldPos[2] = clockPos[0]; oldPos[3] = clockPos[1];
  oldPos[4] = clockPos[0]; oldPos[5] = clockPos[1];
  //get current time from compiler
  currentTime[0] = conv2d(__TIME__);
  currentTime[1] = conv2d(__TIME__ + 3);
  currentTime[2] = conv2d(__TIME__ + 6);
  targetTime = millis() + 1000;

}

void loop() {
  if (targetTime < millis()) {
    targetTime = millis() + 1000;
    currentTime[2]++;
    if (currentTime[2] == 60) {
      currentTime[2] = 0;
      currentTime[1]++;
      if (currentTime[1] > 59) {
        currentTime[1] = 0;
        currentTime[0]++;
        if (currentTime[0] > 23) currentTime[0] = 0;
      }
    }
  }
  tft.glClear(BLACK); 
  tft.writeRect(0, 0, 320, 320, drake_head_320_320);
  drawGauge(clockPos, 0, 360);
  drawClockHands(clockPos, currentTime);
  tft.updateScreen();

}

void drawGauge(int16_t pos[], uint16_t from, uint16_t to) {
  tft.drawCircle(pos[0], pos[1], pos[2], ST77XX_WHITE); //draw instrument container
  tft.drawCircle(pos[0], pos[1], pos[2] + 1, ST77XX_WHITE); //draw instrument container
  roundGaugeTicker(pos[0], pos[1], pos[2], from, to, 1.3, ST77XX_WHITE);
  if (pos[2] > 15) roundGaugeTicker(pos[0], pos[1], pos[2], from + 15, to - 15, 1.1, ST77XX_WHITE); //draw minor ticks
}

void roundGaugeTicker(uint16_t x, uint16_t y, uint16_t r, int from, int to, float dev, uint16_t color)
{
  float dsec;
  int i;
  for (i = from; i <= to; i += 30) {
    dsec = i * (PI / 180);
    tft.drawLine(
      x + (cos(dsec) * (r / dev)) + 1,
      y + (sin(dsec) * (r / dev)) + 1,
      x + (cos(dsec) * r) + 1,
      y + (sin(dsec) * r) + 1,
      color);
  }
}


static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9') v = *p - '0';
  return 10 * v + *++p - '0';
}

void drawClockHands(int16_t pos[], uint8_t curTime[]) {
  hx = (curTime[0] * 30 + (curTime[1] * 6 + (curTime[2] * 6) * 0.01666667) * 0.0833333);
  mx = curTime[1] * 6; 
  sx = (curTime[2]*6);
  Serial.printf("%d, %d, %d\n", curTime[0], curTime[1], curTime[2]);

  drawSeconds();
  drawMinutes();
  drawHours();
}


void drawSeconds(){
  tft.glLoadIdentity();
  tft.glRotatef(-sx, 0.f, 0.f, 1.f);
  tft.glScalef(scale, scale, 0.f);
  tft.glColor3i(255,0,0);
  tft.glBegin(GL_TRIANGLE_STRIP);
    tft.glVertex3f( 0.f, 1.0f, 0.f);
    tft.glVertex3f(-0.05f, 0.f, 0.f);
    tft.glVertex3f( 0.05f, 0.f, 0.f);
  tft.glEnd();
  //tft.updateScreen();
}

void drawMinutes(){
  tft.glLoadIdentity();
  tft.glRotatef(-mx, 0.f, 0.f, 1.f);
  tft.glScalef(3.5f, 3.5f, 0.f);
  tft.glColor3i(0,0,255);
  tft.glBegin(GL_TRIANGLE_STRIP);
    tft.glColorT(0, 0, 0, 255);
    tft.glVertex3f( 0.f, 1.f, 0.f);
    tft.glVertex3f(-0.05f, 0.f, 0.f);
    tft.glVertex3f( 0.05f, 0.f, 0.f);
  tft.glEnd();
  //tft.updateScreen();
}

void drawHours(){
  tft.glLoadIdentity();
  tft.glRotatef(-hx, 0.f, 0.f, 1.f);
  tft.glScalef(3.5f, 3.5f, 0.f);
  tft.glColor3i(0,255,0);
  tft.glBegin(GL_TRIANGLE_STRIP);
    tft.glColorT(0, 0, 0, 255);
    tft.glVertex3f( 0.f, 0.8f, 0.f);
    tft.glVertex3f(-0.05f, 0.f, 0.f);
    tft.glVertex3f( 0.05f, 0.f, 0.f);
  tft.glEnd();
  //tft.updateScreen();
}
