/*
    ArduinoGL.h - OpenGL subset for Arduino.
    Created by Fabio de Albuquerque Dela Antonio
    fabio914 at gmail.com
 */

#include "ArduinoGL.h"
#include <SPI.h>
#if defined ILI9488
#include "ILI9488_t3.h"
#elif defined(ILI9341)
#include "ILI9341_t3n.h"
#include <SPIN.h>
#endif

#if defined(ILI9488)
Arduino_OpenGL::Arduino_OpenGL(SPIClass *SPIWire, uint8_t _CS, uint8_t _DC, uint8_t _RST, uint8_t _MOSI, uint8_t _SCLK, uint8_t _MISO) : ILI9488_t3(SPIWire, _CS, _DC, _RST, _MOSI, _SCLK, _MISO) 
{
}
#elif defined(ILI9341)
Arduino_OpenGL::Arduino_OpenGL(uint8_t _CS, uint8_t _DC, uint8_t _RST, uint8_t _MOSI, uint8_t _SCLK, uint8_t _MISO, SPINClass *pspin) : ILI9341_t3n(  _CS, _DC, _RST, _MOSI, _SCLK, _MISO, (SPINClass*)(&SPIN)) 
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
    fillScreen(color);
}

void Arduino_OpenGL::glPointSize(unsigned size) {
    glPointLength = size;
}

void Arduino_OpenGL::glBegin(GLDrawMode mode) {
    glDrawMode = mode;
    glVerticesCount = 0;
}

void Arduino_OpenGL::glEnd(void) {
    
    if(glDrawMode == GL_NONE)
        return;
    
    float modelviewProjection[16];
    multMatrix(modelviewProjection, glMatrices[GL_PROJECTION], glMatrices[GL_MODELVIEW]);
    
    int frameWidth = _width;
    int frameHeight = _height;
    
    for(int i = 0; i < glVerticesCount; i++) {

        GLVertex aux = multVertex(modelviewProjection, glVertices[i]);
        
        aux.x = aux.x/aux.w;
        aux.y = aux.y/aux.w;
        aux.z = aux.z/aux.w;
        
        glVertices[i] = aux;
    }
    
    if(glDrawMode == GL_POINTS) {
        
        for(int i = 0; i < glVerticesCount; i++) {
            
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
                    drawPixel(x, y, 0xFFFF);  //White for now
        }
    }
    
    else if(glDrawMode == GL_POLYGON) {

        /* TODO Improve! */
        if(glVerticesCount < 2)
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
            
        for(int i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
        
        for(int i = 0; i < glVerticesCount; i++) {
            
            int next = (i + 1 == glVerticesCount) ? 0:(i + 1);
            drawLine(px[i], py[i], px[next], py[next], 0xFFFF);
        }
    }
    
    else if(glDrawMode == GL_TRIANGLE_STRIP) {
        
        /* TODO Improve! */
        if(glVerticesCount < 3)
            return;
        
        int px[MAX_VERTICES], py[MAX_VERTICES];
        
        for(int i = 0; i < glVerticesCount; i++) {
            
            if(!(glVertices[i].z >= -1.0 && glVertices[i].z <= 1.0))
                return;
            
            GLVertex * aux = &(glVertices[i]);
            
            px[i] = (((aux->x + 1.0)/2.0) * (frameWidth - 1));
            py[i] = ((1.0 - ((aux->y + 1.0)/2.0)) * (frameHeight - 1));
        }
        
        for(int i = 0; i < glVerticesCount - 2; i++) {
            
            drawLine(px[i], py[i], px[i + 1], py[i + 1], 0xFFFF);
            drawLine(px[i], py[i], px[i + 2], py[i + 2], 0xFFFF);
            drawLine(px[i + 1], py[i + 1], px[i + 2], py[i + 2], 0xFFFF);
            
        }
    }
}