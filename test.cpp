// CS184 Simple OpenGL Example
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <memory>
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

#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif

#define PI 3.14159265

using namespace std;

//****************************************************
// Some Classes
//****************************************************
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

//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport(0,0,viewport.w,viewport.h);// sets the rectangle that will be the window
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();                // loading the identity matrix for the screen
  // gluOrtho2D(0, viewport.w, 0, viewport.h);

  //----------- setting the projection -------------------------
  // glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


  // glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
  // glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

  glOrtho(-1, 1, -1, 1, 1, -1);    // resize type = stretch

  //------------------------------------------------------------
}


//****************************************************
// sets the window up
//****************************************************
void initScene(){
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent

  myReshape(viewport.w,viewport.h);
}


//***************************************************
// function that does the actual drawing
//***************************************************
void myDisplay() {


  //----------------------- ----------------------- -----------------------
  // This is a quick hack to add a little bit of animation.
  static float tip = 0.0f;
  static float left = -0.1f;
  static float right = 0.1f;
  // // const  float stp = 0.0001f;
  static float stp = 0.03f;
  // static float stp1 = -stp;

  // const  float beg = 0.1f;
  const float beg = -0.85f;
  const  float end = 0.85f;

  static float lefty = 0.195f;
  static float righty = 0.195f;
  static float tipy = 0.25f;

  static float SW = 0.0f;
  static float NW = 0.5f;
  static float NE = 0.5f;
  static float SE = 0.0f;

  //Make the triangle bounce back and forth
  tip += stp;
  left += stp;
  right += stp;
  SW += stp;
  SE += stp;
  NE += stp;
  NW += stp;
  // tipy += stp1;
  // lefty += stp;
  // righty += stp;
  if (tip>end || tip<beg) {
    stp = -stp;
  }


  //----------------------- ----------------------- -----------------------


  glClear(GL_COLOR_BUFFER_BIT);                // clear the color buffer (sets everything to black)

  glMatrixMode(GL_MODELVIEW);                  // indicate we are specifying camera transformations
  glLoadIdentity();                            // make sure transformation is "zero'd"

  //----------------------- code to draw objects --------------------------
  // Rectangle Code
  //glColor3f(red component, green component, blue component);
  glColor3f(1.0f,0.0f,0.0f);                   // setting the color to pure red 90% for the rect


  // Left rectangle
  glBegin(GL_POLYGON);                         // draw rectangle 
  //glVertex3f(x val, y val, z val (won't change the point because of the projection type));
  glVertex3f(-1.0f, SW, 0.0f);               // bottom left corner of rectangle
  glVertex3f(-1.0f, NW, 0.0f);               // top left corner of rectangle
  glVertex3f(-0.95f, NE, 0.0f);               // top right corner of rectangle
  glVertex3f(-0.95f, SE, 0.0f);               // bottom right corner of rectangle
  glEnd();


  glColor3f(3.2f,0.7f,0.9f);                   // setting the color to pink for the right
  // Right rectangle
  // TODO: SHIFT DOWN A BIT?
  glBegin(GL_POLYGON);                         // draw rectangle 
  //glVertex3f(x val, y val, z val (won't change the point because of the projection type));
  glVertex3f(1.0f, SW, 0.0f);               // bottom left corner of rectangle
  glVertex3f(1.0f, NW, 0.0f);               // top left corner of rectangle
  glVertex3f(0.95f, NE, 0.0f);               // top right corner of rectangle
  glVertex3f(0.95f, SE, 0.0f);               // bottom right corner of rectangle
  glEnd();


  // Triangle Code
  glColor3f(1.0f,0.5f,0.0f);                   // setting the color to orange for the triangle
  float basey = -sqrt(0.04f);                  // height of triangle = sqrt(.8^2-.4^2)
  glBegin(GL_POLYGON);
  glVertex3f(tip,  tipy, 0.0f);                // top tip of triangle
  glVertex3f(left, lefty, 0.0f);               // lower left corner of triangle
  glVertex3f(right, righty, 0.0f);               // lower right corner of triangle
  glEnd();


  static float j1 = -0.2f;
  static float j2 = 0.2f;
  static float j3 = 0.2f;
  static float j4 = 0.05f;
  static float j5 = 0.05f;
  static float j6 = -0.4f;
  static float j7 = -0.4f;
  static float j8 = -0.35f;
  static float j9 = -0.35f;
  static float j10 = -0.05f;
  static float j11 = -0.05f;
  static float j12 = -0.2f;

  // J Code
  glColor3f(0.40f,0.5f,-0.5f);                   // setting the color to orange for the triangle
  glBegin(GL_POLYGON);
  glVertex3f(j1, 0.5f, 0.0f);
  glVertex3f(j2, 0.5f, 0.0f);
  glVertex3f(j3, 0.45f, 0.0f);
  glVertex3f(j12, 0.45f, 0.0f);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex3f(j4, 0.45f, 0.0f);
  glVertex3f(j5, -0.55f, 0.0f);
  glVertex3f(j10, -0.55f, 0.0f);
  glVertex3f(j11, 0.45f, 0.0f);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex3f(0.05f, -0.55f, 0.0f);
  glVertex3f(0.05f, -0.65f, 0.0f);
  glVertex3f(-0.35f, -0.65f, 0.0f);
  glVertex3f(-0.35f, -0.55f, 0.0f);
  glEnd();

  glBegin(GL_POLYGON);
  glVertex3f(-0.35f, -0.55f, 0.0f);
  glVertex3f(-0.35f, -0.40f, 0.0f);
  glVertex3f(-0.25f, -0.40f, 0.0f);
  glVertex3f(-0.25f, -0.55f, 0.0f);
  glEnd();
  //-----------------------------------------------------------------------

  glFlush();
  glutSwapBuffers();                           // swap buffers (we earlier set double buffer)
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
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("CS184!!!!!!!!!!!!1");

  initScene();                                 // quick function to set up scene

  glutDisplayFunc(myDisplay);                  // function to run when its time to draw something
  glutReshapeFunc(myReshape);                  // function to run when the window gets resized
  glutIdleFunc(myFrameMove);                   // function to run when not handling any other task
  glutMainLoop();                              // infinite loop that will keep drawing and resizing and whatever else

  return 0;
}








