#include "Vectorz.h"

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

using namespace std;

 Vectorz::Vectorz(GLfloat r, GLfloat g, GLfloat b) {
 	x = r; 
 	y = g; 
 	z = b;
 }

 void Vectorz::setValues(GLfloat x1, GLfloat y1, GLfloat z1) {
        x = x1; y = y1; z = z1;
      }

Vectorz Vectorz::flip() {
        Vectorz res;
        res.setX(-this->getX());
        res.setY(-this->getY());
        res.setZ(-this->getZ());
        return res;
      }

 Vectorz Vectorz::normalize() {
        Vectorz res;
        res.setValues(this->getX() / this->length(), this->getY() / this->length(), this->getZ() / this->length());
        return res;
      }



      void Vectorz::printV() {
        cout << this->getX() << " " << this->getY() << " " << this->getZ() << endl;
      }