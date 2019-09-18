// Informatique graphique LIF041 (Alexandre.Meyer -@- liris.cnrs.fr

#ifndef _OPENGL_HLIF041
#define _OPENGL_HLIF041

#if _WIN32 || _WIN64
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut-3.7.6-bin_win/glut.h"
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <unistd.h> //unix standard header
#include <cstdio>
#include <cstdlib>
#include <cmath>

//! A appeller juste avant la boucle principale (Main Loop)
void GLInit(GLsizei Width, GLsizei Height);

//! Quand la fenetre est 'resizée'
void GLResize(GLsizei Width, GLsizei Height);

//! Pour changer la couleur du Ambiant/Diffus/Speculaire en même temps
void GLColor(const float r, const float g, const float b);

//! The main drawing function
void GLDraw(void);

//! Quand une touche est pressée
void keyPressed(unsigned char key, int x, int y);

//! The function called whenever a normal key is pressed
void specialKeyPressed(int key, int x, int y);

//! Load texture and return the GL identifier
enum LOADGLTEXTURE_OPTIONS { LOADGLTEX_EQUAL=0, LOADGLTEX_GREATER=1, LOADGLTEX_LOWER=2};
unsigned int LoadGLTexture(const char* nomfichier, bool isTransparency, const unsigned char R=255, const unsigned char G=255, const unsigned char B=255, int option=LOADGLTEX_EQUAL);

#endif
