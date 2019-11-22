/*
    ArduinoGL.h - OpenGL subset for Arduino.
    Created by Fabio de Albuquerque Dela Antonio
    fabio914 at gmail.com
 */

#ifndef ArduinoGL_h
#define ArduinoGL_h

//#define ILI9488 1
#define ILI9341 1

#include "Arduino.h"
#if defined ILI9488
#include "ILI9488_t3.h"
#include <SPI.h>
#elif defined(ILI9341)
#include "ILI9341_t3n.h"
#include <SPI.h>
#include <SPIN.h>
#endif


typedef enum {
    GL_NONE = 0,
    GL_POINTS,
    GL_POLYGON,
    GL_TRIANGLE_STRIP
} GLDrawMode;

typedef enum {
    GL_PROJECTION = 0,
    GL_MODELVIEW
} GLMatrixMode;

/* Masks */
#define GL_COLOR_BUFFER_BIT 0x1

typedef struct {
    float x, y, z, w;
} GLVertex;

#define MAX_VERTICES 24
#define MAX_MATRICES 8

#define DEG2RAD (3.15159/180.0)

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
		uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12, 
		SPINClass *pspin=(SPINClass*)(&SPIN));
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
	void glBegin(GLDrawMode mode);
	void glEnd(void);

	//Arduino openGL
	GLDrawMode glDrawMode = GL_NONE;

	GLVertex glVertices[MAX_VERTICES];
	unsigned glVerticesCount = 0;

	GLMatrixMode glmatrixMode = GL_PROJECTION;
	float glMatrices[2][16];
	float glMatrixStack[MAX_MATRICES][16];
	unsigned glMatrixStackTop = 0;
	unsigned glPointLength = 1;

};

#endif
#endif