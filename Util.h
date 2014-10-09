/* Util.h */

#ifndef Util_H
#define Util_H

#include <Eigen/Dense>

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
using namespace Eigen;

//****************************************************
// VECTORZ
//****************************************************

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


//****************************************************
// NORMAL
//****************************************************
/* Derive a class from Vectorz, call super's functions then call normalize at the end of each. */


//****************************************************
// MATRIXZ
//****************************************************

class Matrixz {
	GLfloat mat[4][4];

	public:
		Matrixz();
		GLfloat** getMatrix();
		Matrixz createRotation();
		Matrixz createTranslation();
		Matrixz createScaling();
		// Matrix invert();
		// Matrix SVD();
};

//****************************************************
// LIGHT
//****************************************************

class Light {
  public:
    GLfloat r, g, b, x, y, z;
    void setValues(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat r1, GLfloat g1, GLfloat b1) {
      x = x1; y = y1; z = z1; r = r1; g = g1; b = b1;
    }
    virtual int isDLight() { return 30; } // This is a test result -- if you see this you know that something errored (should always be 0 or 1).
};

class PointLight: public Light {
  public:
    int isDLight() {
      return 0;
    }
};

class DirectionalLight: public Light {
public:
    int isDLight() {
      return 1;
    }
};


//****************************************************
// POINT
//****************************************************

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


//****************************************************
// RAY
//****************************************************

class Ray {
	// Point pos;
	// Vectorz dir;
	GLfloat t_min, t_max;
	Vector4f pos;
	Vector4f dir;

public:
	// ray(t) = pos + t * dir
	Ray();

};


//****************************************************
// SCENE
//****************************************************

class Scene
{
  public:
  	void render();
//     ...
//     bool intersect(Ray &r, double &closest_t, GeometryProperties &geom_prop, MaterialProperties &mat_prop);
//   private:
//     std::vector<Primitive> primitives;
//     std::vector<Light> lights;
};


//****************************************************
// CAMERA
//****************************************************

class Camera
{
	GLfloat eye_x, eye_y, eye_z;
	int width, height;
	// TODO: mapping from for the input corners to output size

public:
	GLfloat getX() { return eye_x; }
	GLfloat getY() { return eye_y; }
	GLfloat getZ() { return eye_z; }
	Vector4f get_eye() { 
		static GLfloat arr[3] = {eye_x, eye_y, eye_z};
		return arr;
	}

};


//****************************************************
// SAMPLE
//****************************************************

class Sample {
	GLfloat x, y;

public:
	GLfloat getX() { return x; }
	GLfloat getY() { return y; }
	setX(GLfloat val) { x = val; }
	setY(GLfloat val) { y = val; }
};


//****************************************************
// SAMPLER
//****************************************************

class Sampler {
	int width, height;


};









#endif