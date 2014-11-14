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

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
public:
  int w, h; // width and height
};

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
// Global Variables
//****************************************************

Viewport viewport;
float sub_div_param;
bool adaptive = false;      //if true: adaptive; if false: uniform
std::vector<int *> patches;

std::vector<Vector3f *> points;

vector<Vector3f> somepoints_toconnect;
float zoomamount = 1.0;
float horizontalshift = 0.0;
float verticalshift = 0.0;
float rotatehoriz = 0.0;
float rotatevertical = 0.0;


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
  // TODO: this? vvv
  // glTranslatef(-horizontalshift, -verticalshift, -0.0);
  glScalef(zoomamount, zoomamount, zoomamount);
  glLineWidth(3);
  // glBegin(GL_LINE_STRIP);

  glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(.5, 0.0, 0.0);
    glVertex3f(0.3, 0.5, 0.0);
    glVertex3f(-0.3, 0.5, 0.0);
    glVertex3f(-0.3, 0.0, 0.0);
  glEnd();
  glLineWidth(1.5);
  glBegin(GL_LINE_STRIP);
    glColor3f(1.0f,0.0f,0.0f); 
    for (int i = 0; i < somepoints_toconnect.size(); i++) {
      glVertex3f(somepoints_toconnect[i].x(), somepoints_toconnect[i].y(), somepoints_toconnect[1].z());
    }
  glEnd();

  glFlush();
  glutSwapBuffers();// swap buffers (we earlier set double buffer)

}


//****************************************************
// called by glut when there are no messages to handle
//****************************************************
void myFrameMove() {
  //nothing here for now
#ifdef _WIN32
  Sleep(10);                                   //give ~10ms back to OS (so as not to waste the CPU)
#endif
  glutPostRedisplay(); // forces glut to call the display function (myDisplay())
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
   break;
  // TODO: check if its = or + (shift)
  case '=':
    zoomamount += .2;
    glutPostRedisplay();
    cout << "+: zoom in" << endl;
    break;
  case '-':
    zoomamount -= .2;
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

Vector3f bezcurveinterp(std::vector<Vector3f *> curve, float u) {
  Vector3f a = *curve[0] * (1.0 - u) + *curve[1] * u;
  Vector3f b = *curve[1] * (1.0 - u) + *curve[2] * u;
  Vector3f c = *curve[2] * (1.0 - u) + *curve[3] * u;

  Vector3f d = a * (1.0 - u) + b * u;
  Vector3f e = b * (1.0 - u) + c * u;

  Vector3f p = d * (1.0 - u) + e * u;

  Vector3f dPdu = 3 * (e - d);
  // cout << curve[0].x << endl;
  // cout << curve[0].y << endl;
  // cout << curve[0].z << endl;
  //cout << p.x() << " " << p.y() << " " << p.z() << "in function" << endl;
  return p;
}

//****************************************************
// MAIN
//****************************************************
int main(int argc, char *argv[]) {
  bool argParse = true;
  // bool argParse = false;

  //*******************************
  // ARGUMENT PARSER
  //*******************************
  if (argParse) {         // hack to turn off until we have input file
  string STRING;
  ifstream infile;
  cout << argv[1] << endl;   //that's input file
  infile.open (argv[1]);
  int numpatches = 0;
  if (!infile.eof()) {
    getline(infile,STRING);
    numpatches = atoi(STRING.c_str());
  }

  if (numpatches == 0) return 1; // throw an exception somehow
  int line = 0;

  // TODO-fix: extra lines at end of file? check if STRING is empty?
  while(!infile.eof()) {      // Each iteration is one LINE <<<<<<
    getline(infile,STRING);
    cout << STRING << endl;
    GLfloat a, b, c, d, e, f, g, h, i, j, k, l;
    std::istringstream iss (STRING);
    iss >> std::skipws >> a >> b >> c;
    iss >> std::skipws >> d >> e >> f;
    iss >> std::skipws >> g >> h >> i;
    iss >> std::skipws >> j >> k >> l;
    // cout << "iteration" << endl;
    // cout << a << b << c << endl;
    // cout << d << e << f << endl;
    // cout << g << h << i << endl;
    // cout << j << k << l << endl;

    // TODO-check if need to pass pointers to the floats...like the vector problem of yore
    /*
    std::vector<GLfloat> point1; 
    point1.push_back(a);
    point1.push_back(b);
    point1.push_back(c);
    std::vector<GLfloat> point2; 
    point2.push_back(d);
    point2.push_back(e);
    point2.push_back(f);
    std::vector<GLfloat> point3; 
    point3.push_back(g);
    point3.push_back(h);
    point3.push_back(i);
    std::vector<GLfloat> point3; 
    point4.push_back(j);
    point4.push_back(k);
    point4.push_back(l);
    std::vector<std::vector<GLfloat>> line;
    line.push_back(point1);
    line.push_back(point2);
    line.push_back(point3);
    line.push_back(point4);
    */


    // glClear(GL_COLOR_BUFFER_BIT);                // clear the color buffer (sets everything to black)
    // glMatrixMode(GL_MODELVIEW);                  // indicate we are specifying camera transformations
    // glLoadIdentity();
    // glColor3f(1.0f,0.0f,0.0f); 

    // glBegin(GL_POLYGON); 
    // glVertex3f(a, b, c);
    // glVertex3f(d, e, f);
    // glVertex3f(g, h, i);
    // glVertex3f(j, k, l);
    // glEnd();
    // glFlush();
    // glutSwapBuffers();  


    Vector3f *apoint = new Vector3f(a, b, c);
    Vector3f *bpoint = new Vector3f(d, e, f);
    Vector3f *cpoint = new Vector3f(g, h, i);
    Vector3f *dpoint = new Vector3f(j, k, l);
    points.push_back(apoint);
    points.push_back(bpoint);
    points.push_back(cpoint);
    points.push_back(dpoint);


    line++;
    if (line == 4) {
      cout << "/~~~~~~~~~ end patch ~~~~~~~~~~/" << endl;
      getline(infile, STRING);
      
      line = 0;
    }
    // cout << j << " THATS j " << k << " k " << l << " and l" << endl;
    //now a-c is one vertex, d-f is another, etc to make up one rectangle/patch
    //do something with them
    //should probably use vector
  }
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
}

// vector<Vector3f*> points;
// Vector3f *apoint = new Vector3f(0.5, 0.0, 0.0);
// Vector3f *bpoint = new Vector3f(0.3, 0.5, 0.0);
// Vector3f *cpoint = new Vector3f(-0.3, 0.5, 0.0);
// Vector3f *dpoint = new Vector3f(-0.3, 0.0, 0.0);
// points.push_back(apoint);
// points.push_back(bpoint);
// points.push_back(cpoint);
// points.push_back(dpoint);

//vector<Vector3f> lines;

// TODO: make step size from input
// for (float i = 0; i < 1; i += 0.01) {
for (float i = 0; i < 1; i += sub_div_param) {
  Vector3f result = bezcurveinterp(points, i);
  //cout << result.x() << " " << result.y() << " " << result.z() << endl;
  somepoints_toconnect.push_back(result);
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



  //*******************************
  // PSEUDOCODE
  //*******************************
  
  /*****
  for each PATCH in file
    data structure to hold four vertices
    // iterate thru u
    //   iterate thru v
    subdivide(patch, step)
      bezpatchinterp(patch,u,v)
        bezpatchinterp(curve,u);


*/







