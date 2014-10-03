#ifndef Vectorz_H
#define Vectorz_H

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

#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;

class Vectorz
{
  GLfloat x, y, z;
  
    public:
      Vectorz() {x = y = z = 0;}
      // virtual ~Vectorz(){}
      Vectorz(GLfloat r, GLfloat g, GLfloat b);
      GLfloat getX() {return x;}
      GLfloat getY() {return y;}
      GLfloat getZ() {return z;}
      void setX(GLfloat val) {x = val;}
      void setY(GLfloat val) {y = val;}
      void setZ(GLfloat val) {z = val;}
      void setValues(GLfloat x1, GLfloat y1, GLfloat z1);
      Vectorz flip();
      GLfloat length() {return sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());}
      Vectorz normalize();
      static GLfloat dot(Vectorz v1, Vectorz v2) {return v1.getX() * v2.getX() + v1.getY() * v2.getY() + v1.getZ() * v2.getZ();}
      static Vectorz add(Vectorz v1, Vectorz v2) {
        Vectorz res;
        res.setX(v1.getX()+v2.getX());
        res.setY(v1.getY()+v2.getY());
        res.setZ(v1.getZ()+v2.getZ());
        return res;
      }

      static Vectorz subtract(Vectorz v1, Vectorz v2) {
        Vectorz res;
        res.setX(v1.getX()-v2.getX());
        res.setY(v1.getY()-v2.getY());
        res.setZ(v1.getZ()-v2.getZ());
        return res;
      }

      static Vectorz elementMult(Vectorz v1, Vectorz v2) {
        Vectorz res;
        res.setX(v1.getX()*v2.getX());
        res.setY(v1.getY()*v2.getY());
        res.setZ(v1.getZ()*v2.getZ());
        return res;
      }

      static Vectorz scale(Vectorz v1, GLfloat val) {
        Vectorz res;
        res.setX(v1.getX()*val);
        res.setY(v1.getY()*val);
        res.setZ(v1.getZ()*val);
        return res;
      }
      void printV();

};


#endif
