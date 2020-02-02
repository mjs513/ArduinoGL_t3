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

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Arduino_OpenGL tft = Arduino_OpenGL(TFT_CS, TFT_DC, 8);
uint8_t use_fb = 1;

void drawLines() {
  /* 3 triangles instead of 4 */
  tft.glBegin(GL_LINES);
    tft.glColor3i(255,255,0);
    tft.glVertex3f(0, 1, 0);
    tft.glVertex3f(0, 0, 0);
  tft.glEnd();
}

void drawLineLoop() {
  /* 3 triangles instead of 4 */
  tft.glBegin(GL_LINE_LOOP);
    tft.glColorT(0, 255, 0, 0);
    tft.glColorT(1, 0, 255, 0);
    tft.glColorT(2, 0, 0, 255);
    tft.glVertex3f(0, 1, 0);
    tft.glVertex3f(0, 0, 0);
    tft.glVertex3f(1, 0, 0);
  tft.glEnd();
}

void drawLineStrip() {
  /* 3 triangles instead of 4 */
  tft.glBegin(GL_LINE_STRIP);
    tft.glColorT(0, 255, 0, 0);
    tft.glColorT(1, 0, 255, 0);
    tft.glColorT(2, 0, 0, 255);
    tft.glVertex3f(0, 1, 0);
    tft.glVertex3f(0, 0, 0);
    tft.glVertex3f(1, 0, 0);
  tft.glEnd();
}

void drawPyramid() {
  /* 3 triangles instead of 4 */
  tft.glBegin(GL_TRIANGLE_STRIP);
    tft.glColorT(0, 255, 0, 0);
    tft.glColorT(1, 0, 255, 0);
    tft.glColorT(2, 0, 0, 255);
    tft.glColorT(2, 255, 0, 255);
    
    tft.glVertex3f(-1, 0, 0);
    tft.glVertex3f(-1, 1, 0);
    tft.glVertex3f(0, 0, 0);
    tft.glVertex3f(0, 1, 0);
  tft.glEnd();
}

void drawPoly(){
  
  tft.glBegin(GL_POLYGON);
    tft.glColor3i(255,0,0);
    tft.glVertex3f(0.f, 0.f, 0.f);
    tft.glVertex3f(0.f, 1.f, 0.f);
    tft.glVertex3f(1.f, 1.f, 0.f);
    tft.glVertex3f(1.f, 0.f, 0.f);
  tft.glEnd();

}

void setup() {
  tft.init(240,320);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);
  tft.println("Waiting for Arduino Serial Monitor...");

  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("OpenGL Test!"); 

  delay(1000);

  /* Pass the canvas to the OpenGL environment */
  tft.glClear(ST7735_BLACK);
  tft.glPointSize(6);
    
  tft.glMatrixMode(GL_PROJECTION);
  tft.glLoadIdentity();
  tft.gluOrtho2D(-5.0, 5.0, -6.0, 6.0);
  tft.glMatrixMode(GL_MODELVIEW);
  //tft.glAttachShader(SimpleVertexShader);  //only shader available.
                                           //only use with triange_strip
  
  tft.useFrameBuffer(1);

}

void loop() {
  /* Read the rotation angle from a potentiometer attached to pin A0 */
  //float angle = 0;
  const float scale = 2.5;

  tft.glClear(ST7735_BLACK); 
  
  tft.glLoadIdentity();
      
  tft.glRotatef(-30.f, 0.f, 0.f, 1.f);
  tft.glScalef(scale, scale, scale);
  //drawPyramid();
  //drawPoly();
  //drawLines();
  //drawLineStrip();
  drawLineLoop();
  tft.updateScreen();
  
  delay(1);
}
