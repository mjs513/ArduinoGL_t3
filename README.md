## ArduinoGL_t3

The ArduinoGL_t3 library is a port of the ArduinoGL library by fabio914: https://github.com/fabio914/arduinogl.  The lib was modified to support framebuffering used in the ILI9488_t3 (https://github.com/mjs513/ILI9488_t3) and the ILI9341_t3n (https://github.com/KurtE/ILI9341_t3n) libraries for the Teens 3.x and the Teensy 4.x.

## OPENGL FEATURES

It is able to draw 2D and 3D wireframes! It has no backface culling, no frustum culling, no colours, no depth/z buffer, no textures, no lighting, no shading, etc...

glBegin(mode) only supports GL_POINTS, GL_POLYGON and GL_TRIANGLE_STRIP. Since Arduino UNO has only 2 Kb of RAM, there is a limit to the number of matrices on the stack (#define MAX_MATRICES 8) and vertices being processed (#define MAX_VERTICES 24) .  These can of course be changed at the users discretion.

## DISPLAY SELECTION FEATURE

To use either the ILI9341 or te ILI9488 the user will have to edit the ArduinoGL.h file and uncomment one on the following lines, in this the ILI9341 is uncommented so it will be configured to use the ILI9341_t3n library:
```
//#define ILI9488 1
#define ILI9341 1
```

In the sample sketch, openGL_square.ino, the code is set up to select either of the displays with a simple define:
```
//#define ILI9488 1
#define ILI9341 1

// For the Adafruit shield, these are the default.
#define TFT_DC  9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
#if defined(ILI9488)
Arduino_OpenGL tft = Arduino_OpenGL(&SPI, TFT_CS, TFT_DC, 8);
#elif defined(ILI9341)
Arduino_OpenGL tft = Arduino_OpenGL(TFT_CS, TFT_DC, 8);
#endif
```
You of course don't have to do it this way, if you are using the ILI9341 display you could just do:
```
#define ILI9341 1

// For the Adafruit shield, these are the default.
#define TFT_DC  9
#define TFT_CS 10

Arduino_OpenGL tft = Arduino_OpenGL(TFT_CS, TFT_DC, 8);
```

Also since right now you are only limited to "White" for drawings.  Maybe at some point will add ability to add colors for vertices and lines.
