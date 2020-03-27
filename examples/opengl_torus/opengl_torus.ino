#include "SPI.h"
//#include "ST7789_t3.h"
#include "ILI9341_t3n.h"
#include <ArduinoGL.h> 
#include "T4_PowerButton.h"
//#include "teapot_high_res.h"
//#include "teapot_low_res.h"
#include "monkey_high_res.h"
//#include "small_bunny.h"

#define _use_triangle 0
#define _use_quad1 0
#define _use_pyramid 0
#define _use_torus 0
#define _use_cube 0
#define _use_model 1


#define _use_FacetShader  1
#define _use_SimpleShader  0
#define _use_perspective 0


// For the Adafruit shield, these are the default.
#define TFT_DC  9
#define TFT_CS  10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Arduino_OpenGL tft = Arduino_OpenGL(TFT_CS, TFT_DC, 8);
uint8_t use_fb = 1;

#define PI_ 3.14159265358979323846
int numc = 8;
int numt = 25;


void setup() {
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.println("Waiting for Arduino Serial Monitor...");
  //tft.setRotation(1);
  
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("OpenGL Test!"); 
  flexRamInfo();

  delay(1000);

  /* Pass the canvas to the OpenGL environment */
  tft.glClear(ILI9341_BLACK);
  tft.glPointSize(2);
    
  tft.glMatrixMode(GL_PROJECTION);
  tft.glLoadIdentity();

    float ar = (float)tft.width() / (float) tft.height();

  if (_use_perspective == 1) {
      tft.gluPerspective(65.0, ar, 0.1f, 9999.f);
   } else {
      tft.glOrtho(-5.0, 5.0, -6.0, 6.0, -5, 5);
  }
  
  tft.glMatrixMode(GL_MODELVIEW);

  if(_use_FacetShader == 1){
    tft.glAttachShader(FacetShader);
  } else if(_use_SimpleShader == 1){
    tft.glAttachShader(SimpleVertexShader);
  }
  
  tft.useFrameBuffer(1);
}

float angle = -45.;
float angley = -45.;

void loop() {
  tft.glClear(ILI9341_BLACK); 
  tft.glLoadIdentity();

  if(_use_perspective == 1)
      tft.gluLookAt(0, 0, -15, 0, 3, 0, 0, 1, 0);
  
  if(_use_model == 1){
    //tft.glTranslatef(0.0, 3., 0.0);
    tft.glRotatef(angle, angley, angle, 1.f);  //note to self -75 for torus
    //tft.glScalef(0.05, 0.05, 0.05);
    tft.glScalef(0.03, 0.03, 0.03);
  } else {
    tft.glRotatef(angle, angley, angle, 1.f);  //note to self -75 for torus
    tft.glScalef(2.5, 2.5, 2.5);  //was 2.5
  }
  
  if(_use_triangle == 1) drawTri();
  if(_use_quad1 == 1) drawQuad2();
  if(_use_torus == 1) torus();
  if(_use_pyramid == 1) drawPyramid();
  if(_use_model == 1) drawFromModel();
  if(_use_cube == 1)drawCube();

  tft.setCursor(0,0);
  //tft.println(angle);
  //Serial.println(angle);
  tft.print(angle);
  tft.updateScreen();

  angle -= 10;
  if(abs(angle) > 350) angle = 0;

  delay(500);
  //Serial.print("Temp: ");
  //Serial.println(tempmonGetTemp());
}
