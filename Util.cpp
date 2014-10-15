/* Util.cpp */

#include "Util.h"
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

/*#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif*/

#include <time.h> 
#include <math.h>
#include "CImg.h"

#define PI 3.14159265  // Should be used from mathlib

using namespace std;
using namespace Eigen;
using namespace cimg_library; 


//****************************************************
// VECTORZ
//****************************************************

Vectorz::Vectorz(float r, float g, float b) {
 	x = r; 
 	y = g; 
 	z = b;
}

void Vectorz::setValues(float x1, float y1, float z1) {
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


//****************************************************
// MATRIX
//****************************************************

Matrixz::Matrixz() {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			mat[i][j] = 0;
		}
	}

}


//****************************************************
// POINT
//****************************************************

void Point::setValues(float x1, float y1, float z1) {
	this->x = x1;
	this->y = y1;
	this->z = z1;
}

//****************************************************
// RAY
//****************************************************

Ray::Ray() {

}

Ray::Ray(Vector4f eye, Vector4f pixel) {
	pos = eye;
	dir = pixel - eye;
}

void Ray::setEye(Vector4f eye) {
	pos = eye;
}

void Ray::setDir(Vector4f pixel) {
	dir = pixel - pos;
}

Vector4f Ray::getDir() {
	return dir;
}

Vector4f Ray::getPos() {
	return pos;
}

// Ray::Ray(float pos_x, float pos_y, float pos_z, float dir_x, float dir_y, float dir_z) {
// 	pos = Vector4f
// }


//****************************************************
// SHAPE
//****************************************************

Shape::Shape() {

}

bool Shape::intersect(Ray ray) {
	//cout << "hi" << endl;
	return false;
}

//****************************************************
// SPHERE
//****************************************************

Sphere::Sphere() {

}

Sphere::Sphere(float r, float x, float y, float z) {
	radius = r;
	center_x = x;
	center_y = y;
	center_z = z;
}

Vector4f Sphere::getCenter() {
	Vector4f v(center_x, center_y, center_z, 1);
	return v;
}

float Sphere::getRadius() {
	return radius;
}

bool Sphere::testIntersect(float a, float b, float c, float &x0, float &x1) {
	//cout << "test Intersect" << endl;
	float d = (b*b) - (4 * a * c);
	if (d < 0) {
		cout << "\ntestIntersect  " << d;
		return false;
	}
	else if (d == 0) {
		x0 = x1 = -0.5 * b/a;
	}
	else {
		float q = (b > 0) ? 
		-0.5 * (b + sqrt(d)) :
		-0.5 * (b - sqrt(d));
		x0 = q/a;
		x1 = c/q;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
	}
	return true;
}

//algorithm credit goes to scratchapixel.com
bool Sphere::intersect(Ray ray) {
	float t0;
	float t1;
	//cout << "hi2" << endl;
	Vector4f difference = ray.getPos() - getCenter();
	float a = ray.getDir().dot(ray.getDir());
	float b = 2 * (ray.getDir()).dot(difference);
	float c = difference.dot(difference) - getRadius();
	if (!testIntersect(a, b, c, t0, t1)) {
		return false;
	}
	// if (t0 > ray.tmax) {
	// 	return false;
	// } else {
	// 	ray.tmax = 0;
	// }
	return true;
}

//****************************************************
// LCAOLGEO
//****************************************************

  LocalGeo::LocalGeo() {

  }

  LocalGeo::LocalGeo(float x1, float y1, float z1, float nx, float ny, float nz) {
  	pos(0) = x = x1;
  	pos(1) = y = y1;
  	pos(2) = z = z1;
  	pos(3) = 1;
  	normal(0) = nx;
  	normal(1) = ny;
  	normal(2) = nz;
  	normal(3) = 0;
  	normal.normalize();
  }

//****************************************************
// INTERSECTION
//****************************************************

Intersection::Intersection() {

}


Intersection::Intersection(LocalGeo local, Shape &s) {
	lg = local;
	shape = &s;
}


//****************************************************
// COLOR
//****************************************************


Color::Color() {

}

Color::Color(float rv, float gv, float bv) {
	r = rv; g = gv; b = bv;
}
float Color::getR() { return r; }
float Color::getG() { return g; }
float Color::getB() { return b; }
void Color::setRGB(float rv, float gv, float bv) { r = rv; g = gv; b = bv; }


//****************************************************
// SAMPLE
//****************************************************

Sample::Sample() {
	x = 0;
	y = 0;
	color = 0.0;
}

Sample::Sample(float xval, float yval, float default_color) {
	x = xval;
	y = yval;
	color = default_color;
}


//****************************************************
// SAMPLER
//****************************************************

Sampler::Sampler() {
	// Garbage initialization
	width = 1000;
	height = 500;
	curr_x = -1;
	curr_y = 0;
}

Sampler::Sampler(int w, int h) {
	width = w;
	height = h;
	curr_x = curr_y = 0;
}

void Sampler::getFirstSample(Sample *sample) {
	sample->setX(0);
	sample->setY(0);
	sample->setColor(0.0);
}

bool Sampler::getNextSample(Sample *sample) {
	curr_x++;
	if (curr_x >= width) {
		curr_x = 0;
		curr_y++;
		if (curr_y >= height) {
			return false;
		}
	}
	sample->setX(curr_x);
	sample->setY(curr_y);
	// Test...later add the function that will calculate color.
	// if (curr_x > 200 && curr_x < 300) {
	// 	sample->setColor(243);
	// } else {
	// 	sample->setColor(0.0);
	// }
	return true;
}



//****************************************************
// FILM
//****************************************************

Film::Film() {
	image.assign(1000, 500, 1, 3, 0); 
}

Film::Film(int w, int h, int z, int v, int default_color) {
	image.assign(w, h, z, v, default_color); 
}

// TODO: CHANGE THIS SO WE CAN SET MULTIPLE COLORSSSSSSS NOT JSUT 1111111 FUCK
void Film::setPixel(int x, int y, int z, int v, int color) {
	image(x, y, z, v) = color;
}

void Film::displayToScreen() {
	image.display();
}


//****************************************************
// CAMERA
//****************************************************

Camera::Camera() {
	eye_x = eye_y = eye_z = 0.0;
	width = height = 0;
}

Camera::Camera(float x, float y, float z, int w, int h, float llx, float lly, float llz, float lrx, float lry, float lrz, float ulx, float uly, float ulz, float urx, float ury, float urz) {
	eye_x = x; eye_y = y; eye_z = z;
	width = w;
	height = h;
	eye(0) = x;
	eye(1) = y;
	eye(2) = z;
	eye(3) = 1;
	ll(0) = llx;
	ll(1) = lly;
	ll(2) = llz;
	ll(3) = 1;
	lr(0) = lrx;
	lr(1) = lry;
	lr(2) = lrz;
	lr(3) = 1;
	ul(0) = ulx;
	ul(1) = uly;
	ul(2) = ulz;
	ul(3) = 1;
	ur(0) = urx;
	ur(1) = ury;
	ur(2) = urz;
	ur(3) = 1;
	plane_width = lrx - llx;
	plane_height = uly - lly;
	scale_w = plane_width / width;
	scale_h = plane_height / height;
}

// TODO
void Camera::generateRay(Sample sample, Ray *ray) {
	float x = sample.getX();
	float y = sample.getY();
	float z = 0;	// TODO: Is this 0? or is this...something else?
	x = x * scale_w + ll[0];
	y = y * scale_h + ll[1];
	// Vector4f eye_vec(eye_x, eye_y, eye_z, 1);	// See if we can abstract this out to class var to avoid reconstructing everytime. (Done!)
	Vector4f pixel_vec(x, y, z, 1);
	// Vector4f dir_vec = pixel_vec - eye_vec;
	ray->setEye(eye);	// TODO: fix - redundant
	ray->setDir(pixel_vec);
}





//****************************************************
// RAYTRACER
//****************************************************

RayTracer::RayTracer() {

}

// void RayTracer::trace(Ray& ray, int depth, Color* color) {

// }

void RayTracer::trace(Ray ray, Sample *sample, Shape &shape) {
	if (!shape.intersect(ray)) {
		sample->setColor(0.0);
	} else {
		sample->setColor(243);
	}
}



//****************************************************
// SCENE
//****************************************************

Scene::Scene(Sampler &s, Film& f, Camera &c, RayTracer &rt) {
	sampler = s;
	film = f;
	camera = c;
	raytracer = rt;
}

void Scene::addShape(Shape &shape) {
	shapes.push_back(&shape);
}

void Scene::render() {
	Sample sample;
	Ray ray;
	bool notDone = sampler.getNextSample(&sample);
	while (notDone) {
		camera.generateRay(sample, &ray);
		//shapes[0]->isShape();
		raytracer.trace(ray, &sample, *shapes[0]); 	// TODO: should be Color class instead of Sample, but hack it
	    // for (int i = 0; i < 3; i++) {
	    	// film.setPixel(sample.getX(), sample.getY(), 0, i, sample.getColor());
	    // }
    	film.setPixel(sample.getX(), sample.getY(), 0, 0, sample.getColor());
	    notDone = sampler.getNextSample(&sample);
  	}
  	film.displayToScreen();
}

// bool scene::intersect(Ray &r, double &closest_t, 
//   GeometryProperties &geom_prop, MaterialProperties &mat_prop);
// {
//   closest_t = std::numeric_limits<double>::infinity();
//   std::vector<Primitive> relevant_prims = 
//     acceleration_root.relevant_prims(r);
//   for(std::vector<Primitive> prim_it = relevant_prims.begin();
//     prim_it != relevant_prims.end(); ++prim_it)
//   {
//     Primitive cur_prim = *prim_it;
//     ...
//   }
// }
















