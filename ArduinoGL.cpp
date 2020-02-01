/*
    ArduinoGL.h - OpenGL subset for Arduino.
    Created by Fabio de Albuquerque Dela Antonio
    fabio914 at gmail.com
 */

#include "ArduinoGL.h"
#include <SPI.h>
#include <math.h>
#include <stdint.h>
#include "color_maps.h"

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

#endif

#if defined(ILI9488)
Arduino_OpenGL::Arduino_OpenGL(SPIClass *SPIWire, uint8_t _CS, uint8_t _DC, uint8_t _RST, uint8_t _MOSI, uint8_t _SCLK, uint8_t _MISO) : ILI9488_t3(SPIWire, _CS, _DC, _RST, _MOSI, _SCLK, _MISO) 
{
}
#elif defined(ILI9341)
Arduino_OpenGL::Arduino_OpenGL(uint8_t _CS, uint8_t _DC, uint8_t _RST, uint8_t _MOSI, uint8_t _SCLK, uint8_t _MISO, SPINClass *pspin) : ILI9341_t3n(  _CS, _DC, _RST, _MOSI, _SCLK, _MISO) 
{
}
#elif defined(ra8875)
Arduino_OpenGL::Arduino_OpenGL(const uint8_t CSp,const uint8_t RSTp,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin) : RA8875( CSp, RSTp, mosi_pin, sclk_pin, miso_pin) 
{
}
#elif defined(ST7735)
  Arduino_OpenGL::Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST) : ST7735_t3( CS,  RS,  SID,  SCLK,  RST)
  {
	  setColorMap(11);
  }
  Arduino_OpenGL::Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t RST) :   ST7735_t3( CS,  RS,  RST)
  {
	  setColorMap(11);
  }
#elif defined(ST7789)
  Arduino_OpenGL::Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST) : ST7789_t3( CS,  RS,  SID,  SCLK,  RST)
  {
  }
  Arduino_OpenGL::Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t RST) : ST7789_t3( CS,  RS,  RST)
  {
  }
#endif

/* Aux functions */
void Arduino_OpenGL::copyMatrix(float * dest, float * src) {
    
    for(int i = 0; i < 16; i++)
        dest[i] = src[i];
}

void Arduino_OpenGL::multMatrix(float * dest, float * src1, float * src2) {
    
    int i, j, k;
    float m[16];
    
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++) {
            
            m[i + j * 4] = 0.0;
            
            for(k = 0; k < 4; k++)
                m[i + j * 4] += src1[i + k * 4] * src2[k + j * 4];
        }
    
    for(i = 0; i < 16; i++)
        dest[i] = m[i];
}

void Arduino_OpenGL::pushMatrix(float * m) {
    
    if(glMatrixStackTop < MAX_MATRICES) {
        
        copyMatrix(glMatrixStack[glMatrixStackTop], m);
        glMatrixStackTop++;
    }
}

void Arduino_OpenGL::popMatrix(void) {
    
    if(glMatrixStackTop > 0) {
        
        glMatrixStackTop--;
    }
}

GLVertex multVertex(float * m, GLVertex v) {
    
    GLVertex ret;
    
    ret.x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w;
    ret.y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w;
    ret.z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w;
    ret.w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w;
    
    return ret;
}

void Arduino_OpenGL::normVector3(float * dest, float * src) {
    
    float norm;
    register int i;
    
    norm = sqrt(src[0] * src[0] + src[1] * src[1] + src[2] * src[2]);
    
    for(i = 0; i < 3; i++)
        dest[i] = src[i]/norm;
}

void Arduino_OpenGL::crossVector3(float * dest, float * src1, float * src2) {
    
    float ret[3];
    register int i;
    
    ret[0] = src1[1] * src2[2] - src1[2] * src2[1];
    ret[1] = src1[2] * src2[0] - src1[0] * src2[2];
    ret[2] = src1[0] * src2[1] - src1[1] * src2[0];
    
    for(i = 0; i < 3; i++)
        dest[i] = ret[i];
}

/* Matrices */

void Arduino_OpenGL::glMatrixMode(GLMatrixMode mode) {
    if(mode == GL_MODELVIEW || mode == GL_PROJECTION)
        glmatrixMode = mode;
}

void Arduino_OpenGL::glMultMatrixf(float * m) {
    multMatrix(glMatrices[glmatrixMode], glMatrices[glmatrixMode], m);
}

void Arduino_OpenGL::glLoadMatrixf(float * m) {
    copyMatrix(glMatrices[glmatrixMode], m);
}

void Arduino_OpenGL::glLoadIdentity(void) {
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        if(i % 5 == 0)
            m[i] = 1.0;
        else m[i] = 0.0;
    
    Arduino_OpenGL::glLoadMatrixf(m);
}

void Arduino_OpenGL::glPushMatrix(void) {
    pushMatrix(glMatrices[glmatrixMode]);
}

void Arduino_OpenGL::glPopMatrix(void) {
    popMatrix();
}

void Arduino_OpenGL::glOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
    
    float tx = -(right + left)/(right - left);
    float ty = -(top + bottom)/(top - bottom);
    float tz = -(zFar + zNear)/(zFar - zNear);
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = 2.0/(right - left);
    m[5] = 2.0/(top - bottom);
    m[10] = -2.0/(zFar - zNear);
    m[12] = tx;
    m[13] = ty;
    m[14] = tz;
    m[15] = 1.0;
    
    glMultMatrixf(m);
}

void Arduino_OpenGL::gluOrtho2D(float left, float right, float bottom, float top) {
    glOrtho(left, right, bottom, top, -1.0, 1.0);
}

void Arduino_OpenGL::glFrustum(float left, float right, float bottom, float top, float zNear, float zFar) {
    
    float A = (right + left)/(right - left);
    float B = (top + bottom)/(top - bottom);
    float C = -(zFar + zNear)/(zFar - zNear);
    float D = -(2.0 * zFar * zNear)/(zFar - zNear);
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = (2.0 * zNear)/(right - left);
    m[5] = (2.0 * zNear)/(top - bottom);
    m[8] = A;
    m[9] = B;
    m[10] = C;
    m[11] = -1.0;
    m[14] = D;
    
    glMultMatrixf(m);
}

void Arduino_OpenGL::gluPerspective(float fovy, float aspect, float zNear, float zFar) {
    
    float aux = tan((fovy/2.0) * DEG2RAD);
    float top = zNear * aux;
    float bottom = -top;
    float right = zNear * aspect * aux;
    float left = -right;
    
    glFrustum(left, right, bottom, top, zNear, zFar);
}

void Arduino_OpenGL::glRotatef(float angle, float x, float y, float z) {
    
    float c = cos(DEG2RAD * angle), s = sin(DEG2RAD * angle);
    float nx, ny, nz, norm;
    
    norm = sqrt(x*x + y*y + z*z);
    
    if(norm == 0)
        return;
    
    nx = x/norm;
    ny = y/norm;
    nz = z/norm;
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = nx*nx*(1.0 - c) + c;
    m[1] = ny*nx*(1.0 - c) + nz*s;
    m[2] = nx*nz*(1.0 - c) - ny*s;
    m[4] = nx*ny*(1.0 - c) - nz*s;
    m[5] = ny*ny*(1.0 - c) + c;
    m[6] = ny*nz*(1.0 - c) + nx*s;
    m[8] = nx*nz*(1.0 - c) + ny*s;
    m[9] = ny*nz*(1.0 - c) - nx*s;
    m[10] = nz*nz*(1.0 - c) + c;
    m[15] = 1.0;
    
    glMultMatrixf(m);
}

void Arduino_OpenGL::glTranslatef(float x, float y, float z) {
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = 1.0;
    m[5] = 1.0;
    m[10] = 1.0;
    m[12] = x;
    m[13] = y;
    m[14] = z;
    m[15] = 1.0;
    
    glMultMatrixf(m);
}

void Arduino_OpenGL::glScalef(float x, float y, float z) {
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = x;
    m[5] = y;
    m[10] = z;
    m[15] = 1.0;
    
    glMultMatrixf(m);
}

void Arduino_OpenGL::gluLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
    
    float dir[3], up[3];
    
    dir[0] = centerX - eyeX;
    dir[1] = centerY - eyeY;
    dir[2] = centerZ - eyeZ;
    
    up[0] = upX;
    up[1] = upY;
    up[2] = upZ;
    
    float n[3], u[3], v[3];
    
    normVector3(n, dir);
    
    crossVector3(u, n, up);
    normVector3(u, u);
    
    crossVector3(v, u, n);
    
    float m[16];
    int i;
    
    for(i = 0; i < 16; i++)
        m[i] = 0.0;
    
    m[0] = u[0];
    m[1] = v[0];
    m[2] = -n[0];
    
    m[4] = u[1];
    m[5] = v[1];
    m[6] = -n[1];
    
    m[8] = u[2];
    m[9] = v[2];
    m[10] = -n[2];
    
    m[15] = 1.0;
    
    glMultMatrixf(m);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}

/* Vertices */

void Arduino_OpenGL::glVertex4fv(float * v) {
    glVertex4f(v[0], v[1], v[2], v[3]);
}

void Arduino_OpenGL::glVertex4f(float x, float y, float z, float w) {
    
    GLVertex v;
    
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    
    if(glVerticesCount < MAX_VERTICES) {
        
        glVertices[glVerticesCount] = v;
        glVerticesCount++;
    }
}

void Arduino_OpenGL::glVertex3fv(float * v) {
    glVertex3f(v[0], v[1], v[2]);
}

void Arduino_OpenGL::glVertex3f(float x, float y, float z) {
    glVertex4f(x, y, z, 1.0);
}



/* OpenGL */

//void glUseCanvas(Canvas * c) {
    //glCanvas = c;
//}

void Arduino_OpenGL::glClear(uint16_t color) {
	#if defined(ra8875)
		clearScreen(color);
	#else
		fillScreen(color);
	#endif
}

void Arduino_OpenGL::glPointSize(unsigned size) {
    glPointLength = size;
}

void Arduino_OpenGL::glAttachShader(GLShader shader){
	glShader = shader;
}

void Arduino_OpenGL::glBegin(GLDrawMode mode) {
    glDrawMode = mode;
    glVerticesCount = 0;
	glColor = 0xFFFF;
	for(uint8_t i=0; i<24; i++) {glColor_T[i] = 0xFFFF; }
	for(uint8_t i=0; i<4; i++) {glColor_Q[i] = 0xFFFF; }
	color_default_array = 1;
	color_default = 1;
}

void Arduino_OpenGL::glEnd(void) {
    
    if(glDrawMode == GL_NONE)
        return;
    
    float modelviewProjection[16];
    multMatrix(modelviewProjection, glMatrices[GL_PROJECTION], glMatrices[GL_MODELVIEW]);
    
    int frameWidth = _width;
    int frameHeight = _height;
    
    for(uint16_t i = 0; i < glVerticesCount; i++) {

        GLVertex aux = multVertex(modelviewProjection, glVertices[i]);
        
        aux.x = aux.x/aux.w;
        aux.y = aux.y/aux.w;
        aux.z = aux.z/aux.w;
        
        glVertices[i] = aux;
    }
    
    if(glDrawMode == GL_POINTS) {
        
        for(uint16_t i = 0; i < glVerticesCount; i++) {
            
            GLVertex * aux = &(glVertices[i]);
            
            if(!(glVertices[i].x >= -1.0 && glVertices[i].x <= 1.0))
                continue;
            
            if(!(glVertices[i].y >= -1.0 && glVertices[i].y <= 1.0))
                continue;
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                continue;
            
            int px = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            int py = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
            
            for(int x = (px - glPointLength/2.0); x <= (px + glPointLength/2.0); x++)
                for(int y = (py - glPointLength/2.0); y <= (py + glPointLength/2.0); y++)
                    drawPixel(x, y, glColor);  //White for now
        }
		glColor3i(255,255,255);
    }

    else if(glDrawMode == GL_LINES) {
		
        if(glVerticesCount < 2)
            return;
        
        int px[2], py[2];
            
        for(uint16_t i = 0; i < 2; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
			
			px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
		
		int dx = px[1] - px[0];
		int dy = py[1] - py[0];
		for(uint8_t thick = 0; thick < glPointLength; thick++){
			if(dx == 0){
				drawLine(px[0]+thick, py[0], px[1]+thick, py[1], glColor);
			}
			else if(dy == 0) {
				drawLine(px[0], py[0]+thick, px[1], py[1]+thick, glColor);
			}
			else {
				drawLine(px[0]+thick, py[0]+2*thick, px[1]+thick, py[1]+2*thick, glColor);
			}
		}
	}
	
    else if(glDrawMode == GL_LINE_STRIP) {
		
        if(glVerticesCount < 2)
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
            
        for(uint16_t i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
			
			px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
		
		for(uint16_t i = 0; i < glVerticesCount-1; i++) {
			int dx = px[i+1] - px[i];
			int dy = py[i+1] - py[i];
			for(uint8_t thick = 0; thick < glPointLength; thick++){
				if(dx == 0){
					drawLine(px[i]+thick, py[i], px[i+1]+thick, py[i+1], glColor_T[i]);
				}
				else if(dy == 0) {
					drawLine(px[i], py[i]+thick, px[i+1], py[i+1]+thick, glColor_T[i]);
				}
				else {
					drawLine(px[i]+thick, py[i]+2*thick, px[i+1]+thick, py[i+1]+2*thick, glColor_T[i]);
				}
			}
		}
	}
	
    else if(glDrawMode == GL_LINE_LOOP) {
		
        if(glVerticesCount < 2)
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
            
        for(uint16_t i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
			
			px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
		
		for(uint16_t i = 0; i < glVerticesCount-1; i++) {
			int dx = px[i+1] - px[i];
			int dy = py[i+1] - py[i];
			for(uint8_t thick = 0; thick < glPointLength; thick++){
				if(dx == 0){
					drawLine(px[i]+thick, py[i], px[i+1]+thick, py[i+1], glColor_T[i]);
				}
				else if(dy == 0) {
					drawLine(px[i], py[i]+thick, px[i+1], py[i+1]+thick, glColor_T[i]);
				}
				else {
					drawLine(px[i]+thick, py[i]+2*thick, px[i+1]+thick, py[i+1]+2*thick, glColor_T[i]);
				}
			}
		}
		
		float dx = px[glVerticesCount-1] - px[0];
		float dy = py[glVerticesCount-1] - py[0];
		for(uint8_t thick = 0; thick < glPointLength; thick++){
			if(dx == 0){
				drawLine(px[glVerticesCount-1]+thick, py[glVerticesCount-1], px[0]+thick, py[0], glColor_T[glVerticesCount-1]);
			}
			else if(dy == 0) {
				drawLine(px[glVerticesCount-1], py[glVerticesCount-1]+thick, px[0], py[0]+thick, glColor_T[glVerticesCount-1]);
			}
			else {
				drawLine(px[glVerticesCount-1]+thick, py[glVerticesCount-1]+2*thick, px[0]+thick, py[0]+2*thick, glColor_T[glVerticesCount-1]);
			}
		}
		
	}
	
    else if(glDrawMode == GL_QUADS) {

        /* TODO Improve! */
        if(glVerticesCount < 4)
            return;
        
        int px[4], py[4];
            
        for(uint8_t i = 0; i < 4; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
        
		uint8_t idxQ = 0;
        for(uint16_t i = 0; i < 4; i++) {
            uint16_t next = (i + 1 == glVerticesCount) ? 0:(i + 1);
            drawLine(px[i], py[i], px[next], py[next], glColor_Q[idxQ]);
			idxQ += 1;
			if(next == 0) idxQ = 0;
        }
		
    }
	
    else if(glDrawMode == GL_POLYGON) {

        /* TODO Improve! */
        if(glVerticesCount < 2)
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
            
        for(uint16_t i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
        
		uint8_t idxQ = 0;
        for(uint16_t i = 0; i < glVerticesCount; i++) {
            uint16_t next = (i + 1 == glVerticesCount) ? 0:(i + 1);
            drawLine(px[i], py[i], px[next], py[next], glColor_Q[idxQ]);
			idxQ += 1;
			if(next == 0) idxQ = 0;
        }
		
	}
    
    else if(glDrawMode == GL_TRIANGLE_STRIP) {
        
        /* TODO Improve! */
        if(glVerticesCount < 3)
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
        
        for(uint16_t i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }

		if(color_default_array != 1){
			for(uint16_t i = 0; i < glVerticesCount - 2; i++) {
				drawLine(px[i], py[i], px[i + 1], py[i + 1], glColor_T[i]);
				drawLine(px[i], py[i], px[i + 2], py[i + 2], glColor_T[i+1]);
				drawLine(px[i + 1], py[i + 1], px[i + 2], py[i + 2], glColor_T[i+2]);
			}
		} else {

			for(uint16_t i = 0; i < glVerticesCount - 2; i++) {
				fillTriangle ( px[i], py[i],
						  px[i + 1], py[i + 1],
						  px[i + 2], py[i + 2], glColor);
			}
			glColor3i(255,255,255);
		}
 
		
		if(glShader == SimpleVertexShader) {
			for(uint16_t i = 0; i < glVerticesCount - 2; i++) {
				shadeTriangle(px[i], py[i], px[i+1], py[i+1], px[i+2], py[i+2], glColor_T[i], glColor_T[i+1], glColor_T[i+2]);
			}
		}
    }
}

void Arduino_OpenGL::glColor3i(uint8_t r, uint8_t g, uint8_t b ) {  //not really openGL format
	glColor = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	color_default = 0;
}

void Arduino_OpenGL::glColorT(uint8_t idx, uint8_t r, uint8_t g, uint8_t b ) {  //not really openGL format
	glColor_T[idx] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	color_default_array = 0;
}

void Arduino_OpenGL::glColorQ(uint8_t idx, uint8_t r, uint8_t g, uint8_t b ) {  //not really openGL format
	glColor_Q[idx] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


////////////////////////////////////////////////////////////////////////
// Triangle shaders
////////////////////////////////////////////////////////////////////////


/*  Computes convex combination of color1 and color2 with weight
 *  Weight is normalizes s.t. [0,1] maps to [0,256]
 */
uint16_t Arduino_OpenGL::interpolate(int16_t color1, int16_t color2, int16_t alpha) {
  // The clean way: break out the RGB components and reassemble
  return color1 * alpha + (color2 * (32-alpha)) >> 5;
}


// Horizontal fill of a segment with color interpolation
void Arduino_OpenGL::interpolateFlood(int16_t x, int16_t y, int16_t i, uint16_t stop, int16_t length, uint16_t color1, uint16_t color2)
{
    uint8_t alpha = 0xff;
	
    while (i<stop) {
        uint8_t weight = i*32/length;
        if (weight!=alpha) {
            alpha = weight;
            uint16_t c = interpolate(color1, color2, alpha);
			drawPixel(i+x, y ,c);
        }
        i++;
    }
}

// Fast horizontal line supporting overdraw and interpolation
// Does not support masking
void Arduino_OpenGL::interpolateFastHLine(int16_t x, int16_t y, uint16_t length, uint16_t color1, uint16_t color2) {
    if (length<1) return;
    interpolateFlood(x, y, 0, length,length,color1,color2);
	return;

}

// Shade a triangle with three colors
// Supports optional overdraw rendering
// Does not support masking -- masking is typically used to erases so
// will not be needed here.
void Arduino_OpenGL::shadeTriangle ( 
          int16_t x0, int16_t y0,
		  int16_t x1, int16_t y1,
		  int16_t x2, int16_t y2, 
          uint16_t color0, 
          uint16_t color1, 
          uint16_t color2) 
{
    int16_t a, b, y, last;
	
    if (y0 > y1) { swap(y0, y1); swap(x0, x1); swap(color0,color1);}
    if (y1 > y2) { swap(y2, y1); swap(x2, x1); swap(color2,color1);}
    if (y0 > y1) { swap(y0, y1); swap(x0, x1); swap(color0,color1);}

	  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if(x1 < a)      a = x1;
		else if(x1 > b) b = x1;
		if(x2 < a)      a = x2;
		else if(x2 > b) b = x2;
		drawFastHLine(a, y0, b-a+1, color0);
		return;
	  }
	
    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;
	
  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
	
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip the y1 scanline
	
    //int len1 = y2 - y0;
    int len2 = y1 - y0;
    int len3 = y2 - y1;
    //int round2 = len2/2;
    //int round3 = len3/2;
    uint16_t midpoint = interpolate(color2,color0,(y1-y0)*32./(y2-y0));
    uint16_t colorX = midpoint;
    uint16_t colorY = color1;
    uint16_t colorZ = color0;

    // Skip the first line to avoid triangle overlap in meshes
    sa += dx01;
    sb += dx02;
    for(y=y0+1; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        float weight = ((y-y0)*32.)/len2;
        uint16_t colorA = interpolate(colorX,colorZ,weight);
        uint16_t colorB = interpolate(colorY,colorZ,weight);
        if(a > b) {
            swap(a,b);
            swap(colorA,colorB);
        }
        interpolateFastHLine(a, y, b-a+1, colorA, colorB);
    }
	
    uint16_t colorU = midpoint;
    uint16_t colorV = color1;
    uint16_t colorW = color2;
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        float weight = 32.-((y-y1)*32.)/len3;
        uint16_t colorA = interpolate(colorU,colorW,weight);
        uint16_t colorB = interpolate(colorV,colorW,weight);
        if(a > b) {
            swap(a,b);
            swap(colorA,colorB);
        }
        interpolateFastHLine(a, y, b-a+1, colorA, colorB);
    }
}

/*
void Arduino_OpenGL::setColorMap(uint8_t cmap) {
    const uint16_t  * map;
    switch (cmap) {
        case 0: map = &reddish[0]; break;
        case 1: 
        case 2: map = &pinkish[0]; break;
        case 3:
        case 4: map = &blueish[0]; break;
        case 5:
        case 6: map = &greenish[0]; break;
        case 7:
        case 8: map = &brownish[0]; break;
        case 9:
        case 10:
        case 11:
        default:
            map = &greyish[0];
    }
    for (uint8_t i=0; i<1; i++) color_map[i] = readUnsignedByte(map[i]);
}
*/