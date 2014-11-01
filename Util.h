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
// TRANSFORMATION
//****************************************************

class Transformation {
  Matrix4f mat, inv; // transpose
public:
  Transformation();
  void setID() {
    mat << 1, 0, 0, 0,
           0, 1, 0, 0,
           0, 0, 1, 0,
           0, 0, 0, 1;
    inv = mat;
  }
  void setMat(Matrix4f m);
  Matrix4f getMat();
  Matrix4f getInv();
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
  void appendRGB(float rv, float gv, float bv) { r += rv; g+= gv; b+= bv; }
  void addColor(Color color) { r += color.getR(); g+= color.getG(); b+= color.getB(); }
  void printV() { cout << r << " " << g << " " << b << endl; }
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
  void setDir(Vector4f pminuse);  // Please pass in (P - E)
  void flipDir() { dir = dir * -1; }
  Vector4f getPos();
  Vector4f getDir();
  float getTmin() {return t_min;}
  float getTmax() {return t_max;}
  void setTmax(float number) {t_max = number;}
  void setTmin(float number) {t_min = number;}
};


//****************************************************
// LOCALGEO
//****************************************************
class LocalGeo {
  // float x, y, z;
  Vector4f pos, normal; // normalize
public:
  LocalGeo();
  // LocalGeo(float x1, float y1, float z1, float nx, float ny, float nz);
  void setPos(Vector4f p) { pos = p; }
  void setNormal(Vector4f n) { normal = n; }
  Vector4f getPos() { return pos; }
  Vector4f getNormal() { return normal; }
};

class Primitive;

//****************************************************
// INTERSECTION
//****************************************************

class Intersection {
  LocalGeo lg;
  Primitive* primitive;
public:
  Intersection();
  Intersection(LocalGeo local, Primitive &s);
  void setPrimitive(Primitive *p) { primitive = p; }
  void setLocalGeo(LocalGeo local) { lg = local; }
  LocalGeo getLocalGeo() { return lg; }
  Primitive* getPrimitive() { return primitive; }
};


//****************************************************
// LIGHT
//****************************************************

class Light {
  public:
    float r, g, b, x, y, z;
    Vector4f pos;
    Vector3f color;
    virtual void setValues(float x1, float y1, float z1, float r1, float g1, float b1) {
      x = x1; y = y1; z = z1; r = r1; g = g1; b = b1;
      pos << x, y, z, 1;
    }
    virtual int isDLight() { return 30; } // This is a test result -- if you see this you know that something errored (should always be 0 or 1).
    virtual int isALight() { return 0; }
    float getRColor() {return r;}
    float getGColor() {return g;}
    float getBColor() {return b;}
    Vector4f getPos() { return pos; }
    float getX() { return x; }
    float getY() { return y; }
    float getZ() { return z; }
    Vector3f getColor() { return color; }
    virtual void getLightRay(Ray* light_ray, Color* light_color, LocalGeo lg) = 0;
};

class AmbientLight: public Light {
  public:
    AmbientLight(float r2, float g2, float b2) {
      r = r2;
      g = g2;
      b = b2;
      x = y = z = 0.0;
      color << r2, g2, b2;
      pos << x, y, z, 1;
    }
    virtual void setValues(float r2, float g2, float b2) {
      r = r2; g = g2; b = b2; x = y = z = 0.0;   //x, y, z shouldn't get used
      color << r2, g2, b2;
      pos << x, y, z, 1;
    }
    virtual int isALight() {
      return 1;
    }
    virtual void getLightRay(Ray* light_ray, Color* light_color, LocalGeo lg) {} ;
};

class PointLight: public Light {
  public:
    virtual int isDLight() {
      return 0;
    }    
    virtual void setValues(float x1, float y1, float z1, float r1, float g1, float b1) {
      x = x1; y = y1; z = z1; r = r1; g = g1; b = b1;
      // TODO...changed 0 to 1 
      pos << x, y, z, 1;
      color << r, g, b;
    }
    virtual void getLightRay(Ray* light_ray, Color* light_color, LocalGeo lg);
};

class DirectionalLight: public Light {
public:
    virtual int isDLight() {
      return 1;
    }    
    virtual void setValues(float x1, float y1, float z1, float r1, float g1, float b1) {
      x = x1; y = y1; z = z1; r = r1; g = g1; b = b1;
      pos << x1, y1, z1, 0;
      color << r1, g1, b1;
    }
    virtual void getLightRay(Ray* light_ray, Color* light_color, LocalGeo lg);
};


//****************************************************
// BRDF
//****************************************************

class BRDF {
  Vector3f ka, kd, ks, kr;
  int ambient, diffuse, specular, reflection;
  float ksp;
public:
  BRDF() { 
    ambient = diffuse = specular = reflection = 0; 
    ksp = 0.0;
    ka << 0, 0, 0;
    kd << 0, 0, 0;
    ks << 0, 0, 0;
    kr << 0, 0, 0;
  }
  int hasAmbient() { return ambient; }
  int hasDiffuse() { return diffuse; }
  int hasSpecular() { return specular; }
  int hasReflection() { return reflection; }
  Vector3f getKA() { 
    return ka; }
  Vector3f getKD() { return kd; }
  Vector3f getKS() { return ks; }
  Vector3f getKR() { return kr; }
  float getKSP() { return ksp; }
  void setAmbient(float r, float g, float b) {
    if ((r == 0.0) && r == g && g == b) {
      ka << r, g, b;
      ambient = 0;
      // cout << "AMBIENT IS NOTHING!" << endl;
      return;
    }
    ka << r, g, b;
    ambient = 1;
  }
  void setDiffuse(float r, float g, float b) {
    if ((r == 0.0) && r == g && g == b) {
      kd << r, g, b;
      diffuse = 0;
      // cout << "DIFFUSE IS NOTHING!" << endl;
      return;
    }
    kd(0) = r;
    kd(1) = g;
    kd(2) = b;
    diffuse = 1;
  }
  void setSpecular(float r, float g, float b, float sp) {
    if ((r == 0.0) && r == g && g == b) {
      ks << r, g, b;
      specular = 0;
      // cout << "SPECULAR IS NOTHING!" << endl;
      return;
    }
    ks(0) = r;
    ks(1) = g;
    ks(2) = b;
    ksp = sp;
    specular = 1;
  }
  void setReflection(float r, float g, float b) {
    if ((r == 0.0) && r == g && g == b) {
      kr << r, g, b;
      reflection = 0;
      // cout << "REFLECTION IS NOTHING!" << endl;
      return;
    }
    kr(0) = r;
    kr(1) = g;
    kr(2) = b;
    reflection = 1;
  }
};


//****************************************************
// MATERIAL
//****************************************************

class Material {
  BRDF* brdf;
public:
  Material() { brdf = new BRDF(); } // TODO-lookat: no new BRDF(), no viable oper=
  BRDF* getBRDF() { return brdf; }


};

//****************************************************
// PRIMITIVE
//****************************************************

class Primitive {
  Transformation o2w, w2o;
  Material mat;
  string name;
public:
  Primitive();
  virtual void isPrimitive() { cout << "primitive" << endl; }
  Transformation getTransform() { return o2w; }
  virtual bool intersect(Ray &ray, float *thit, Intersection* in);
  virtual bool intersectP(Ray &lray);
  void setMaterial(Material m) { mat = m; } 
  // TODO: posMin needs to be checked outside the function for negative
  float posMin(float t0, float t1) { 
    if (t0 < 0 && t1 > 0) return t1;
    if (t1 < 0 && t0 > 0) return t0;
    // return std::min(std::abs(t0), std::abs(t1)); 
    //cout << t0 << " " << t1 << endl;
    return std::min(t0, t1);
  }
  void setO2W(Transformation trans) {
    o2w = trans;
    //w2o = trans.inv();
    //w2o is just o2w->getInv()
  }
  Transformation getO2W() { return o2w; }
  void setName(string val) { name = val; }
  string getName() { return name; }
  //float posMin(float t0, float t1);
  Material getMaterial() { return mat; }
  virtual Vector4f getCenter() = 0;
  Ray transformRay(Ray &ray);
};


//****************************************************
// SPHERE         
//****************************************************

class Sphere: public Primitive {
  float radius, center_x, center_y, center_z;
public:
  Sphere();
  Sphere(float r, float x, float y, float z);
  virtual void isPrimitive() { cout << "sphere" << endl; }
  virtual Vector4f getCenter();
  float getRadius();
  bool testIntersect(float &a, float &b, float &c, float &x1, float &x2);
  virtual bool intersect(Ray &ray, float *thit, Intersection* in);
  virtual bool intersectP(Ray &lray);
};


//****************************************************
// TRIANGLE       
//****************************************************

class Triangle: public Primitive {
  //Point a, b, c;
  Vector3f a, b, c;
public:
  Triangle();
  Triangle(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz);
  virtual void isPrimitive() { cout << "triangle" << endl; }
  virtual bool intersect(Ray &ray, float *thit, Intersection* in);
  virtual bool intersectP(Ray &lray);
  // TODO: do this vvv
  virtual Vector4f getCenter() { Vector4f v; return v; }
  // TODO: triangle intersection
  // bool testIntersect(float a, float b, float c, float &x1, float &x2);
  // virtual bool intersect(Ray& ray, float *thit, Intersection* in);
};









//****************************************************
// SAMPLE
//****************************************************

class Sample {
	float x, y, z;
  float r, g, b;

public:
  Sample();
	Sample(float xval, float yval, float r1, float g1, float b1);
	float getX() { return x; }
	float getY() { return y; }
  float getZ() { return z; }
  float getRColor() { return r; }
  float getGColor() { return g; }
  float getBColor() { return b; }
	void setX(float val) { x = val; }
	void setY(float val) { y = val; }
  void setZ(float val) { z = val; }
  void setBlack() { r = g = b = 0.0; }
	void setRColor(float val) {r = val;}
  void setGColor(float val) {g = val;}
  void setBColor(float val) {b = val;}
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
	Film(int w, int h, int z, int v, float default_color);
	void setPixel(int x, int y, Color& color);
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
  std::vector<Primitive *> primitives;
  std::vector<Light *> lights;
  Vector4f eye, camera; 
public:
  RayTracer();
  RayTracer(Vector4f eye);
  RayTracer(std::vector<Primitive *> ps, std::vector<Light *> ls, Vector4f cameraPos);
  void addPrimitive(Primitive &primitive);
  void addLight(Light &light);
  void trace(Ray& ray, int depth, Color* color);
  std::vector<Primitive *> getPrims() { return primitives; }
  // void trace(Ray ray, Sample *sample, std::vector<Primitive *> primitives, std::vector<Light *> lights); 
  Color shade(LocalGeo lg, BRDF* brdf, Ray* light_ray, Color *light_color);
  void setEye(Vector4f ai) { eye = ai; }
  Vector4f getEye() { return eye; }
  Ray createReflectRay(LocalGeo lg, Ray& ray);
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
  Color color;

  public:
    Scene(Sampler &s, Film &f, Camera &c, RayTracer &rt);
    void render();
//     ...
//     bool intersect(Ray &r, double &closest_t, GeometryProperties &geom_prop, MaterialProperties &mat_prop);
    // void calculateAcceleration();
//   private:
    // AABBNode acceleration_root;
//     std::vector<Primitive> primitives;
};






















#endif