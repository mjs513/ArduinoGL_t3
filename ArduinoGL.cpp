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
Arduino_OpenGL::Arduino_OpenGL(uint8_t _CS, uint8_t _DC, uint8_t _RST, uint8_t _MOSI, uint8_t _SCLK, uint8_t _MISO) : ILI9341_t3n(  _CS, _DC, _RST, _MOSI, _SCLK, _MISO) 
{
}
#elif defined(ra8875)
Arduino_OpenGL::Arduino_OpenGL(const uint8_t CSp,const uint8_t RSTp,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin) : RA8875( CSp, RSTp, mosi_pin, sclk_pin, miso_pin) 
{
}
#elif defined(ST7735)
  Arduino_OpenGL::Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST) : ST7735_t3( CS,  RS,  SID,  SCLK,  RST)
  {
  }
  Arduino_OpenGL::Arduino_OpenGL(uint8_t CS, uint8_t RS, uint8_t RST) :   ST7735_t3( CS,  RS,  RST)
  {
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

FLASHMEM
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

FLASHMEM
GLVertex multVertex(float * m, GLVertex v) {
    
    GLVertex ret;
    
    ret.x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w;
    ret.y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w;
    ret.z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w;
    ret.w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w;
    
    return ret;
}

FLASHMEM
void Arduino_OpenGL::normVector3(float * dest, float * src) {
    
    float norm;
    register int i;
    
    norm = sqrt(src[0] * src[0] + src[1] * src[1] + src[2] * src[2]);
    
    for(i = 0; i < 3; i++)
        dest[i] = src[i]/norm;
}

FLASHMEM
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
    if(mode == GL_MODELVIEW || mode == GL_PROJECTION){
        glmatrixMode = mode;
		gluLookAt(0,0,1,0,0,0,0,1,0) ;
	}
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

FLASHMEM
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

FLASHMEM
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

FLASHMEM
void Arduino_OpenGL::gluPerspective(float fovy, float aspect, float zNear, float zFar) {
    
    float aux = tan((fovy/2.0) * DEG2RAD);
    float top = zNear * aux;
    float bottom = -top;
    float right = zNear * aspect * aux;
    float left = -right;
    
    glFrustum(left, right, bottom, top, zNear, zFar);
}

FLASHMEM
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

FLASHMEM
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

FLASHMEM
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

FLASHMEM
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
	_eyeX = eyeX;  _eyeY = eyeY; _eyeZ = eyeZ;
}

/* Vertices */

void Arduino_OpenGL::glVertex4fv(float * v) {
    glVertex4f(v[0], v[1], v[2], v[3]);
}

FLASHMEM
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
	NFACES = 0;
	for (uint16_t i=0; i<MAX_VERTICES/3; i++) draw_order[i]=i;
}

FLASHMEM
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
		glColor3ub(255,255,255);
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
		for(uint8_t t = 0; t < glPointLength; t++){
			thick = offset_thick[t];
			if(dx == 0){
				drawLine(px[0]+thick, py[0], px[1]+thick, py[1], glColor);
			}
			else if(dy == 0) {
				drawLine(px[0], py[0]+thick, px[1], py[1]+thick, glColor);
			}
			else {
				if(dx < 0 && dy > 0) {
					drawLine(px[0]+thick, py[0]+thick, px[1]+thick, py[1]+thick, glColor);
				} else if(dx > 0 && dy < 0){
					drawLine(px[0]-thick, py[0]-thick, px[1]-thick, py[1]-thick, glColor);
				} else {
					drawLine(px[0]-thick, py[0]+thick, px[1]-thick, py[1]+thick, glColor);
				}
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
			for(uint8_t t = 0; t < glPointLength; t++){
				thick = offset_thick[t];
				if(dx == 0){
					drawLine(px[i]+thick, py[i], px[i+1]+thick, py[i+1], glColor_T[i]);
				}
				else if(dy == 0) {
					drawLine(px[i], py[i]+thick, px[i+1], py[i+1]+thick, glColor_T[i]);
				}
				else {
					if(dx < 0 && dy > 0) {
						drawLine(px[i]+thick, py[i]+thick, px[i+1]+thick, py[i+1]+thick, glColor_T[i]);
					} else if(dx > 0 && dy < 0){
						drawLine(px[i]-thick, py[i]-thick, px[i+1]-thick, py[i+1]-thick, glColor_T[i]);
					} else {
						drawLine(px[i]-thick, py[i]+thick, px[i+1]-thick, py[i+1]+thick, glColor_T[i]);
					}
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
			for(uint8_t t = 0; t < glPointLength; t++){
				thick = offset_thick[t];
				if(dx == 0){
					drawLine(px[i]+thick, py[i], px[i+1]+thick, py[i+1], glColor_T[i]);
				}
				else if(dy == 0) {
					drawLine(px[i], py[i]+thick, px[i+1], py[i+1]+thick, glColor_T[i]);
				}
				else {
					if(dx < 0 && dy > 0) {
						drawLine(px[i]+thick, py[i]+thick, px[i+1]+thick, py[i+1]+thick, glColor_T[i]);
					} else if(dx > 0 && dy < 0){
						drawLine(px[i]-thick, py[i]-thick, px[i+1]-thick, py[i+1]-thick, glColor_T[i]);
					} else {
						drawLine(px[i]-thick, py[i]+thick, px[i+1]-thick, py[i+1]+thick, glColor_T[i]);
					}
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
				if(dx < 0 && dy > 0) {
					drawLine(px[glVerticesCount-1]+thick, py[glVerticesCount-1]+thick, px[0]+thick, py[0]+thick, glColor_T[glVerticesCount-1]);
				} else if(dx > 0 && dy < 0){
					drawLine(px[glVerticesCount-1]-thick, py[glVerticesCount-1]-thick, px[0]-thick, py[0]-thick, glColor_T[glVerticesCount-1]);
				} else {
					drawLine(px[glVerticesCount-1]-thick, py[glVerticesCount-1]+thick, px[0]-thick, py[0]+thick, glColor_T[glVerticesCount-1]);
				}
			}
		}
		
	}
	
    else if(glDrawMode == GL_QUADS) {
      // 0---2---4
      // |   |   |
      // 1---3---5
	  
        /* TODO Improve! */
        if(glVerticesCount < 4 && (glVerticesCount % 4 == 0))
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
            
        for(uint8_t i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
        
		NFACES = glVerticesCount/4;
		
		//breaking the quad up into triangles allows for shading and fills
		for(uint16_t quad = 0; quad < (glVerticesCount / 4); quad++){
			uint16_t ptIdx = quad*4;
			uint16_t i = ptIdx;
			if(color_default_array != 1){
					drawLine(px[i], py[i], px[i + 1], py[i + 1], glColor_T[i]);
					drawLine(px[i], py[i], px[i + 2], py[i + 2], glColor_T[i+1]);
					drawLine(px[i + 1], py[i + 1], px[i + 2], py[i + 2], glColor_T[i+2]);
					
					drawLine(px[i+2], py[i+2], px[i + 3], py[i + 3], glColor_T[i]);
					drawLine(px[i+3], py[i+3], px[i], py[i], glColor_T[i+1]);
					drawLine(px[i], py[i], px[i + 2], py[i + 2], glColor_T[i+2]);
			} else {
					fillTriangle ( px[i], py[i],
							  px[i + 1], py[i + 1],
							  px[i + 2], py[i + 2], glColor);
					fillTriangle ( px[i+2], py[i+2],
							  px[i + 3], py[i + 3],
							  px[i], py[i], glColor);
			}
		}
		
		if(glShader == SimpleVertexShader) {
			updateDrawingOrder(NFACES, glVertices, draw_order);
			for(uint16_t quad = 0; quad < (glVerticesCount / 4); quad++){
				//uint16_t i = quad*4;
				uint16_t order = draw_order? draw_order[quad]:quad;  //face to draw
				uint16_t i = order*4;
				
				float p[3], q[3], r[3];
				// if triangle is facing the camera, draw it
				p[0] = glVertices[i].x;   p[1] = glVertices[i].y;   p[2] = glVertices[i].z;
				q[0] = glVertices[i+1].x; q[1] = glVertices[i+1].y; q[2] = glVertices[i+1].z;
				r[0] = glVertices[i+2].x; r[1] = glVertices[i+2].y; r[2] = glVertices[i+2].z;
			
				if(facing_camera(p,q,r) > 0){
					shadeTriangle(px[i], py[i], px[i+1], py[i+1], px[i+2], py[i+2], glColor_T[i], glColor_T[i+1], glColor_T[i+2]);
					shadeTriangle(px[i+2], py[i+2], px[i+3], py[i+3], px[i], py[i], glColor_T[i+2], glColor_T[i+3], glColor_T[i]);
				}
			}
		} 
		else if(glShader == FacetShader){
			//vertex normals and normalization
			updateDrawingOrder(NFACES, glVertices, draw_order);
			vertex_normalize(draw_order);
			
			float normColor;
			for(uint16_t quad = 0; quad < glVerticesCount / 4; quad++) {
				uint16_t order = draw_order? draw_order[quad]:quad;  //face to draw
				uint16_t i = order*4;
				normColor = (glVertices[i].nz < 0?0:glVertices[i].nz);
				
				if(normColor <0.001 && glVertices[i].nz != 0 ) normColor = abs(glVertices[i].nz);
				//normColor = abs(glVertices[i].nz);
				uint16_t color = (((uint8_t)(normColor* _r) & 0xF8) << 8) | (((uint8_t)(normColor* _g) & 0xFC) << 3) | ((uint8_t)((normColor* _b)) >> 3);
				/*
				Serial.printf("Indicies %d, %d, %d\n", i, i+1, i+2);
				Serial.printf("\t %d, %d, %d, %f, %x\n", i, px[i], py[i],
					glVertices[i].nz, normColor* _r);
				Serial.printf("\t %d, %d, %d, %f, %x\n", i+1, px[i+1], py[i+1],
					glVertices[i+1].nz, normColor* _g);
				Serial.printf("\t %d, %d, %d, %f, %x\n", i+2, px[i+2], py[i+2],
					glVertices[i+2].nz, normColor* _b);
				*/
				float p[3], q[3], r[3];
				// if triangle is facing the camera, draw it
				p[0] = glVertices[i].x;   p[1] = glVertices[i].y;   p[2] = glVertices[i].z;
				q[0] = glVertices[i+1].x; q[1] = glVertices[i+1].y; q[2] = glVertices[i+1].z;
				r[0] = glVertices[i+2].x; r[1] = glVertices[i+2].y; r[2] = glVertices[i+2].z;
			
				//Serial.println(facing_camera(p,q,r));
			if(facing_camera(p,q,r) > 0)
				//if (glVertices[i].nz >= 0)
					fillTriangle ( px[i], py[i],
							  px[i + 1], py[i + 1],
							  px[i + 2], py[i + 2], color);
					fillTriangle ( px[i+2], py[i+2],
							  px[i + 3], py[i + 3],
							  px[i], py[i], color);
			}
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
    
    else if(glDrawMode == GL_TRIANGLES) {

		/* TODO Improve! */
        if(glVerticesCount < 3)
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
		
		NFACES = glVerticesCount/3;

        for(uint16_t i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }

 
		if(glShader == SimpleVertexShader) {
			for(uint16_t i = 0; i < glVerticesCount - 2; i++) {
				shadeTriangle(px[i], py[i], px[i+1], py[i+1], px[i+2], py[i+2], glColor_T[i], glColor_T[i+1], glColor_T[i+2]);
			}
		} 
		else if(glShader == FacetShader){
			//vertex normals and normalization
			updateDrawingOrder(NFACES, glVertices, draw_order);
			vertex_normalize(draw_order);
			
			float normColor;
			for(uint16_t trig = 0; trig < glVerticesCount / 3; trig++) {
				//uint16_t i = trig*3;
				uint16_t order = draw_order? draw_order[trig]:trig;  //face to draw
				uint16_t i = order*3;
				normColor = (glVertices[i].nz < 0?0:glVertices[i].nz);
				
				if(normColor <0.001 && glVertices[i].nz != 0 ) normColor = abs(glVertices[i].nz);
				//normColor = abs(glVertices[i].nz);
				uint16_t color = (((uint8_t)(normColor* _r) & 0xF8) << 8) | (((uint8_t)(normColor* _g) & 0xFC) << 3) | ((uint8_t)((normColor* _b)) >> 3);
				/*
				Serial.printf("Indicies %d, %d, %d\n", i, i+1, i+2);
				Serial.printf("\t %d, %d, %d, %f, %x\n", i, px[i], py[i],
					glVertices[i].nz, normColor* _r);
				Serial.printf("\t %d, %d, %d, %f, %x\n", i+1, px[i+1], py[i+1],
					glVertices[i+1].nz, normColor* _g);
				Serial.printf("\t %d, %d, %d, %f, %x\n", i+2, px[i+2], py[i+2],
					glVertices[i+2].nz, normColor* _b);
				*/
				float p[3], q[3], r[3];
				// if triangle is facing the camera, draw it
				p[0] = glVertices[i].x; p[1] = glVertices[i].y; p[2] = glVertices[i].z;
				q[0] = glVertices[i+1].x; q[1] = glVertices[i+1].y; q[2] = glVertices[i+1].z;
				r[0] = glVertices[i+2].x; r[1] = glVertices[i+2].y; r[2] = glVertices[i+2].z;
			
				//Serial.println(facing_camera(p,q,r));
			if(facing_camera(p,q,r) > 0)
				//if (glVertices[i].nz >= 0)
					fillTriangle ( px[i], py[i],
						  px[i + 1], py[i + 1],
						  px[i + 2], py[i + 2], color);
			}
		} 
		else if(glShader == SmoothShader) {		
			
			//TODO FOR MODELS ONLY
			
		} else {
			for(uint16_t trig = 0; trig < ((uint8_t) (glVerticesCount / 3)); trig++){
				//uint16_t ptIdx = trig*3;
				uint16_t i = trig*3;
				if(color_default_array != 1){
					//for(uint16_t i = ptIdx; i < ptIdx+3 - 2; i++) {
					//for(uint16_t i = 0; i < glVerticesCount - 2; i++) {
						drawLine(px[i], py[i], px[i + 1], py[i + 1], glColor_T[i]);
						drawLine(px[i], py[i], px[i + 2], py[i + 2], glColor_T[i+1]);
						drawLine(px[i + 1], py[i + 1], px[i + 2], py[i + 2], glColor_T[i+2]);
					//}
				} else {
					//for(uint16_t i = ptIdx; i < ptIdx+3 - 2; i++) {
					//for(uint16_t i = 0; i < glVerticesCount - 2; i++) {
						fillTriangle ( px[i], py[i],
								  px[i + 1], py[i + 1],
								  px[i + 2], py[i + 2], glColor);
					//}
					//glColor3ub(255,255,255);
				}
			}
		}
		
    }
    else if(glDrawMode == GL_TRIANGLE_STRIP) {

		/* TODO Improve! */
        if(glVerticesCount < 3)
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
		
		NFACES = glVerticesCount - 2;
        
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
		}
 
		if(glShader == SimpleVertexShader) {
			for(uint16_t i = 0; i < glVerticesCount - 2; i++) {
				shadeTriangle(px[i], py[i], px[i+1], py[i+1], px[i+2], py[i+2], glColor_T[i], glColor_T[i+1], glColor_T[i+2]);
			}
		} 
		else if(glShader == FacetShader){
			//vertex normals and normalization
			updateDrawingOrder(NFACES, glVertices, draw_order);
			vertex_normalize(draw_order);
			
			float normColor;
			for(uint16_t trig = 0; trig < NFACES; trig++) {
				uint16_t order = draw_order? draw_order[trig]:trig;  //face to draw
				uint16_t i = order*3;
				
				normColor = (glVertices[i].nz < 0?0:glVertices[i].nz);
				
				if(normColor <0.001 && glVertices[i].nz != 0 ) normColor = abs(glVertices[i].nz);

				uint16_t color = (((uint8_t)(normColor* _r) & 0xF8) << 8) | (((uint8_t)(normColor* _g) & 0xFC) << 3) | ((uint8_t)((normColor* _b)) >> 3);

				float p[3], q[3], r[3];
				// if triangle is facing the camera, draw it
				p[0] = glVertices[i].x; p[1] = glVertices[i].y; p[2] = glVertices[i].z;
				q[0] = glVertices[i+1].x; q[1] = glVertices[i+1].y; q[2] = glVertices[i+1].z;
				r[0] = glVertices[i+2].x; r[1] = glVertices[i+2].y; r[2] = glVertices[i+2].z;
			
				//Serial.println(facing_camera(p,q,r));
				if(facing_camera(p,q,r) > 0)
					fillTriangle ( px[i], py[i],
						  px[i + 1], py[i + 1],
						  px[i + 2], py[i + 2], color);
			}
		} else {
			for(uint16_t i = 0; i < glVerticesCount - 2; i++) {
				fillTriangle ( px[i], py[i],
						  px[i + 1], py[i + 1],
						  px[i + 2], py[i + 2], glColor);
			}
			//glColor3ub(255,255,255);
		}
    }
}

void Arduino_OpenGL::glColor3ub(uint8_t r, uint8_t g, uint8_t b ) {  //not really openGL format
	_r = r;
	_g = g;
	_b = b;
	glColor = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	color_default = 0;
}

void Arduino_OpenGL::glColorT(uint16_t idx, uint8_t r, uint8_t g, uint8_t b ) {  //not really openGL format
	glColor_T[idx] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	color_default_array = 0;
}

void Arduino_OpenGL::glColorQ(uint16_t idx, uint8_t r, uint8_t g, uint8_t b ) {  //not really openGL format
	glColor_Q[idx] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


/*
 * <https://computergraphics.stackexchange.com/questions/4031/programmatically-generating-vertex-normals> 
 */
FLASHMEM
void Arduino_OpenGL::vertex_normalize(uint16_t *draw_order) 
{ 		
	 float ba[3], ca[3];
	 float vn[3];
	 uint16_t ptIdx;

	for(uint8_t j = 0; j<3; j++) vn[j] = 0.0;
	
 
	if( glDrawMode == GL_TRIANGLES ||  glDrawMode == GL_QUADS) {	
		 for( int i=0; i < NFACES; i++ )
		 {
			//uint8_t ptIdx = i*3;
			uint16_t order = draw_order? draw_order[i]:i;  //face to draw
			if(glDrawMode == GL_QUADS) {
				ptIdx = order*4;
			} else {
				ptIdx = order*3;
			}

			//Serial.printf("Points:%d %f, %f, %f\n", i, glVertices[i].x,glVertices[ptIdx].y, glVertices[ptIdx].z);
			//Serial.printf("Points:%d %f, %f, %f\n", i, glVertices[i+1].x,glVertices[ptIdx+1].y, glVertices[ptIdx+1].z);
			//Serial.printf("Points:%d %f, %f, %f\n", i, glVertices[ptIdx+2].x,glVertices[ptIdx+2].y, glVertices[ptIdx+2].z);

			ba[0] = glVertices[ptIdx+1].x - glVertices[ptIdx].x;
			ba[1] = glVertices[ptIdx+1].y - glVertices[ptIdx].y;		
			ba[2] = glVertices[ptIdx+1].z - glVertices[ptIdx].z;

			ca[0] = glVertices[ptIdx+2].x - glVertices[ptIdx].x;
			ca[1] = glVertices[ptIdx+2].y - glVertices[ptIdx].y;		
			ca[2] = glVertices[ptIdx+2].z - glVertices[ptIdx].z;
			
			//vertex normals
			crossVector3(vn, ba, ca);
			
			glVertices[ptIdx].nx = vn[0]; 
			glVertices[ptIdx].ny = vn[1];
			glVertices[ptIdx].nz = vn[2];
			glVertices[ptIdx+1].nx = vn[0]; 
			glVertices[ptIdx+1].ny = vn[1];
			glVertices[ptIdx+1].nz = vn[2];
			glVertices[ptIdx+2].nx = vn[0]; 
			glVertices[ptIdx+2].ny = vn[1];
			glVertices[ptIdx+2].nz = vn[2];
			
			if(glDrawMode == GL_QUADS) {
				glVertices[ptIdx+3].nx = vn[0]; 
				glVertices[ptIdx+3].ny = vn[1];
				glVertices[ptIdx+3].nz = vn[2];
			}
			
			//Serial.printf("\tVn: (%d) %f, %f, %f\n", i, glVertices[ptIdx].nz , glVertices[ptIdx+1].nz, glVertices[ptIdx+2].nz);
		 }

	} 
	
	else if( glDrawMode == GL_TRIANGLE_STRIP ) {
		 for( int i=0; i < NFACES; i++ )
		 {
			ptIdx = i;
			//Serial.printf("Points:%d %f, %f, %f\n", i, glVertices[i].x,glVertices[ptIdx].y, glVertices[ptIdx].z);
			//Serial.printf("Points:%d %f, %f, %f\n", i, glVertices[i+1].x,glVertices[ptIdx+1].y, glVertices[ptIdx+1].z);
			//Serial.printf("Points:%d %f, %f, %f\n", i, glVertices[ptIdx+2].x,glVertices[ptIdx+2].y, glVertices[ptIdx+2].z);

			ba[0] = glVertices[ptIdx+1].x - glVertices[ptIdx].x;
			ba[1] = glVertices[ptIdx+1].y - glVertices[ptIdx].y;		
			ba[2] = glVertices[ptIdx+1].z - glVertices[ptIdx].z;

			ca[0] = glVertices[ptIdx+2].x - glVertices[ptIdx].x;
			ca[1] = glVertices[ptIdx+2].y - glVertices[ptIdx].y;		
			ca[2] = glVertices[ptIdx+2].z - glVertices[ptIdx].z;
			
			//vertex normals
			crossVector3(vn, ba, ca);
			
			glVertices[ptIdx].nx = vn[0]; 
			glVertices[ptIdx].ny = vn[1];
			glVertices[ptIdx].nz = vn[2];
			glVertices[ptIdx+1].nx = vn[0]; 
			glVertices[ptIdx+1].ny = vn[1];
			glVertices[ptIdx+1].nz = vn[2];
			glVertices[ptIdx+2].nx = vn[0]; 
			glVertices[ptIdx+2].ny = vn[1];
			glVertices[ptIdx+2].nz = vn[2];
			
			//Serial.printf("\tVn: (%d) %f, %f, %f\n", i, vn[0], vn[1], vn[2]);
		 }
	}
	
	for( int i=0; i < glVerticesCount; i++ )
	 {
		//vector normalization
		//normVector3(vnn, vn);
		float norm;
		//Serial.printf("%d, %f, %f, %f\n", i, glVertices[i].nx, glVertices[i].ny, glVertices[i].nz);
		norm = sqrt(glVertices[i].nx * glVertices[i].nx  +  glVertices[i].ny * glVertices[i].ny + glVertices[i].nz * glVertices[i].nz);
 		
		glVertices[i].nx /= norm; 
		glVertices[i].ny /= norm;
		glVertices[i].nz /= norm;
		//Serial.printf("\t%f, %f, %f, (%f)\n\n", glVertices[i].nx, glVertices[i].ny, glVertices[i].nz, norm);
	 }
 }

///////////////////////////////////////////////////////////////////////////////
// compute cofactor of 3x3 minor matrix without sign
// input params are 9 elements of the minor matrix
// NOTE: The caller must know its sign.
///////////////////////////////////////////////////////////////////////////////
float Arduino_OpenGL::getCofactor(float m0, float m1, float m2,
                           float m3, float m4, float m5,
                           float m6, float m7, float m8)
{
    return m0 * (m4 * m8 - m5 * m7) -
           m1 * (m3 * m8 - m5 * m6) +
           m2 * (m3 * m7 - m4 * m6);
}

////////////////////////////////////////////////////////////////////////
//  3D drawing commands ported from from Michael Rule's Uno9341TFT library
////////////////////////////////////////////////////////////////////////
#define readUnsignedByte(t)     ((uint16_t)pgm_read_word(&(t)))
void Arduino_OpenGL::get_triangle_points(Model *M, int16_t *vertices, int16_t i, int16_t **p, int16_t **q, int16_t **r) {
    // Get the vertex indecies for the triangle
    PU16 t = &M->faces[i*3];
    uint16_t pi = readUnsignedByte(t[0]);
    uint16_t qi = readUnsignedByte(t[1]);
    uint16_t ri = readUnsignedByte(t[2]);
    // get the  X and Y coordinates for the triangle
    *p = &vertices[pi*3];
    *q = &vertices[qi*3];
    *r = &vertices[ri*3];
} 

float Arduino_OpenGL::facing_camera(float *p, float *q, float *r) {
    return (r[0]-p[0])*(q[1]-p[1])<((q[0]-p[0])*(r[1]-p[1]));
}
 

////////////////////////////////////////////////////////////////////////
// Triangle shaders
////////////////////////////////////////////////////////////////////////


/*  Computes convex combination of color1 and color2 with weight
 *  Weight is normalizes s.t. [0,1] maps to [0,256]
 */
uint16_t Arduino_OpenGL::interpolate(int16_t color1, int16_t color2, int16_t alpha) {
  // The clean way: break out the RGB components and reassemble
  return color1 * alpha + color2 * ((int) _div-alpha) >> 5;
}


// Horizontal fill of a segment with color interpolation
void Arduino_OpenGL::interpolateFlood(int16_t x, int16_t y, int16_t i, int16_t stop, int16_t length, uint16_t color1, uint16_t color2)
{
    uint16_t alpha = 0xff;
	
    while (i< stop) {
        uint16_t weight = i*_div/length;
        //if (weight != alpha) {
            alpha = weight;
            uint16_t c = interpolate(color1, color2, alpha);
			drawPixel(i+x, y , c);
        //}
        i++;
    }
}

// Fast horizontal line supporting overdraw and interpolation
// Does not support masking
void Arduino_OpenGL::interpolateFastHLine(int16_t x, int16_t y, uint16_t length, uint16_t color1, uint16_t color2) {
    if (length < 1) return;
    interpolateFlood(x, y, 0, length, length,color1,color2);
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
    uint16_t midpoint = interpolate(color2,color0,(y1-y0)*_div/(y2-y0));
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
        float weight = ((y-y0)*_div)/len2;
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
        float weight = 32.-((y-y1)*_div)/len3;
        uint16_t colorA = interpolate(colorU,colorW,weight);
        uint16_t colorB = interpolate(colorV,colorW,weight);
        if(a > b) {
            swap(a,b);
            swap(colorA,colorB);
        }
        interpolateFastHLine(a, y, b-a+1, colorA, colorB);
    }
}

////////////////////////////////////////////////////////////////////////
// Either face_colors or vertex_colors may be NULL, to use the model-
// specified colors, or the current foreground color if those are not
// available.
// Draw order may be NUL, but if it is provided triangles are sorted
// from front to back and overdraw avoidance is used. 
////////////////////////////////////////////////////////////////////////
/*  To do correct flat shading to do depths.
void Arduino_3D::fillFaces(Model *M, int16_t *vertices, uint16_t *face_colors, uint16_t *draw_order) {
    updateDrawingOrder(M,vertices,draw_order);
    uint16_t color = foreground_color;
    uint16_t nt    = M->NFaces;
    for (int j=0; j<nt; j++) {
        int i = draw_order? draw_order[j] : j;
        int16_t *p,*q,*r;
        get_triangle_points(M,vertices,i,&p,&q,&r);
        if (facing_camera(p,q,r)) {
            if (face_colors  !=NULL) color = color_map[face_colors[i]]*0x0101;
            uint16_t  x1 = p[0]+X0;
            uint16_t  x2 = q[0]+X0;
            uint16_t  x3 = r[0]+X0;
            fillTriangle(x1,p[1]+Y0,x2,q[1]+Y0,x3,r[1]+Y0,color);
        }
    }
}
*/

void Arduino_OpenGL::shadeFaces(GLVertex *vertices, uint16_t *vertex_colors, uint16_t *draw_order) {
    uint16_t nt    = NFACES;
    updateDrawingOrder(nt, vertices, draw_order);
	float p[3], q[3], r[3];
	uint8_t idx;
    for (int j=0; j<nt; j++) {
		idx = j*3;
        int i = draw_order? draw_order[j]:j;
		//aux is the transformed vertices that were read in 
		//previously using get_triangle
		// if triangle is facing the camera, draw it
		p[0] = vertices[j].x; p[1] = vertices[j].y; p[2] = vertices[j].z;
		q[0] = vertices[j+1].x; q[1] = vertices[j+1].y; q[2] = vertices[j+1].z;
		r[0] = vertices[j+2].x; r[1] = vertices[j+2].y; r[2] = vertices[j+2].z;
		
        if (facing_camera(p,q,r)) {
            //uint16_t color1 = vertex_colors[pi];
            //uint16_t color2 = vertex_colors[qi];
            //uint16_t color3 = vertex_colors[ri];
            uint16_t  x1 = p[0];
            uint16_t  x2 = q[0];
            uint16_t  x3 = r[0];
            //shadeTriangle(x1,p[1],x2,q[1],x3,r[1],color1,color2,color3);
        }
    }
}


////////////////////////////////////////////////////////////////////////
// Non-convex 3D surfaces can overlap themselvs. Sorting triangles from
// front to back and checking to make sure we don't draw on top of areas
// that have already been drawn can avoid Arduino_3D::overlap artefacts. To 
// maintain sorted lists of polygons across frames, theses functions
// accept a permutation list for the triangle drawing order. The 
// permutation is updated to reflect the current z-order. 
////////////////////////////////////////////////////////////////////////

// Helper routine for sorting triangles
// Faces may remain partially ordered after rotating the object
// To take advantage of this, we store and use a fixed permutation 
// array "draw_order"

void Arduino_OpenGL::computeTriangleDepths(uint16_t nt, GLVertex *vertices, uint16_t *draw_order, float *depths) {
    //uint16_t nt = NFACES;
    float p,q,r,s;
	uint16_t idx;
	
	if(glDrawMode == GL_QUADS){
		for (int j=0; j < nt; j++) {
			int i = draw_order!=NULL? draw_order[j]:j;
			idx = j*4;
			p = vertices[idx].z;
			q = vertices[idx+1].z;
			r = vertices[idx+2].z;
			s = vertices[idx+3].z;
			
			// get the rotated vertex Z coordinates for the triangle
			float z = (p+q+r+s)/4;
			depths[j] = z;
		}
	} else if(glDrawMode == GL_TRIANGLES){
		for (int j=0; j < nt; j++) {
			int i = draw_order!=NULL? draw_order[j]:j;
			idx = j*3;
			p = vertices[idx].z;
			q = vertices[idx+1].z;
			r = vertices[idx+2].z;
			
			// get the rotated vertex Z coordinates for the triangle
			float z = (p+q+r)/3;
			depths[j] = z;
		}
	}
}

// Sorts triangles from front to back to properly handle occlusions
// Bubble sort is in fact the efficient solution here. 
// It is O(N) for sorted data, and requires no additional memory to sort. 
// Triangles remain mostly sorted as object rotates.
void Arduino_OpenGL::updateDrawingOrder(uint16_t nt, GLVertex *vertices, uint16_t *draw_order) {
    if (draw_order==NULL) return;
    //uint16_t nt = NFACES;
    float depths[nt];
    computeTriangleDepths(nt, vertices, draw_order, depths);
    // Bubble sort the triangles by depth keeping track of the permutation
    uint8_t sorted = 0;
    while (!sorted) {
        sorted = 1;
        for (int i=1;i<nt;i++) {
            float d1 = depths[i-1];
            float d2 = depths[i];
            if (d2>d1) {
                depths[i-1] = d2;
                depths[i]   = d1;
                uint16_t temp    = draw_order[i];
                draw_order[i]   = draw_order[i-1];
                draw_order[i-1] = temp;
                sorted = 0;
            }
        }
    }
}
