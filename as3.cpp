#include <cstdlib>
#include <memory>
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <limits>
//#include <math>

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


//****************************************************
// HELPER FUNCTIONS
//****************************************************

/* part of algorithm credit goes to stack overflow */
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


//****************************************************
// PATCH
//****************************************************

class Patch {
public:
  Vector3f patch[4][4];
  vector<vector<Vector3f> > mypoints;

  Patch() {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        patch[i][j] = Vector3f(0.0, 0.0, 0.0);
      }
    }
  }

  void printOldPatch() {
    cout << "old patch" << endl;
    for (int i = 0; i < 4; i++) {
      cout << patch[i][0].transpose() << " " << patch[i][1].transpose() << " " <<  patch[i][2].transpose() << " " <<  patch[i][3].transpose() << " " << endl;
    }
  }

  void printNewPatch() {
    cout << "new patch" << endl;
    for (int i = 0; i < mypoints.size(); i++) {
      // string result = "";
      for (int j = 0; j < mypoints[0].size(); j++) {
          std::cout << mypoints[i][j].transpose() << endl; 
       } 
       std::cout << "endline" << endl;
    } 
    std::cout << "DONE!" << endl;
  }

  void setPoints(vector<vector<Vector3f> > somepoints) {
    // printOldPatch();
    mypoints = somepoints;
    // printNewPatch();
  }

  vector<vector<Vector3f> > getPoints() {
    return mypoints;
  }

};

class Triangle {
public:
  Vector3f points[3];
  Vector3f normals[3];
  Vector2f uvpoints[6];
  Triangle(Vector3f x, Vector3f y, Vector3f z) {
    points[0] = x;
    points[1] = y;
    points[2] = z;
  }
  void setUV(Vector2f one, Vector2f two, Vector2f three, Vector2f four, Vector2f five, Vector2f six) {
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
  Vector3f side1() {
    return points[0];
  }
  Vector3f side2() {
    return points[1];
  }
  Vector3f side3() {
    return points[2];
  }
  void drawit() {
    glNormal3f(normals[0].x(), normals[0].y(), normals[0].z());
    glVertex3f(points[0].x(), points[0].y(), points[0].z());
    glNormal3f(normals[1].x(), normals[1].y(), normals[1].z());
    glVertex3f(points[1].x(), points[1].y(), points[1].z());
    glNormal3f(normals[2].x(), normals[2].y(), normals[2].z());
    glVertex3f(points[2].x(), points[2].y(), points[2].z());
  }

  void setNormals(Vector3f x, Vector3f y, Vector3f z) {
    normals[0] = x; normals[1] = y; normals[2] = z;
  }
};

  Vector3f midpoint (Vector3f one, Vector3f two) {
    return (one + two) / 2.000;
  }
  Vector2f midpoint (Vector2f one, Vector2f two) {
    return (one + two) / 2.000;
  }


 Vector3f bezcurveinterp(Vector3f* zero, Vector3f* one, Vector3f* two, Vector3f* three, GLfloat u, Vector3f *dPdu) {
  // cout << "bezcurve " << (*zero).transpose() << " " << (*one).transpose() << " " << (*two).transpose() << " " << (*three).transpose() << " U_step " << u << endl;
  Vector3f a = *zero * (1.0 - u) + *one * u;
  Vector3f b = *one * (1.0 - u) + *two * u;
  Vector3f c = *two * (1.0 - u) + *three * u;

  Vector3f d = a * (1.0 - u) + b * u;
  Vector3f e = b * (1.0 - u) + c * u;

  Vector3f p = d * (1.0 - u) + e * u;

  Vector3f fordpdu = 3 * (e - d);
  *dPdu << fordpdu[0], fordpdu[1], fordpdu[2];

  //cout << "bezcurve returns " << p.transpose() << endl;
  return p;
}


Vector3f bezpatchinterp(Patch &patch, float &u, float &v, Vector3f* n) {
  vector<Vector3f> vcurve;
  vector<Vector3f> ucurve;
  Vector3f p;
  Vector3f *dPdu = new Vector3f(0.0,0.0,0.0);
  Vector3f *dPdv = new Vector3f(0.0,0.0,0.0);
  vcurve.push_back(bezcurveinterp(&patch.patch[0][0], &patch.patch[0][1], &patch.patch[0][2], &patch.patch[0][3], u, dPdu));
  vcurve.push_back(bezcurveinterp(&patch.patch[1][0], &patch.patch[1][1], &patch.patch[1][2], &patch.patch[1][3], u, dPdu));
  vcurve.push_back(bezcurveinterp(&patch.patch[2][0], &patch.patch[2][1], &patch.patch[2][2], &patch.patch[2][3], u, dPdu));
  vcurve.push_back(bezcurveinterp(&patch.patch[3][0], &patch.patch[3][1], &patch.patch[3][2], &patch.patch[3][3], u, dPdu));

  ucurve.push_back(bezcurveinterp(&patch.patch[0][0], &patch.patch[1][0], &patch.patch[2][0], &patch.patch[3][0], v, dPdu));
  ucurve.push_back(bezcurveinterp(&patch.patch[0][1], &patch.patch[1][1], &patch.patch[2][1], &patch.patch[3][1], v, dPdu));
  ucurve.push_back(bezcurveinterp(&patch.patch[0][2], &patch.patch[1][2], &patch.patch[2][2], &patch.patch[3][2], v, dPdu));
  ucurve.push_back(bezcurveinterp(&patch.patch[0][3], &patch.patch[1][3], &patch.patch[2][3], &patch.patch[3][3], v, dPdu));

  Vector3f v_point = bezcurveinterp(&vcurve[0], &vcurve[1], &vcurve[2], &vcurve[3], v, dPdv);
  Vector3f u_point = bezcurveinterp(&ucurve[0], &ucurve[1], &ucurve[2], &ucurve[3], u, dPdu);
  p = v_point;
  Vector3f n2 = (*dPdu).cross(*dPdv);
  *n = n2 / n2.norm();
  return p;
}


void subdividepatch(Patch &patch, float step) {
  Vector3f* n = new Vector3f(0.0, 0.0, 0.0);
  int numdiv;
  if (fmod(1.0, step) == 0) {
    numdiv = 1.0 / step;
  } else {
    numdiv = 1 / step;
  }
  vector<vector<Vector3f > >allpoints;
  for (int iu = 0; iu <= numdiv; iu++) {
    vector<Vector3f> onepoint;
    float u = iu * step;
    for (int iv = 0; iv <= numdiv; iv++) {
      float v = iv * step;
      Vector3f p = bezpatchinterp(patch, u, v, n);
      onepoint.push_back(p);
    }
    allpoints.push_back(onepoint);
  }
  //cout << patch.getPoints()[0] << " before" << endl;
  patch.setPoints(allpoints);
  cout << patch.getPoints()[0][0] << " after" << endl;
}

bool lessthan(Vector3f somevector, float eps) {
  return somevector.x() < eps and somevector.y() < eps and somevector.z() < eps;
}


//****************************************************
// Global Variables
//****************************************************

Viewport viewport;
float sub_div_param;
bool adaptive = false;      //if true: adaptive; if false: uniform
std::vector< Patch* > patchez;
std::vector< Patch* > normalz;
vector<Vector3f> somepoints_toconnect;
vector<Vector3f> toconnect;
float zoomamount = 1.0;
float horizontalshift = 0.0;
float verticalshift = 0.0;
float rotatehoriz = 0.0;
float rotatevertical = 0.0;
vector<vector<Vector3f*> > curves;
float maxx = std::numeric_limits<float>::min();
float minx = std::numeric_limits<float>::max();
float maxy = std::numeric_limits<float>::min();
float miny = std::numeric_limits<float>::max();
vector<vector<Vector3f> > pointsofcurves;
bool wireframe = true, flat = true;

GLfloat diffuse[]={1.0, 0.0, 0.0, 1.0};
GLfloat ambient[]={0.1, 0.1, 0.1, 1.0};
GLfloat specular[]={1.0, 1.0, 1.0, 1.0};
GLfloat light_pos[]={10.0, 20.0, 30.0, 0.0};


void checkandDivide(Triangle* t1, int i) {
    Vector3f e1 = midpoint(t1->side3(), t1->side1());
    Vector3f e2 = midpoint(t1->side1(), t1->side2());
    Vector3f e3 = midpoint(t1->side2(), t1->side3());
    GLfloat zero = 0;
    Vector3f dont1;
    Vector3f dont2;
    Vector3f dont3;
    Vector3f p1 = bezpatchinterp(*patchez[i], t1->uvpoints[0].x(), t1->uvpoints[0].y(), &dont1);
    Vector3f p2 = bezpatchinterp(*patchez[i], t1->uvpoints[2].x(), t1->uvpoints[2].y(), &dont2);
    Vector3f p3 = bezpatchinterp(*patchez[i], t1->uvpoints[4].x(), t1->uvpoints[4].y(), &dont3);
    Vector3f diff2 = (p2 - e2).cwiseAbs();
    Vector3f diff3 = (p3 - e3).cwiseAbs();
    Vector3f diff1 = (p1 - e1).cwiseAbs();
    if (lessthan(diff1, sub_div_param) and lessthan(diff2, sub_div_param) and lessthan(diff3, sub_div_param)) {
      t1->drawit();//base case
    } else if (!lessthan(diff1, sub_div_param) and !lessthan(diff2, sub_div_param) and !lessthan(diff3, sub_div_param)) {
      //call this function on four new triangles
      Triangle *newtri = new Triangle(t1->side1(), p2, p1);
      newtri->setNormals(t1->normals[0], dont2, dont1);
      newtri->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[1]),t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[2]),t1->uvpoints[2], midpoint(t1->uvpoints[0], t1->uvpoints[2]), t1->uvpoints[0]);
      
      Triangle *newtri2 = new Triangle(p1, p2, p3);
      newtri2->setNormals(dont1, dont2, dont3);
      newtri2->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[4]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[2]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[4]), t1->uvpoints[4]);
      
      Triangle *newtri3 = new Triangle(p2, t1->side2(), p3);
      newtri3->setNormals(dont2, t1->normals[1], dont3);
      newtri3->setUV(midpoint(t1->uvpoints[2], t1->uvpoints[4]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[3]), t1->uvpoints[3], midpoint(t1->uvpoints[3], t1->uvpoints[4]), t1->uvpoints[4]);
      
      Triangle *newtri4 = new Triangle(p1, p3, t1->side3());
      newtri4->setNormals(dont1, dont3, t1->normals[2]);
      newtri4->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[5]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[4]), t1->uvpoints[4], midpoint(t1->uvpoints[4], t1->uvpoints[5]), t1->uvpoints[5]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
      checkandDivide(newtri3, i);
      checkandDivide(newtri4, i);
    } else if (!lessthan(diff1, sub_div_param) and lessthan(diff2, sub_div_param) and !lessthan(diff3, sub_div_param)) {
      //call this function on three new triangles
      Triangle *newtri = new Triangle(t1->side1(), t1->side2(), p1);
      newtri->setNormals(t1->normals[0], t1->normals[1], dont1);
      newtri->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[1]), t1->uvpoints[1], t1->uvpoints[2], t1->uvpoints[3], midpoint(t1->uvpoints[0], t1->uvpoints[3]), t1->uvpoints[0]);
      Triangle *newtri2 = new Triangle(p1, t1->side2(), p3);
      newtri2->setNormals(dont1, t1->normals[1], dont3);
      newtri2->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[4]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[3]), t1->uvpoints[3], midpoint(t1->uvpoints[3], t1->uvpoints[4]), t1->uvpoints[4]);
      Triangle *newtri3 = new Triangle(p1, p3, t1->side3());
      newtri3->setNormals(dont1, dont3, t1->normals[2]);
      newtri3->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[5]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[4]), t1->uvpoints[4], midpoint(t1->uvpoints[4], t1->uvpoints[5]), t1->uvpoints[5]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
      checkandDivide(newtri3, i);
    } else if(lessthan(diff1, sub_div_param) and !lessthan(diff2, sub_div_param) and !lessthan(diff3, sub_div_param)) {
      //call this function on three new triangles
      Triangle *newtri = new Triangle(t1->side1(), p2, p3);
      newtri->setNormals(t1->normals[0], dont2, dont3);
      newtri->setUV(midpoint(t1->uvpoints[1], t1->uvpoints[4]), t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[2]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[4]), t1->uvpoints[4]);
      Triangle *newtri2 = new Triangle(t1->side1(), p3, t1->side3());
      newtri2->setNormals(t1->normals[0], dont3, t1->normals[2]);
      newtri2->setUV(t1->uvpoints[0], t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[4]), t1->uvpoints[4], midpoint(t1->uvpoints[5], t1->uvpoints[4]), t1->uvpoints[5]);
      Triangle *newtri3 = new Triangle(p2, t1->side2(), p3);
      newtri3->setNormals(dont2, t1->normals[1], dont3);
      newtri3->setUV(midpoint(t1->uvpoints[2], t1->uvpoints[4]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[3]), t1->uvpoints[3], midpoint(t1->uvpoints[3], t1->uvpoints[4]), t1->uvpoints[4]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
      checkandDivide(newtri3, i);
    } else if (!lessthan(diff1, sub_div_param) and !lessthan(diff2, sub_div_param) and lessthan(diff3, sub_div_param)) {
      //call this function on three new triangles
      Triangle *newtri = new Triangle(p2, t1->side2(), t1->side3());
      newtri->setNormals(dont2, t1->normals[1], t1->normals[2]);
      newtri->setUV(midpoint(t1->uvpoints[2], t1->uvpoints[5]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[3]), t1->uvpoints[3], t1->uvpoints[4], t1->uvpoints[5]);
      Triangle *newtri2 = new Triangle(p1, p2, t1->side3());
      newtri2->setNormals(dont1, dont2, t1->normals[2]);
      newtri2->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[5]),t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[2]),t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[5]), t1->uvpoints[5]);
      Triangle *newtri3 = new Triangle(t1->side1(), p2, p1);
      newtri3->setNormals(t1->normals[0], dont2, dont1);
      newtri3->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[1]),t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[2]),t1->uvpoints[2], midpoint(t1->uvpoints[0], t1->uvpoints[2]), t1->uvpoints[0]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
      checkandDivide(newtri3, i);
    } else if (lessthan(diff1, sub_div_param) and lessthan(diff2, sub_div_param) and !lessthan(diff3, sub_div_param)) {
      //call this function on two new triangles
      Triangle *newtri = new Triangle(t1->side1(), p3, t1->side3());
      newtri->setNormals(t1->normals[0], dont3, t1->normals[2]);
      newtri->setUV(t1->uvpoints[0], t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[4]), t1->uvpoints[4], midpoint(t1->uvpoints[4], t1->uvpoints[5]), t1->uvpoints[5]);
      Triangle *newtri2 = new Triangle(t1->side1(), t1->side2(), p3);
      newtri2->setNormals(t1->normals[0], t1->normals[1], dont3);
      newtri2->setUV(midpoint(t1->uvpoints[1], t1->uvpoints[4]), t1->uvpoints[1], t1->uvpoints[2], t1->uvpoints[3], midpoint(t1->uvpoints[3], t1->uvpoints[4]), t1->uvpoints[4]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
    } else if (lessthan(diff1, sub_div_param) and !lessthan(diff2, sub_div_param) and lessthan(diff3, sub_div_param)) {
      //call this function on two new triangles
      Triangle *newtri = new Triangle(t1->side1(), p2, t1->side3());
      newtri->setNormals(t1->normals[0], dont2, t1->normals[2]);
      newtri->setUV(t1->uvpoints[0], t1->uvpoints[1], midpoint(t1->uvpoints[1], t1->uvpoints[2]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[5]), t1->uvpoints[5]);
      Triangle *newtri2 = new Triangle(p2, t1->side2(), t1->side3());
      newtri2->setNormals(dont2, t1->normals[1], t1->normals[2]);
      newtri2->setUV(midpoint(t1->uvpoints[5], t1->uvpoints[2]), t1->uvpoints[2], midpoint(t1->uvpoints[2], t1->uvpoints[3]), t1->uvpoints[3], t1->uvpoints[4], t1->uvpoints[5]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
    } else if (!lessthan(diff1, sub_div_param) and lessthan(diff2, sub_div_param) and lessthan(diff3, sub_div_param)) {
      //call this function on two new triangles
      Triangle *newtri = new Triangle(t1->side1(), t1->side2(), p1);
      newtri->setNormals(t1->normals[0], t1->normals[1], dont1);
      newtri->setUV(midpoint(t1->uvpoints[0], t1->uvpoints[1]), t1->uvpoints[1], t1->uvpoints[2], t1->uvpoints[3], midpoint(t1->uvpoints[0], t1->uvpoints[3]), t1->uvpoints[0]);
      Triangle *newtri2 = new Triangle(p1, t1->side2(), t1->side3());
      newtri2->setNormals(dont1, t1->normals[1], t1->normals[2]);
      newtri2->setUV(midpoint(t1->uvpoints[5], t1->uvpoints[0]), t1->uvpoints[0], midpoint(t1->uvpoints[0], t1->uvpoints[3]), t1->uvpoints[3], t1->uvpoints[4], t1->uvpoints[5]);
      checkandDivide(newtri, i);
      checkandDivide(newtri2, i);
    } else {
      // TODO: mke it doesnt
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
  // bool lighting = false;
  bool lighting = true;

  if (lighting){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
    myReshape(viewport.w,viewport.h);

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    // glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    // glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, diffuse);

    /* Use depth buffering for hidden surface elimination. */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

  } else {
    // glDisable(GL_LIGHTING);
    glColor3f(1.0,1.0,0.0);
  }  
}


//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);       // clear the color buffer
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, 1.0, 1.0, 100.0);
  // glMatrixMode(GL_MODELVIEW);
  glClearDepth(1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // clear the color buffer
  gluLookAt(0.0, 0.0, 10.0,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.0);

  glMatrixMode(GL_MODELVIEW);        // indicate we are specifying camera transformations
  glLoadIdentity();        // make sure transformation is "zero'd"
  // glOrtho(-1, 1, -1, 1, 1, -1);    // resize type = stretch



  /*************************
  ** TRANSFORMATIONS
  *************************/
  //todo fix roationt issue for top-down
  glTranslatef(horizontalshift, verticalshift, 0.0);
  glRotatef(rotatevertical, 1, 0, 0);
  glRotatef(rotatehoriz, 0, 1, 0);
  glScalef(zoomamount, zoomamount, zoomamount);

  /*************************
  ** SHADING OPENGL STUFF
  *************************/

  if (flat) glShadeModel(GL_FLAT);
  else glShadeModel(GL_SMOOTH);    // TODO: change to be smooth and flat
  glFrontFace(GL_CW);
  glLineWidth(.75);
  if (wireframe) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  // glColor3f(1.0f,1.0f,0.0f); 


  /************************************
  ** MAKE OUR QUADS WITH OUR VERTICES
  *************************************/
if (!adaptive) {//uniform
  vector<Vector3f> somevertices;
  for (int i = 0; i < patchez.size(); i++) {
    int numdiv = patchez[i]->getPoints().size() - 1;
    //cout << numdiv << " NUMDIV" << endl;
    for (int j = 0; j < numdiv + 1; j ++) {
      for (int k = 0; k < numdiv + 1; k ++) {
        if (k != numdiv and j != numdiv) {
          glBegin(GL_QUADS);
          glNormal3f(normalz[i]->getPoints()[j][k].x(), normalz[i]->getPoints()[j][k].y(), normalz[i]->getPoints()[j][k].z());
          glVertex3f(patchez[i]->getPoints()[j][k].x(), patchez[i]->getPoints()[j][k].y(), patchez[i]->getPoints()[j][k].z());
          glNormal3f(normalz[i]->getPoints()[j][k + 1].x(), normalz[i]->getPoints()[j][k + 1].y(), normalz[i]->getPoints()[j][k + 1].z());
          glVertex3f(patchez[i]->getPoints()[j][k + 1].x(), patchez[i]->getPoints()[j][k + 1].y(), patchez[i]->getPoints()[j][k + 1].z());
          glNormal3f(normalz[i]->getPoints()[j + 1][k + 1].x(), normalz[i]->getPoints()[j + 1][k + 1].y(), normalz[i]->getPoints()[j + 1][k + 1].z());
          glVertex3f(patchez[i]->getPoints()[j + 1][k + 1].x(), patchez[i]->getPoints()[j + 1][k + 1].y(), patchez[i]->getPoints()[j + 1][k + 1].z());
          glNormal3f(normalz[i]->getPoints()[j + 1][k].x(), normalz[i]->getPoints()[j + 1][k].y(), normalz[i]->getPoints()[j + 1][k].z());
          glVertex3f(patchez[i]->getPoints()[j + 1][k].x(), patchez[i]->getPoints()[j + 1][k].y(), patchez[i]->getPoints()[j + 1][k].z());
        }
      }
    }
  }
} else {//adaptive
    vector<Vector3f> midpoints;
    for (int i = 0; i < patchez.size(); i++) {
      glBegin(GL_TRIANGLES);
        Triangle *t1 = new Triangle(patchez[i]->getPoints()[0][1], patchez[i]->getPoints()[1][1], patchez[i]->getPoints()[0][0]);
        t1->setNormals(normalz[i]->getPoints()[0][1], normalz[i]->getPoints()[1][1], normalz[i]->getPoints()[0][0]);
        Vector2f onev = *(new Vector2f(0.0, 0.5));
        Vector2f twov = *( new Vector2f(0.0, 1.0));
        Vector2f threev = *(new Vector2f(0.5, 1.0));
        Vector2f fourv = *(new Vector2f(1.0, 1.0));
        Vector2f fivev = *(new Vector2f(0.5, 0.5));
        Vector2f sixv = *(new Vector2f(0.0, 0.0));
        t1->setUV(onev, twov, threev, fourv, fivev, sixv);
        checkandDivide(t1, i);
        Triangle *t2 = new Triangle(patchez[i]->getPoints()[0][0], patchez[i]->getPoints()[1][1], patchez[i]->getPoints()[1][0]);
        t2->setNormals(normalz[i]->getPoints()[0][1], normalz[i]->getPoints()[1][1], normalz[i]->getPoints()[1][0]);
        Vector2f onev2 = *(new Vector2f(0.5, 0.0));
        Vector2f twov2 = *( new Vector2f(0.0, 0.0));
        Vector2f threev2 = *(new Vector2f(0.5, 0.5));
        Vector2f fourv2 = *(new Vector2f(1.0, 1.0));
        Vector2f fivev2 = *(new Vector2f(1.0, 0.5));
        Vector2f sixv2 = *(new Vector2f(1.0, 0.0));
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
   flat = !flat;
   break;
  case 'w':
   cout << "w: toggle filled/wireframe" << endl;
   wireframe = !wireframe;
   // initScene();
   break;
  case '=':
    zoomamount += .1;
    glutPostRedisplay();
    cout << "=: zoom in" << endl;
    break;
  case '-':
    zoomamount -= .1;
    glutPostRedisplay();
    cout << "-: zoom out" << endl;
    break;
  case '+':
    zoomamount += .1;
    glutPostRedisplay();
    cout << "+: zoom in" << endl;
    break;
  case '_':
    zoomamount -= .1;
    glutPostRedisplay();
    cout << "_: zoom out" << endl;
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
      rotatevertical -= 10;
      glutPostRedisplay();
      cout << "rotate: up " << rotatevertical << endl;
      break;
    case GLUT_KEY_DOWN:
      rotatevertical += 10;
      glutPostRedisplay();
      cout << "rotate: down " << rotatevertical << endl;
      break;
    case GLUT_KEY_RIGHT:
      rotatehoriz += 10;
      glutPostRedisplay();
      cout << "rotate: right " << rotatehoriz << endl;
      break;
    case GLUT_KEY_LEFT:
      rotatehoriz -= 10;
      glutPostRedisplay();
      cout << "rotate: left " << rotatehoriz << endl;
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

// TODO: MAKE EVERYTHING POINTERS AGAIN SO AVOID OVERWRITING
void subdividepatch(Patch &patch, Patch &normal_patch, GLfloat step) {
  Vector3f* n = new Vector3f(0.0, 0.0, 0.0);
  // cout << "--------- START --------- " << (*n).transpose() << endl;
  GLfloat epsilon = step - fmod(1.0, step);
  GLfloat numdiv = ((1 + epsilon) / step);
  //cout << "numdiv " << numdiv << endl;
  vector<vector<Vector3f > > allpoints;
  vector<vector<Vector3f > > allnormals;
  for (int iu = 0; iu <= numdiv; iu++) {
    vector<Vector3f> onepoint;
    vector<Vector3f> normal;
    GLfloat u = iu * step;
    // cout << "iu " << iu << endl;
    for (int iv = 0; iv <= numdiv; iv++) {
      // cout << "iv " << iv << endl;
      GLfloat v = iv * step;
      // cout << "subdivide: u: " << u << " v: " << v << endl;
      // TODO: first row is always nan nan nan
      Vector3f p = bezpatchinterp(patch, u, v, n);
      onepoint.push_back(p);
      // cout << "onepoint curr: " << p.transpose() << " onepoint first: " << onepoint.begin()->transpose() << endl;
      Vector3f normz = *n;
      normal.push_back(normz);
      // // cout << "pushed a normal on, check if all normal stuff was overridden by: " << normz.transpose() << endl;
      // for( std::vector<Vector3f>::const_iterator i = normal.begin(); i != normal.end(); ++i)
      //   std::cout << (*i).transpose() << endl;
    }
    allpoints.push_back(onepoint);
    allnormals.push_back(normal);
  }

      // cout << allpoints.back().back().transpose() << endl;
  // cout << ">>>>>>>>>> patch" << endl;
  patch.setPoints(allpoints);
  // cout << ">>>>>>>>>>" << endl;
  // normal_patch.printPatch();

      // cout << allnormals.back().back().transpose() << endl;
  // cout << "~~~~~~~~~~ normals" << endl;
  normal_patch.setPoints(allnormals);
  // normal_patch.printNewPatch();
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

    while(!infile.eof() && line < 4) {      // Each iteration is one LINE <<<<<<
      getline(infile,STRING);

      if (isEmptyOrBlank(STRING)) {
        continue;
      }
      cout << STRING << endl;
      float a, b, c, d, e, f, g, h, i, j, k, l;
      std::istringstream iss (STRING);
      iss >> std::skipws >> a >> b >> c;
      iss >> std::skipws >> d >> e >> f;
      iss >> std::skipws >> g >> h >> i;
      iss >> std::skipws >> j >> k >> l;

      maxx = max(max(max(max(a, d), g), j), maxx);
      maxy = max(max(max(max(b, e), h), k), maxy);
      miny = min(min(min(min(b, e), h), k), miny);
      minx = min(min(min(min(a, d), g), j), minx);

      vector<Vector3f*> curve;
      Vector3f *apoint = new Vector3f(a, b, c);
      Vector3f *bpoint = new Vector3f(d, e, f);
      Vector3f *cpoint = new Vector3f(g, h, i);
      Vector3f *dpoint = new Vector3f(j, k, l);

      // curve.push_back(apoint);
      // curve.push_back(bpoint);
      // curve.push_back(cpoint);
      // curve.push_back(dpoint);
      // curves.push_back(curve);

      current_patch->patch[line][0] = *apoint;
      current_patch->patch[line][1] = *bpoint;
      current_patch->patch[line][2] = *cpoint;
      current_patch->patch[line][3] = *dpoint;

      line++;

    } // while

    cout << "/~~~~~~~~~ end patch ~~~~~~~~~~/" << endl;
    // getline(infile, STRING);
    patchez.push_back(current_patch);
    Patch* current_normal = new Patch();
    normalz.push_back(current_normal);

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
    // (*normalz[k]).printPatch();
    if (!adaptive) {
      subdividepatch(*patchez[k], *normalz[k], sub_div_param);
    } else {
      subdividepatch(*patchez[k], *normalz[k], 1.0);//to get the squares
    }
    // (*patchez[k]).printPatch();
  }


  


  //*******************************
  // GLUT STUFF
  //*******************************

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

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









