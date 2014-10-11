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

// #ifdef OSX
// #include <GLUT/glut.h>
// #include <OpenGL/glu.h>
// #else
// #include <GL/glut.h>
// #include <GL/glu.h>
// #endif

#include <time.h>
#include <math.h>
#include "CImg.h"

#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;
using namespace Eigen;
using namespace cimg_library; 

//****************************************************
// VECTORZ
//****************************************************

class Vectorz
{
  float x, y, z;
  
    public:
      Vectorz() {x = y = z = 0;}
      // virtual ~Vectorz(){}
      Vectorz(float r, float g, float b);
      float getX() {return x;}
      float getY() {return y;}
      float getZ() {return z;}
      void setX(float val) {x = val;}
      void setY(float val) {y = val;}
      void setZ(float val) {z = val;}
      void setValues(float x1, float y1, float z1);
      Vectorz flip();
      float length() {return sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());}
      Vectorz normalize();
      static float dot(Vectorz v1, Vectorz v2) {return v1.getX() * v2.getX() + v1.getY() * v2.getY() + v1.getZ() * v2.getZ();}
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

      static Vectorz scale(Vectorz v1, float val) {
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
	float mat[4][4];

	public:
		Matrixz();
		float** getMatrix();
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
    float r, g, b, x, y, z;
    void setValues(float x1, float y1, float z1, float r1, float g1, float b1) {
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
	float x, y, z;
  
    public:
      Point() {x = y = z = 0;}
      // virtual ~Vectorz(){}
      Point(float r, float g, float b) {
      	x = r; 
 		y = g; 
 		z = b;
      }
      float getX() {return x;}
      float getY() {return y;}
      float getZ() {return z;}
      void setX(float val) {x = val;}
      void setY(float val) {y = val;}
      void setZ(float val) {z = val;}
      void setValues(float x1, float y1, float z1);
};


//****************************************************
// RAY
//****************************************************

class Ray {
	// Point pos;
	// Vectorz dir;
	float t_min, t_max;
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
	float eye_x, eye_y, eye_z;
	int width, height;
	// TODO: mapping from for the input corners to output size

public:
	Camera(); // four corners, width, height, eye
	float getX() { return eye_x; }
	float getY() { return eye_y; }
	float getZ() { return eye_z; }
	// Vector4f get_eye() { 
	float* get_eye() {
		static float arr[3] = {eye_x, eye_y, eye_z};
		return arr;
	}

};


//****************************************************
// SAMPLE
//****************************************************

class Sample {
	float x, y, color;

public:
	Sample(float xval, float yval, float default_color);
	float getX() { return x; }
	float getY() { return y; }
  float getColor() { return color; }
	void setX(float val) { x = val; }
	void setY(float val) { y = val; }
	void setColor(float val) { color = val; }
};


//****************************************************
// SAMPLER (TODO: Discuss whether we need this extra wasted space, rather than working with CImg directly)
//****************************************************

class Sampler {
	int width, height, curr_x, curr_y, done;
public:
	Sampler(int w, int h);
	Sample getNextSample();
	int getWidth() { return width; }
	int getHeight() { return height; }
	int isDone() { return done; }

};


//****************************************************
// FILM
//****************************************************

class Film {
	CImg<float> image;
public:
	// v refers to the RGB (R=0, G=1, B=2)
	Film(int w, int h, int z, int v, int default_color);
	void setPixel(int x, int y, int z, int v, int color);
	void displayToScreen();
};








#endif