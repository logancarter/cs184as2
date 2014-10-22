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
    virtual int isDLight() { return 0; } // This is a test result -- if you see this you know that something errored (should always be 0 or 1).
    virtual int isALight() { return 0; }
};

class AmbientLight: public Light {
  public:
    void setValues(float r2, float g2, float b2) {
      r = r2; g = g2; b = b2; x = 30; y = 30; z = 30;//x, y, z shouldn't get used
    }
    int isALight() {
      return 1;
    }
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
  Vector4f coordinates;
  
    public:
      Point() { x = y = z = 0; }
      // virtual ~Vectorz(){}
      Point(float r, float g, float b) {
      	x = r; 
     		y = g; 
     		z = b;
        coordinates(0) = r;
        coordinates(1) = g;
        coordinates(2) = b;
        coordinates(3) = 1;
      }
      float getX() {return x;}
      float getY() {return y;}
      float getZ() {return z;}
      Vector4f getCoords() { return coordinates; }
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
	Vector4f pos; // E
	Vector4f dir; // P - E

public:
	// ray(t) = pos + t * dir
  // TODO: getters and setters
	Ray();
  // Ray(float pos_x, float pos_y, float pos_z, float dir_x, float dir_y, float dir_z);
  Ray(Vector4f eye, Vector4f pixel);
  void setEye(Vector4f eye);
  void setDir(Vector4f pixel);  // Takes in the pixel, will generate the vector (P - E) inside!
  Vector4f getPos();
  Vector4f getDir();
};


//****************************************************
// Primitive
//****************************************************

class Primitive {
  Transformation o2w;
public:
  Primitive();
  //virtual void isPrimitive() { cout << "0" << endl; }
  Transformation getTransform() { return w2o; }
  virtual bool intersect(Ray ray);
};


//****************************************************
// SPHERE         
//****************************************************

class Sphere: public Primitive {
  float radius, center_x, center_y, center_z;
public:
  Sphere();
  Sphere(float r, float x, float y, float z);
  //virtual void isPrimitive() { cout << "1" << endl; }
  Vector4f getCenter();
  float getRadius();
  bool testIntersect(float a, float b, float c, float &x1, float &x2);
  virtual bool intersect(Ray ray);
};


//****************************************************
// TRIANGLE       
//****************************************************

class Triangle: public Primitive {
  Point a, b, c;
public:
  Triangle();
  Triangle(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz);
  //virtual void isPrimitive() { cout << "2" << endl; }

  // TODO: triangle intersection
  // bool testIntersect(float a, float b, float c, float &x1, float &x2);
  // virtual bool intersect(Ray ray);
};


//****************************************************
// LCAOLGEO
//****************************************************
class LocalGeo {
  float x, y, z;
  Vector4f pos, normal; // normalize
public:
  LocalGeo();
  LocalGeo(float x1, float y1, float z1, float nx, float ny, float nz);
};


//****************************************************
// COLOR
//****************************************************

class Color {
  float r, g, b;
public:
  Color();
  Color(float rv, float gv, float bv);
  float getR();
  float getG();
  float getB();
  void setRGB(float rv, float gv, float bv);
};


//****************************************************
// INTERSECTION
//****************************************************

class Intersection {
  LocalGeo lg;
  Primitive* primitive;
public:
  Intersection();
  Intersection(LocalGeo local, Primitive &s);
};


//****************************************************
// TRANSFORMATION
//****************************************************

class Transformation {
  Matrix4f mat, inv; // transpose
public:
  Transformation();
  void setMat(Matrix4f m);
  Matrix4f getMat();
  Matrix4f getInv();
};

//****************************************************
// SAMPLE
//****************************************************

class Sample {
  // TODO: MAKE COLOR 3 CHANNEL!!!!!!!!!
	float x, y;
  float color;

public:
  Sample();
	Sample(float xval, float yval, float default_color);
	float getX() { return x; }
	float getY() { return y; }
  float getColor() { return color; }
	void setX(float val) { x = val; }
	void setY(float val) { y = val; }
	void setColor(float val) { color = val; }
};


//****************************************************
// SAMPLER
//****************************************************

class Sampler {
	int width, height, curr_x, curr_y;
public:
  Sampler();
	Sampler(int w, int h);
  void getFirstSample(Sample *sample);
	bool getNextSample(Sample *sample);
	int getWidth() { return width; }
	int getHeight() { return height; }

};


//****************************************************
// FILM
//****************************************************

class Film {
	CImg<float> image;
public:
	// v refers to the RGB (R=0, G=1, B=2)
  Film();
	Film(int w, int h, int z, int v, int default_color);
	void setPixel(int x, int y, int z, int v, int color);
	void displayToScreen();
};


//****************************************************
// CAMERA
//****************************************************

class Camera
{
  float eye_x, eye_y, eye_z;
  int width, height;
  // DEV: When working with eye and stuff for reals, make a vector on the spot out of them. This class will merely be a container for the values.
  Vector4f eye, lr, ll, ur, ul;
  float plane_width, plane_height, scale_w, scale_h;
  // TODO: mapping from for the input corners to output size

public:
  Camera(); // four corners, width, height, eye
  Camera(float x, float y, float z, int w, int h, float llx, float lly, float llz, float lrx, float lry, float lrz, float ulx, float uly, float ulz, float urx, float ury, float urz);
  float getX() { return eye_x; }
  float getY() { return eye_y; }
  float getZ() { return eye_z; }
  Vector4f get_eye() {
    return eye;
  }
  void generateRay(Sample sample, Ray *ray); // sample& to sample TODO

};


//****************************************************
// RAYTRACER
//****************************************************

class RayTracer {
public:
  RayTracer();
  // void trace(Ray& ray, int depth, Color* color);
  void trace(Ray ray, Sample *sample, Primitive &primitive, std::vector<Light> lights);  // Hacked method, deprecate this!
};


//****************************************************
// SCENE
//****************************************************

class Scene
{
  Camera camera;
  Sampler sampler;
  Film film;
  RayTracer raytracer;
  std::vector<Primitive *> primitives;
  std::vector<Light> lights;

  public:
    Scene(Sampler &s, Film &f, Camera &c, RayTracer &rt);
    void addPrimitive(Primitive &primitive); // TODO: decide if this needs to be pointer or not
    void render();
    void addLight(Light &light);
//     ...
//     bool intersect(Ray &r, double &closest_t, GeometryProperties &geom_prop, MaterialProperties &mat_prop);
    // void calculateAcceleration();
//   private:
    // AABBNode acceleration_root;
//     std::vector<Primitive> primitives;
};






















#endif