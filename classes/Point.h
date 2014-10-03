#ifndef Point_H
#define Point_H

#include <cstdlib>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>

class Point {
	GLfloat x, y, z;
  
    public:
      Point() {x = y = z = 0;}
      // virtual ~Vectorz(){}
      Point(GLfloat r, GLfloat g, GLfloat b) {
      	x = r; 
 		y = g; 
 		z = b;
      }
      GLfloat getX() {return x;}
      GLfloat getY() {return y;}
      GLfloat getZ() {return z;}
      void setX(GLfloat val) {x = val;}
      void setY(GLfloat val) {y = val;}
      void setZ(GLfloat val) {z = val;}
      void setValues(GLfloat x1, GLfloat y1, GLfloat z1);
};

#endif