#include <cstdlib>
#include <memory>
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>

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


//***************************************************--*
// Some Classes
//****************************************************

class Viewport;

class Viewport {
public:
  int w, h; // width and height
};

//part of algorithm credit goes to stack overflow
vector<string> split(const string &s, char delim, char delim2)
{
  vector<string> elems; 
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim) || getline(ss, item, delim2)) {
    cout << item << " thats item" << endl;
    if (item.compare(" ") != 0 && item.compare("\n") != 0 && !item.empty()) {
      cout << item << " thats item" << endl;
      elems.push_back(item);
    }
  }
  return elems;
}

class Patch {
public:
  Vector3d patch[4][4];
  vector<vector<Vector3d> > mypoints;

  Patch() {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        patch[i][j] = Vector3d(0.0, 0.0, 0.0);
      }
    }
  }
  void printPatch() {
    for (int i = 0; i < 4; i++) {
        cout << patch[i][0].transpose() << " " << patch[i][1].transpose() << " " <<  patch[i][2].transpose() << " " <<  patch[i][3].transpose() << " " << endl;
    }
  }
  void setPoints(vector<vector<Vector3d> > somepoints) {
    mypoints = somepoints;
  }
  vector<vector<Vector3d> > getPoints() {
    return mypoints;
  }
};

class Triangle {
public:
  Vector3d points[3];
  Vector2d uvpoints[6];
  Triangle(Vector3d x, Vector3d y, Vector3d z) {
    points[0] = x;
    points[1] = y;
    points[2] = z;
  }
  void setUV(Vector2d one, Vector2d two, Vector2d three, Vector2d four, Vector2d five, Vector2d six) {
    cout << uvpoints[0].x() << " X " << uvpoints[0].y() << endl;
    if (one.x() != abs(one.x()) or one.y() != abs(one.y())) {
      cout << "SSOMETHIGNS WRONG" << endl;
    }
    uvpoints[0]= one;
    uvpoints[1]= two;
    uvpoints[2]= three;
    uvpoints[3]= four;
    uvpoints[4]= five;
    uvpoints[5]= six;
  }
  Vector3d side1() {
    return points[0];
  }
  Vector3d side2() {
    return points[1];
  }
  Vector3d side3() {
    return points[2];
  }
  void drawit() {
    glVertex3d(points[0].x(), points[0].y(), points[0].z());
    glVertex3d(points[1].x(), points[1].y(), points[1].z());
    glVertex3d(points[2].x(), points[2].y(), points[2].z());
  }
};

Vector3d midpoint (Vector3d one, Vector3d two) {
  return (one + two) / 2.000;
}
Vector2d midpoint (Vector2d one, Vector2d two) {
  cout << "HIII";
  return (one + two) / 2.000;
}

 Vector3d bezcurveinterp(Vector3d* zero, Vector3d* one, Vector3d* two, Vector3d* three, double u, Vector3d *dPdu) {
  // cout << "bezcurve " << (*zero).transpose() << " " << (*one).transpose() << " " << (*two).transpose() << " " << (*three).transpose() << " U_step " << u << endl;
  Vector3d a = *zero * (1.0 - u) + *one * u;
  Vector3d b = *one * (1.0 - u) + *two * u;
  Vector3d c = *two * (1.0 - u) + *three * u;

  Vector3d d = a * (1.0 - u) + b * u;
  Vector3d e = b * (1.0 - u) + c * u;

  Vector3d p = d * (1.0 - u) + e * u;

  Vector3d fordpdu = 3 * (e - d);
  *dPdu << fordpdu[0], fordpdu[1], fordpdu[2];

  //cout << "bezcurve returns " << p.transpose() << endl;
  return p;
}


Vector3d bezpatchinterp(Patch &patch, double &u, double &v, Vector3d* n) {
  //cout << "bezpatch u: " << u << " v: " << v << endl;
  vector<Vector3d> vcurve;
  vector<Vector3d> ucurve;
  // Vector3d p = *(new Vector3d(0.0,0.0,0.0));
  Vector3d p;
  Vector3d *dPdu = new Vector3d(0.0,0.0,0.0);
  Vector3d *dPdv = new Vector3d(0.0,0.0,0.0);
  // cout << "first row: " << patch.patch[0][0].transpose() << " " << patch.patch[0][1].transpose() << " " << patch.patch[0][2].transpose() << " " << patch.patch[0][3].transpose() << endl;
  vcurve.push_back(bezcurveinterp(&patch.patch[0][0], &patch.patch[0][1], &patch.patch[0][2], &patch.patch[0][3], u, dPdu));
  vcurve.push_back(bezcurveinterp(&patch.patch[1][0], &patch.patch[1][1], &patch.patch[1][2], &patch.patch[1][3], u, dPdu));
  vcurve.push_back(bezcurveinterp(&patch.patch[2][0], &patch.patch[2][1], &patch.patch[2][2], &patch.patch[2][3], u, dPdu));
  vcurve.push_back(bezcurveinterp(&patch.patch[3][0], &patch.patch[3][1], &patch.patch[3][2], &patch.patch[3][3], u, dPdu));

  ucurve.push_back(bezcurveinterp(&patch.patch[0][0], &patch.patch[1][0], &patch.patch[2][0], &patch.patch[3][0], v, dPdu));
  ucurve.push_back(bezcurveinterp(&patch.patch[0][1], &patch.patch[1][1], &patch.patch[2][1], &patch.patch[3][1], v, dPdu));
  ucurve.push_back(bezcurveinterp(&patch.patch[0][2], &patch.patch[1][2], &patch.patch[2][2], &patch.patch[3][2], v, dPdu));
  ucurve.push_back(bezcurveinterp(&patch.patch[0][3], &patch.patch[1][3], &patch.patch[2][3], &patch.patch[3][3], v, dPdu));

  //cout << vcurve[0].transpose() << " "<< vcurve[1].transpose() << " "<< vcurve[2].transpose() << " " << vcurve[3].transpose() << endl;
  Vector3d v_point = bezcurveinterp(&vcurve[0], &vcurve[1], &vcurve[2], &vcurve[3], v, dPdv);
  //cout << "v curve: " << v_point.transpose() << endl;
  // cout << "p " << p.transpose() << endl;
  Vector3d u_point = bezcurveinterp(&ucurve[0], &ucurve[1], &ucurve[2], &ucurve[3], u, dPdu);
  // cout << "p " << p.transpose() << endl;
  //cout << "u curve: " << u_point.transpose() << endl;
  p = v_point;

  // cout << "dpdu " << *dPdu << endl;
  // cout << "dpdv " << *dPdv << endl;
  cout << u << " u" << v << " v" << endl;
  Vector3d n2 = (*dPdu).cross(*dPdv);

  // cout << "n2 : " << n2 << endl;

  *n = n2 / n2.norm();
  return p;
}


void subdividepatch(Patch &patch, double step) {
  Vector3d* n = new Vector3d(0.0, 0.0, 0.0);
  //GLfloat epsilon = step - fmod(1.0, step);
  int numdiv;
  if (fmod(1.0, step) == 0) {
    numdiv = 1.0 / step;
  } else {
    numdiv = 1 / step;
  }
  //numdiv = ((1 + epsilon) / step);
  //cout << "numdiv " << numdiv << endl;
  vector<vector<Vector3d > >allpoints;
  for (int iu = 0; iu <= numdiv; iu++) {
    vector<Vector3d> onepoint;
    double u = iu * step;
    cout << "iu " << iu << endl;
    for (int iv = 0; iv <= numdiv; iv++) {
      cout << "iv " << iv << endl;
      double v = iv * step;
      // cout << "subdivide: u: " << u << " v: " << v << endl;
      Vector3d p = bezpatchinterp(patch, u, v, n);
      //TODO:save surface point and normal      // cout << *n << endl;
      cout << "---- end -----" << endl;
      onepoint.push_back(p);
    }
    allpoints.push_back(onepoint);
  }
  patch.setPoints(allpoints);
}

bool lessthan(Vector3d somevector, double eps) {
  return somevector.x() < eps and somevector.y() < eps and somevector.z() < eps;
}


//****************************************************
// Global Variables
//****************************************************

Viewport viewport;
double sub_div_param;
bool adaptive = false;      //if true: adaptive; if false: uniform
std::vector< Patch * > patchez;
//vector<Vector3f> somepoints_toconnect;
//vector<Vector3f> toconnect;
float zoomamount = 1.0;
float horizontalshift = 0.0;
float verticalshift = 0.0;
float rotatehoriz = 0.0;
float rotatevertical = 0.0;
vector<vector<Vector3d*> > curves;
//vector<vector<Vector3f> > pointsofcurves;
bool wireframe = true;


void checkandDivide(Triangle* t1, int i) {
    Vector3d e1 = midpoint(t1->side3(), t1->side1());
    Vector3d e2 = midpoint(t1->side1(), t1->side2());
    Vector3d e3 = midpoint(t1->side2(), t1->side3());
    GLfloat zero = 0;
    Vector3d dont;
    Vector3d p1 = bezpatchinterp(*patchez[i], t1->uvpoints[0].x(), t1->uvpoints[0].y(), &dont);
    Vector3d p2 = bezpatchinterp(*patchez[i], t1->uvpoints[2].x(), t1->uvpoints[2].y(), &dont);
    Vector3d p3 = bezpatchinterp(*patchez[i], t1->uvpoints[4].x(), t1->uvpoints[4].y(), &dont);
    Vector3d diff2 = (p2 - e2).cwiseAbs();
    Vector3d diff3 = (p3 - e3).cwiseAbs();
    Vector3d diff1 = (p1 - e1).cwiseAbs();
    if (lessthan(diff1, sub_div_param) and lessthan(diff2, sub_div_param) and lessthan(diff3, sub_div_param)) {
      cout << "YESSSSSSSSSSS" << endl;
      t1->drawit();//base case yaay
    } else if (!lessthan(diff1, sub_div_param) and !lessthan(diff2, sub_div_param) and !lessthan(diff3, sub_div_param)) {
      //call this function on four new triangles
      cout << "one" << endl;
      Triangle *newtri = new Triangle(t1->side1(), p2, p1);
      newtri->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[1]),t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[2]),t1->uvpoints[2], midpoint(t1->uvpoints[0], t1->uvpoints[2]), t1->uvpoints[0]);
      
      Triangle *newtri2 = new Triangle(p1, p2, p3);
      newtri2->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[4]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[2]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[4]), t1->uvpoints[4]);
      
      Triangle *newtri3 = new Triangle(p2, t1->side2(), p3);
      newtri3->setUV(midpoint(t1->uvpoints[2], t1->uvpoints[4]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[3]), t1->uvpoints[3], midpoint(t1->uvpoints[3], t1->uvpoints[4]), t1->uvpoints[4]);
      
      Triangle *newtri4 = new Triangle(p1, p3, t1->side3());
      newtri4->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[5]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[4]), t1->uvpoints[4], midpoint(t1->uvpoints[4], t1->uvpoints[5]), t1->uvpoints[5]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
      checkandDivide(newtri3, i);
      checkandDivide(newtri4, i);
    } else if (!lessthan(diff1, sub_div_param) and lessthan(diff2, sub_div_param) and !lessthan(diff3, sub_div_param)) {
      //call this function on three new triangles
      cout << "two" << endl;
      Triangle *newtri = new Triangle(t1->side1(), t1->side2(), p1);
      newtri->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[1]), t1->uvpoints[1], t1->uvpoints[2], t1->uvpoints[3], midpoint(t1->uvpoints[0], t1->uvpoints[3]), t1->uvpoints[0]);
      Triangle *newtri2 = new Triangle(p1, t1->side2(), p3);
      newtri2->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[4]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[3]), t1->uvpoints[3], midpoint(t1->uvpoints[3], t1->uvpoints[4]), t1->uvpoints[4]);
      Triangle *newtri3 = new Triangle(p1, p3, t1->side3());
      newtri3->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[5]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[4]), t1->uvpoints[4], midpoint(t1->uvpoints[4], t1->uvpoints[5]), t1->uvpoints[5]);
      //t1->drawit();
      // newtri->drawit();
      // newtri2->drawit();
      // newtri3->drawit();
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
      checkandDivide(newtri3, i);
    } else if(lessthan(diff1, sub_div_param) and !lessthan(diff2, sub_div_param) and !lessthan(diff3, sub_div_param)) {
      //call this function on three new triangles
      cout << "three" << endl;
      Triangle *newtri = new Triangle(t1->side1(), p2, p3);
      // cout << "h" << endl;
      // cout << midpoint(t1->uvpoints[1], t1->uvpoints[4]);
      // cout << t1->uvpoints[1];
      // cout << midpoint(t1->uvpoints[1], t1->uvpoints[2]);
      // cout << t1->uvpoints[2];
      // cout << midpoint(t1->uvpoints[2], t1->uvpoints[4]);
      // cout << t1->uvpoints[4];
      newtri->setUV(midpoint(t1->uvpoints[1], t1->uvpoints[4]), t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[2]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[4]), t1->uvpoints[4]);
      cout << "b" << endl;
      Triangle *newtri2 = new Triangle(t1->side1(), p3, t1->side3());
      cout << "d" << endl;
      newtri2->setUV(t1->uvpoints[0], t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[4]), t1->uvpoints[4], midpoint(t1->uvpoints[5], t1->uvpoints[4]), t1->uvpoints[5]);
      cout << "a" << endl;
      Triangle *newtri3 = new Triangle(p2, t1->side2(), p3);
      cout << "y" << endl;
      newtri3->setUV(midpoint(t1->uvpoints[2], t1->uvpoints[4]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[3]), t1->uvpoints[3], midpoint(t1->uvpoints[3], t1->uvpoints[4]), t1->uvpoints[4]);
     // newtri->drawit();
    //  newtri2->drawit();
    //  newtri3->drawit();
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
      checkandDivide(newtri3, i);
    } else if (!lessthan(diff1, sub_div_param) and !lessthan(diff2, sub_div_param) and lessthan(diff3, sub_div_param)) {
      //call this function on three new triangles
      cout << "four" << endl;
      Triangle *newtri = new Triangle(p2, t1->side2(), t1->side3());
      newtri->setUV(midpoint(t1->uvpoints[2], t1->uvpoints[5]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[3]), t1->uvpoints[3], t1->uvpoints[4], t1->uvpoints[5]);
      Triangle *newtri2 = new Triangle(p1, p2, t1->side3());
      newtri2->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[5]),t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[2]),t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[5]), t1->uvpoints[5]);
      Triangle *newtri3 = new Triangle(t1->side1(), p2, p1);
      newtri3->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[1]),t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[2]),t1->uvpoints[2], midpoint(t1->uvpoints[0], t1->uvpoints[2]), t1->uvpoints[0]);
      // newtri->drawit();
      // newtri2->drawit();
      // newtri3->drawit();
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
      checkandDivide(newtri3, i);
    } else if (lessthan(diff1, sub_div_param) and lessthan(diff2, sub_div_param) and !lessthan(diff3, sub_div_param)) {
      //call this function on two new triangles
      cout << "five" << endl;
      Triangle *newtri = new Triangle(t1->side1(), p3, t1->side3());
      newtri->setUV(t1->uvpoints[0], t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[4]), t1->uvpoints[4], midpoint(t1->uvpoints[4], t1->uvpoints[5]), t1->uvpoints[5]);
      Triangle *newtri2 = new Triangle(t1->side1(), t1->side2(), p3);
      newtri2->setUV(midpoint(t1->uvpoints[1], t1->uvpoints[4]), t1->uvpoints[1], t1->uvpoints[2], t1->uvpoints[3], midpoint(t1->uvpoints[3], t1->uvpoints[4]), t1->uvpoints[4]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
    } else if (lessthan(diff1, sub_div_param) and !lessthan(diff2, sub_div_param) and lessthan(diff3, sub_div_param)) {
      //call this function on two new triangles
                  cout << "six" << endl;
      Triangle *newtri = new Triangle(t1->side1(), p2, t1->side3());
      newtri->setUV(t1->uvpoints[0], t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[2]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[5]), t1->uvpoints[5]);
      Triangle *newtri2 = new Triangle(p2, t1->side2(), t1->side3());
      newtri2->setUV(midpoint(t1->uvpoints[5], t1->uvpoints[2]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[3]), t1->uvpoints[3], t1->uvpoints[4], t1->uvpoints[5]);
     // newtri->drawit();
     // newtri2->drawit();
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
    } else if (!lessthan(diff1, sub_div_param) and lessthan(diff2, sub_div_param) and lessthan(diff3, sub_div_param)) {
      //call this function on two new triangles
      cout << "seven" << endl;
      Triangle *newtri = new Triangle(t1->side1(), t1->side2(), p1);
      newtri->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[1]), t1->uvpoints[1], t1->uvpoints[2], t1->uvpoints[3], midpoint(t1->uvpoints[0], t1->uvpoints[3]), t1->uvpoints[0]);
      Triangle *newtri2 = new Triangle(p1, t1->side2(), t1->side3());
      newtri2->setUV(midpoint(t1->uvpoints[5], t1->uvpoints[0]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[3]), t1->uvpoints[3], t1->uvpoints[4], t1->uvpoints[5]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
    } else {
      cout << "SHOULDN'T get here" << endl;
    }
}



//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1, 1, -1, 1, 1, -1);    // resize type = stretch

}

//****************************************************
// Simple init function
//****************************************************
void initScene(){

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
  myReshape(viewport.w,viewport.h);

}


//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);       // clear the color buffer

  glMatrixMode(GL_MODELVIEW);        // indicate we are specifying camera transformations
  glLoadIdentity();        // make sure transformation is "zero'd"

  glColor3f(1.0f,0.0f,0.0f);                   // setting the color to pure red 90% for the rect
  //gluLookAt(-2, 0, 0, 0, 0,-1, 0, 1, 0);
  glTranslatef(horizontalshift, verticalshift, 0.0);
  glRotatef(rotatevertical, 1, 0, 0);
  glRotatef(rotatehoriz, 0, 1, 0);
  glScalef(zoomamount, zoomamount, zoomamount);

  glLineWidth(1);
  if (wireframe) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glColor3f(1.0f,0.0f,0.0f); 
  if (!adaptive) {//uniform
    for (int i = 1; i < patchez.size(); i++) {
      int numdiv = patchez[i]->getPoints().size() - 1;
      //cout << numdiv << " NUMDIV" << endl;
      for (int j = 0; j < numdiv + 1; j ++) {
        for (int k = 0; k < numdiv + 1; k ++) {
          if (k != numdiv and j != numdiv) {
            glBegin(GL_QUADS);
            glColor3f(1.0f,1.0f,0.0f); 
            glVertex3d(patchez[i]->getPoints()[j][k].x(), patchez[i]->getPoints()[j][k].y(), patchez[i]->getPoints()[j][k].z());
            glVertex3d(patchez[i]->getPoints()[j][k + 1].x(), patchez[i]->getPoints()[j][k + 1].y(), patchez[i]->getPoints()[j][k + 1].z());
            glVertex3d(patchez[i]->getPoints()[j + 1][k + 1].x(), patchez[i]->getPoints()[j+1][k + 1].y(), patchez[i]->getPoints()[j+1][k + 1].z());
            glVertex3d(patchez[i]->getPoints()[j + 1][k].x(), patchez[i]->getPoints()[j + 1][k].y(), patchez[i]->getPoints()[j + 1][k].z());
          }
        }
      }
    }
  } else {//adaptive
    vector<Vector3d> midpoints;
    for (int i = 0; i < patchez.size(); i++) {
      glBegin(GL_TRIANGLES);
        Triangle *t1 = new Triangle(patchez[i]->getPoints()[0][1], patchez[i]->getPoints()[1][1], patchez[i]->getPoints()[0][0]);
        Vector2d onev = *(new Vector2d(0.0, 0.5));
        Vector2d twov = *( new Vector2d(0.0, 1.0));
        Vector2d threev = *(new Vector2d(0.5, 1.0));
        Vector2d fourv = *(new Vector2d(1.0, 1.0));
        Vector2d fivev = *(new Vector2d(0.5, 0.5));
        Vector2d sixv = *(new Vector2d(0.0, 0.0));
        t1->setUV(onev, twov, threev, fourv, fivev, sixv);
        checkandDivide(t1, i);
        Triangle *t2 = new Triangle(patchez[i]->getPoints()[0][0], patchez[i]->getPoints()[1][1], patchez[i]->getPoints()[1][0]);
        Vector2d onev2 = *(new Vector2d(0.5, 0.0));
        Vector2d twov2 = *( new Vector2d(0.0, 0.0));
        Vector2d threev2 = *(new Vector2d(0.5, 0.5));
        Vector2d fourv2 = *(new Vector2d(1.0, 1.0));
        Vector2d fivev2 = *(new Vector2d(1.0, 0.5));
        Vector2d sixv2 = *(new Vector2d(1.0, 0.0));
        t2->setUV(onev2, twov2, threev2, fourv2, fivev2, sixv2);
        checkandDivide(t2, i);
    }
  }
  glEnd();
  glFlush();
  glutSwapBuffers();        // swap buffers (we earlier set double buffer)

}

//****************************************************
// called by glut when there are no messages to handle
//****************************************************
void myFrameMove() {
  //nothing here for now
#ifdef _WIN32
  Sleep(10);                                 //give ~10ms back to OS (so as not to waste the CPU)
#endif
  glutPostRedisplay();                       // forces glut to call the display function (myDisplay())
}


//****************************************************
// function that handles keyboard callback
//****************************************************
void myKeyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 0x20:
    cout << "space: exit" << endl;
    exit(0);
    break;
  case 's':
   cout << "s: toggle flat/smooth" << endl;
   break;
  case 'w':
   cout << "w: toggle filled/wireframe" << endl;
   wireframe = !wireframe;
   break;
  // TODO: check if its = or + (shift)
  case '=':
    zoomamount += .1;
    glutPostRedisplay();
    cout << "+: zoom in" << endl;
    break;
  case '-':
    zoomamount -= .1;
    glutPostRedisplay();
    cout << "-: zoom out" << endl;
    break;
  default:
    cout << "unidentified " << key << endl;
    break;

  }

}

void specialKeys(int key, int x, int y) {
  int mod = glutGetModifiers();
  if (mod == GLUT_ACTIVE_SHIFT) {
    switch (key) {
    case GLUT_KEY_UP:
      verticalshift += .1;
      glutPostRedisplay();
      cout << "translate: up" << endl;
      break;
    case GLUT_KEY_DOWN:
      verticalshift -= .1;
      glutPostRedisplay();
      cout << "translate: down" << endl;
      break;
    case GLUT_KEY_RIGHT:
      horizontalshift += .1;
      glutPostRedisplay();
      cout << "translate: right" << endl;
      break;
    case GLUT_KEY_LEFT:
      glutPostRedisplay();
      horizontalshift -= .1;
      cout << "translate: left" << endl;
      break;
    default:
      cout << "shift special " << key << endl;
      break;
    }
  } else {
    switch (key) {
    case GLUT_KEY_UP:
      rotatevertical += 10;
      glutPostRedisplay();
      cout << "rotate: up" << endl;
      break;
    case GLUT_KEY_DOWN:
      rotatevertical -= 10;
      glutPostRedisplay();
      cout << "rotate: down" << endl;
      break;
    case GLUT_KEY_RIGHT:
      rotatehoriz += 10;
      glutPostRedisplay();
      cout << "rotate: right" << endl;
      break;
    case GLUT_KEY_LEFT:
      rotatehoriz -= 10;
      glutPostRedisplay();
      cout << "rotate: left" << endl;
      break;
    default:
      cout << "special " << key << endl;
      break;
    }
  }
}

//credit goes to stack overflow for this function
bool isEmptyOrBlank(const std::string& str) {
   int len = str.size();
   if(len == 0) { return true; }

   for(int i=0;i<len;++i) {
       if(str[i] != ' ' and str[i] != '\n' and str[i] != '\t' and str[i] != '\r') { return false; }
   }
   return true;
}



//****************************************************
// MAIN
//****************************************************
int main(int argc, char *argv[]) {

  //*******************************
  // ARGUMENT PARSER
  //*******************************
  string STRING;
  ifstream infile;
  cout << argv[1] << endl;
  infile.open (argv[1]);
  int numpatches = 0;
  if (!infile.eof()) {
    getline(infile,STRING);
    numpatches = atoi(STRING.c_str());
  }

  if (numpatches == 0) return 1; // TODO: throw an exception somehow]
  
  // TODO: Assume input is well-formed!!!
  for (int i = 0; i < numpatches; i++) {
    Patch* current_patch = new Patch();
    int line = 0;


    // TODO-fix: extra lines at end of file? check if STRING is empty?
    while(!infile.eof() && line < 4) {      // Each iteration is one LINE <<<<<<
      getline(infile,STRING);

      if (isEmptyOrBlank(STRING)) {
        continue;
      }

      cout << STRING << endl;
      double a, b, c, d, e, f, g, h, i, j, k, l;
      std::istringstream iss (STRING);
      iss >> std::skipws >> a >> b >> c;
      iss >> std::skipws >> d >> e >> f;
      iss >> std::skipws >> g >> h >> i;
      iss >> std::skipws >> j >> k >> l;

      vector<Vector3d*> curve;
      Vector3d *apoint = new Vector3d(a, b, c);
      Vector3d *bpoint = new Vector3d(d, e, f);
      Vector3d *cpoint = new Vector3d(g, h, i);
      Vector3d *dpoint = new Vector3d(j, k, l);

      curve.push_back(apoint);
      curve.push_back(bpoint);
      curve.push_back(cpoint);
      curve.push_back(dpoint);
      curves.push_back(curve);

      current_patch->patch[line][0] = *apoint;
      current_patch->patch[line][1] = *bpoint;
      current_patch->patch[line][2] = *cpoint;
      current_patch->patch[line][3] = *dpoint;

      line++;

    } // while

    cout << "/~~~~~~~~~ end patch ~~~~~~~~~~/" << endl;
    // getline(infile, STRING);
    patchez.push_back(current_patch);

  } // for


  string strsubp = argv[2];
  sub_div_param = atof(strsubp.c_str());
  cout << sub_div_param << endl;
  if (argc > 3) {
    string nextarg;
    nextarg = argv[3];
    if (nextarg.compare("-a") == 0) {
      adaptive = true;
      cout << "adaptive" << endl;
    }
  } else {
    cout << "not adaptive" << endl;
  }
  infile.close();

  Vector3f normal;
  for (int k = 0; k < patchez.size(); k++) {

    // Uniform
    if (!adaptive) {
      subdividepatch(*patchez[k], sub_div_param);
    } else {
      subdividepatch(*patchez[k], 1.0);
    }
  }


  //*******************************
  // GLUT STUFF
  //*******************************

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 500;
  viewport.h = 500;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();      // quick function to set up scene

  glutDisplayFunc(myDisplay);     // function to run when its time to draw something

  glutReshapeFunc(myReshape);     // function to run when the window gets resized
  glutIdleFunc(myFrameMove);
  glutKeyboardFunc(myKeyboard);           // function to run when spacebar is pressed: should exit
  glutSpecialFunc(specialKeys);

  glutMainLoop();   // infinite loop that will keep drawing and resizing

  return 0;
}









