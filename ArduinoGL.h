/*
    ArduinoGL.h - OpenGL subset for Arduino.
    Created by Fabio de Albuquerque Dela Antonio
    fabio914 at gmail.com
 */

#ifndef ArduinoGL_h
#define ArduinoGL_h

#include "Arduino.h"
#include "SPI.h"
#if __has_include(<RA8875.h>)
	#include "RA8875.h"
	#define ra8875 1
#elif __has_include(<ILI9488_t3.h>)
	#include "ILI9488_t3.h"
	#define ILI9488 1
#elif __has_include(<ILI9341_t3n.h>)
	#include "ILI9341_t3n.h"
	#define ILI9341 1
#elif __has_include(<ILI9341_t3.h>)
	#include "ILI9341_t3.h"
#elif __has_include(<ST7789_t3.h>) 
	#include "ST7789_t3.h"
	#define ST7789 1
#elif __has_include(<ST7735_t3.h>)
	#include "ST7735_t3.h"
	#define ST7735 1
#else
	error "no device selected"
#endif


typedef enum {
    GL_NONE = 0,
    GL_POINTS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
    GL_QUAD,
	GL_POLYGON,
	GL_TRIANGLES,
    GL_TRIANGLE_STRIP
} GLDrawMode;

typedef enum {
    GL_PROJECTION = 0,
    GL_MODELVIEW
} GLMatrixMode;

typedef enum {
	NONE = 0,
    SimpleVertexShader,
	FacetShader
} GLShader;

/* Masks */
#define GL_COLOR_BUFFER_BIT 0x1

typedef struct {
    float x, y, z, w;   // vertex points
	float nx, ny, nz;	// vertex normals per face vertex count - 2
} GLVertex;


#define MAX_VERTICES 240
#define MAX_MATRICES 80

#define DEG2RAD (M_PI/180.0)
//#define readUnsignedByte(t)     ((uint16_t)pgm_read_byte(&(t)))

#ifdef __cplusplus

#if defined(ILI9488)
class Arduino_OpenGL : public ILI9488_t3 
{
public:
  Arduino_OpenGL(SPIClass *SPIWire, uint8_t _CS, uint8_t _DC, uint8_t _RST = 255, uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12);
#elif defined(ILI9341)
class Arduino_OpenGL : public ILI9341_t3n 
{
public:
  Arduino_OpenGL(uint8_t _CS, uint8_t _DC, uint8_t _RST = 255, 
		uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12);
#elif defined(ra8875)
class Arduino_OpenGL : public  RA8875
{
public:
  Arduino_OpenGL(const uint8_t CSp,const uint8_t RSTp=255,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);
#elif defined(ST7735)
class Arduino_OpenGL : public ST7735_t3
{
public:
  Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST = -1);
  Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t RST = -1);
#elif defined(ST7789)
class Arduino_OpenGL : public ST7789_t3
{
public:
  Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST = -1);
  Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t RST = -1);
#endif

	void copyMatrix(float * dest, float * src);
	void multMatrix(float * dest, float * src1, float * src2);
	void pushMatrix(float * m);
	void popMatrix(void);
	void normVector3(float * dest, float * src);
	void crossVector3(float * dest, float * src1, float * src2);

	/* Matrices */
	void glMatrixMode(GLMatrixMode mode);
	void glMultMatrixf(float * m);
	void glLoadMatrixf(float * m);
	void glLoadIdentity(void);

	void glPushMatrix(void);
	void glPopMatrix(void);

	void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
	void gluOrtho2D(float left, float right, float bottom, float top);
	void glFrustum(float left, float right, float bottom, float top, float zNear, float zFar);
	void gluPerspective(float fovy, float aspect, float zNear, float zFar);

	void glRotatef(float angle, float x, float y, float z);
	void glTranslatef(float x, float y, float z);
	void glScalef(float x, float y, float z);
	void gluLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);

	/* Vertices */
	void glVertex4fv(float * v);
	void glVertex4f(float x, float y, float z, float w);
	void glVertex3fv(float * v);
	void glVertex3f(float x, float y, float z);

	/* OpenGL */
	//void glUseCanvas(Canvas * c); /* <-- Arduino only */

	void glPointSize(unsigned size);
	void glClear(uint16_t color);
	void glAttachShader(GLShader shader);
	void glBegin(GLDrawMode mode);
	void glEnd(void);

	//Arduino openGL
	GLDrawMode glDrawMode = GL_NONE;
	GLShader glShader = NONE;
	GLVertex glVertices[MAX_VERTICES];
	unsigned glVerticesCount = 0;

	GLMatrixMode glmatrixMode = GL_PROJECTION;
	float glMatrices[2][16];
	float glMatrixStack[MAX_MATRICES][16];
	unsigned glMatrixStackTop = 0;
	unsigned glPointLength = 1;
	
	void glColor3ub(uint8_t r, uint8_t g, uint8_t b );
	uint16_t glColor = 0xFFFF;


	void glColorT(uint8_t idx, uint8_t r, uint8_t g, uint8_t b );
	uint16_t glColor_T[240] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF};
	void glColorQ(uint8_t idx, uint8_t r, uint8_t g, uint8_t b );
	uint16_t glColor_Q[4] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF};
	
	int16_t thick = 1;
	int16_t offset_thick[11] = {0, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5};
	int NFACES = 0;
	
  /* Basic triangle shader functions. 
   * Extracted and slightly modified from Michael Rule's
   * Uno9341TFT library, Optomized graphics drivers and demos for 
   * Arduino Uno for the 320x240 TFT LCD screens 
   * with the ILI9341 driver, https://github.com/michaelerule/Uno9341TFT
   *   
   * Interpolated color triangles are drawn similarly to regular triangles
   * In that they are broken into horizontal scanlines. For this we need
   * a fast horizontal interpolated line function. Additionally, we 
   * optionally support overdraw, which prevents pixels from the current
   * frame from being over-drawn. This requires skipping some segments
   * of the triangle.
   */
  uint8_t color_map[16];

  void     interpolateFlood(int16_t x, int16_t y, int16_t i, uint16_t stop, int16_t length, uint16_t color1, uint16_t color2);
  void     interpolateFastHLine(int16_t x, int16_t y0, uint16_t w, uint16_t color1, uint16_t color2);
  void     shadeTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color0, uint16_t color1, uint16_t color2);
  uint16_t interpolate(int16_t color1, int16_t color2, int16_t alpha);
  void     setColorMap(uint8_t cmap);
  
  //Shading
  void vertex_normalize();
  
private:
	uint8_t color_default, color_default_array;
	uint8_t _r, _g, _b;
};

#endif
#endif