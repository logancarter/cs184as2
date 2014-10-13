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

// Ray::Ray(float pos_x, float pos_y, float pos_z, float dir_x, float dir_y, float dir_z) {
// 	pos = Vector4f
// }






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
	eye.assign(3, 0);
	ll.assign(3, 0);
}

Camera::Camera(float x, float y, float z, int w, int h, float llx, float lly, float llz, float lrx, float lry, float lrz, float ulx, float uly, float ulz, float urx, float ury, float urz) {
	eye_x = x; eye_y = y; eye_z = z;
	width = w;
	height = h;
	eye.clear();
	eye.push_back(x);
	eye.push_back(y);
	eye.push_back(z);
	ll.push_back(llx);
	ll.push_back(lly);
	ll.push_back(llz);
	lr.push_back(lrx);
	lr.push_back(lry);
	lr.push_back(lrz);
	ul.push_back(ulx);
	ul.push_back(uly);
	ul.push_back(ulz);
	ur.push_back(urx);
	ur.push_back(ury);
	ur.push_back(urz);
	plane_width = lrx - llx;
	plane_height = uly - lly;
	scale_w = plane_width / width;
	scale_h = plane_height / height;
}

// TODO
void Camera::generateRay(Sample &sample, Ray *ray) {
	float x = sample.getX();
	float y = sample.getY();
	float z = 0;
	x = x * scale_w + ll[0];
	y = y * scale_h + ll[1];
	Vector4f eye_vec(eye_x, eye_y, eye_z, 1);	// See if we can abstract this out to class var to avoid reconstructing everytime.
	Vector4f pixel_vec(x, y, z, 1);
	// Vector4f dir_vec = pixel_vec - eye_vec;
	ray->setEye(eye_vec);
	ray->setDir(pixel_vec);
}

//****************************************************
// SCENE
//****************************************************

Scene::Scene(Sampler &s, Film& f) {
	sampler = s;
	film = f;

}

void Scene::render() {
	Sample sample;
	bool notDone = sampler.getNextSample(&sample);
	while (notDone) {
	    for (int i = 0; i < 3; i++) {
	    	film.setPixel(sample.getX(), sample.getY(), 0, i, sample.getColor());
	    }
	    notDone = sampler.getNextSample(&sample);
  	}
  	film.displayToScreen();
}

//****************************************************
// SHAPE
//****************************************************

Shape::Shape() {

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

