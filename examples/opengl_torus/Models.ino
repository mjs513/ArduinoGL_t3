void drawTri() {
  /* 3 triangles instead of 4 */
  tft.glBegin(GL_TRIANGLES);
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

void drawLineLoop() {
  /* 3 triangles instead of 4 */
  tft.glBegin(GL_LINE_LOOP);
    tft.glColorT(0, 255, 0, 0);
    tft.glColorT(1, 0, 255, 0);
    tft.glColorT(2, 0, 0, 255);
    tft.glVertex3f(0, 1, 0);
    tft.glVertex3f(-1, -1, 0);
    tft.glVertex3f(1, 1, 0);
  tft.glEnd();
}

void drawLines() {
  /* 3 triangles instead of 4 */
  tft.glBegin(GL_LINES);
    tft.glColor3ub(255,255,0);
    tft.glVertex3f(0, 1, 0);
    tft.glVertex3f(0, 0, 0);
  tft.glEnd();
}

void drawPoly(){
  tft.glBegin(GL_POLYGON);
    tft.glColor3ub(255,0,0);
    tft.glVertex3f(0.f, 0.f, 0.f);
    tft.glVertex3f(0.f, 1.f, 0.f);
    tft.glVertex3f(1.f, 1.f, 0.f);
    tft.glVertex3f(1.f, 0.f, 0.f);
  tft.glEnd();
}

void drawQuad2(){
  tft.glBegin(GL_QUADS);
    tft.glColor3ub(255,0,0);
    tft.glVertex3f(0.2, 0.2,0.0);
    tft.glVertex3f(0.8, 0.2,0.0);
    tft.glVertex3f(0.4,0.4,0.0);
    tft.glVertex3f(0.6,0.4,0.0);
    
    tft.glColor3ub(255,255,0);
    tft.glVertex3f(0.4,0.6,0.0);
    tft.glVertex3f(0.6,0.6,0.0);
    tft.glVertex3f(0.2,0.8,0.0);
    tft.glVertex3f(0.8,0.8,0.0);
  tft.glEnd();
}

void drawFilledQuad(){
  tft.glBegin(GL_QUADS);
    tft.glColor3ub(255,0,0);
    tft.glVertex3f(0.f, 0.f, 0.f);
    tft.glVertex3f(0.f, 1.f, 0.f);
    tft.glVertex3f(1.f, 0.f, 0.f);
    tft.glVertex3f(1.f, 1.f, 0.f);
  tft.glEnd();
}

/* Draw a torus */
 void torus()
{
   int i, j, k;
   double s, t, x, y, z, twopi;

   twopi = 2 * PI_;
   for (i = 0; i < numc; i++) {
     tft.glBegin(GL_TRIANGLE_STRIP);
      tft.glColor3ub(102, 178, 255);
      for(int c = 0; c < numc*numt; c++){
        tft.glColorT(c, 255, 255, 0);
      }
      for (j = 0; j <= numt; j++) {
         for (k = 1; k >= 0; k--) {
            s = (i + k) % numc + 0.5;
            t = j % numt;
            x = (1+.1*cos(s*twopi/numc))*cos(t*twopi/numt);
            y = (1+.1*cos(s*twopi/numc))*sin(t*twopi/numt);
            z = .1 * sin(s * twopi / numc);
            tft.glVertex3f(x, y, z);
         }
      }
      tft.glEnd();
   }
}

//Important when drawing quads
   //Coordinate ordinate order.
      // 0---2---4
      // |   |   |
      // 1---3---5
void drawQuad1(){
  tft.glBegin(GL_QUADS);
    tft.glColor3ub(0, 255, 0);
    tft.glVertex3f(0.f, 0.f, 0.f);
    tft.glVertex3f(0.f, 1.f, 0.f);
    tft.glVertex3f(1.f, 1.f, 0.f);
    tft.glVertex3f(1.f, 0.f, 0.f);
  tft.glEnd();
}

void drawPyramid(){
   tft.glBegin(GL_TRIANGLES);           // Begin drawing the pyramid with 4 triangles
      //tft.glColor3ub(102, 178, 255);
      if(_use_FacetShader){
        tft.glColor3ub(102, 178, 255);
      } else {
        for(int c = 0; c < 4; c++){
          tft.glColorT(c, 255, 0, 0);
        }
      }
      // Front
      tft.glVertex3f(  0.0f, 1.0f, 0.0f);
      tft.glVertex3f(-1.0f, -1.0f, 1.0f);
      tft.glVertex3f(1.0f, -1.0f, 1.0f);
 
      // Right
      tft.glVertex3f(0.0f, 1.0f, 0.0f);
      tft.glVertex3f(1.0f, -1.0f, 1.0f);
      tft.glVertex3f(1.0f, -1.0f, -1.0f);
 
      // Back
      tft.glVertex3f(0.0f, 1.0f, 0.0f);
      tft.glVertex3f(1.0f, -1.0f, -1.0f);
      tft.glVertex3f(-1.0f, -1.0f, -1.0f);
 
      // Left
      tft.glVertex3f( 0.0f, 1.0f, 0.0f);
      tft.glVertex3f(-1.0f,-1.0f,-1.0f);
      tft.glVertex3f(-1.0f,-1.0f, 1.0f);
   tft.glEnd();   // Done drawing the pyramid
}

void drawCube()
{

    // glMatrixMode(GL_MODELVIEW);
    // clear the drawing buffer.
    //glClear(GL_COLOR_BUFFER_BIT);
   ///glLoadIdentity();
   //     glTranslatef(0.0,0.0,-10.5);
    //glRotatef(xRotated,1.0,0.0,0.0);
    // rotation about Y axis
    //glRotatef(yRotated,0.0,1.0,0.0);
    // rotation about Z axis
    //glRotatef(zRotated,0.0,0.0,1.0);


  tft.glBegin(GL_QUADS);        // Draw The Cube Using quads
      if(_use_FacetShader){
          tft.glColor3ub(218,165,32);
      } else {
        for(int c = 0; c < 4; c++){
          tft.glColorT(c, 255, 0, 0);
        }
        for(int c = 4; c < 8; c++){
          tft.glColorT(c, 0, 255, 0);
        }
        for(int c = 8; c < 12; c++){
          tft.glColorT(c, 0, 0, 255);
        }
        
        for(int c = 12; c < 16; c++){
          tft.glColorT(c, 255, 255, 0);
        }
        for(int c = 16; c < 20; c++){
          tft.glColorT(c, 0, 255, 255);
        }
        for(int c = 20; c < 24; c++){
          tft.glColorT(c, 255, 0, 255);
        }
      }
   
    tft.glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Top)
    tft.glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Top)
    tft.glVertex3f(-1.0f, 1.0f, 1.0f);    // Bottom Left Of The Quad (Top)
    tft.glVertex3f( 1.0f, 1.0f, 1.0f);    // Bottom Right Of The Quad (Top)
    
    tft.glVertex3f( 1.0f,-1.0f, 1.0f);    // Top Right Of The Quad (Bottom)
    tft.glVertex3f(-1.0f,-1.0f, 1.0f);    // Top Left Of The Quad (Bottom)
    tft.glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Bottom)
    tft.glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Bottom)
    
    tft.glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Front)
    tft.glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Front)
    tft.glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Front)
    tft.glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Front)

    tft.glVertex3f( 1.0f,-1.0f,-1.0f);    // Top Right Of The Quad (Back)
    tft.glVertex3f(-1.0f,-1.0f,-1.0f);    // Top Left Of The Quad (Back)
    tft.glVertex3f(-1.0f, 1.0f,-1.0f);    // Bottom Left Of The Quad (Back)
    tft.glVertex3f( 1.0f, 1.0f,-1.0f);    // Bottom Right Of The Quad (Back)
   
    tft.glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Left)
    tft.glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Left)
    tft.glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Left)
    tft.glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Left)
   
    tft.glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Right)
    tft.glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Right)
    tft.glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Right)
    tft.glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Right)
  tft.glEnd();            // End Drawing The Cube
}

uint8_t teapot;
void drawFromModel(){
  init_model();
  Model *M = &model_data;
 
  uint16_t nt    = M->NFaces;
  tft.glBegin(GL_TRIANGLES);
       if(_use_FacetShader) {
          tft.glColor3ub(218,165,32);
       } else {
         tft.glColor3ub(102, 178, 255);
         for(int c = 0; c < M->NVertices; c++){
           tft.glColorT(c, 255, 255, 0);
         }
       }
      // Get the vertex indicies for the triangle
      for (uint16_t i=0; i< nt; i++) {
           int16_t *p,*q,*r;
           tft.get_triangle_points(M, vertices, i, &p, &q, &r);
           tft.glVertex3f((float) p[0], (float) p[1], (float) p[2]);
           tft.glVertex3f((float) q[0], (float) q[1], (float) q[2]);
           tft.glVertex3f((float) r[0], (float) r[1], (float) r[2]);
      }
    tft.glEnd();
}
