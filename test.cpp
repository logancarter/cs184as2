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
#include <time.h>

#include "Util.h"

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

using namespace std;
using namespace Eigen;


//***************************************************--*
// Some Classes
//****************************************************

class Viewport {
public:
  int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
GLfloat zoomamount = 1.0;
GLfloat horizontalshift = 0.0;
GLfloat verticalshift = 0.0;
GLfloat rotatehoriz = 120.0;
GLfloat rotatevertical = -20.0;
bool wireframe = true, flat = true;

GLfloat ARM_LENGTH = 5.0;
GLfloat RADIUS = 1.0;
GLint SLICES = 15;
GLint STACKS = 15;
GLfloat OFFSET = 0.6;
Vector3f root = *(new Vector3f(0, 0, 0));
//Vector3f currentendpoint = *(new Vector3f(0, 0, 0));//initialize to wherever we want to start
GLfloat epsilon = .1;
std::vector< Joint * > joints;
// Joint* joints[5];
std::vector< Arm * > arms;
Vector4f angles = *(new Vector4f(0, 0, 0, 0));
GLfloat armslength = 2.0;//hardcoded for now, the total length of the arm
Vector3f endpoint = *(new Vector3f(2, 0, 0));//the current endpoint
Vector3f goal = *(new Vector3f(0.0, 1.0, 0));


Vector3f rotater(0.0, 0.05, 0.0);

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

    glColor3f(1.0,1.0,0.0);

}


//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
//   glMatrixMode(GL_PROJECTION);
// glLoadIdentity();
// gluPerspective (60.0, viewport.w/viewport.h, 0.1, 100.0);
//   gluLookAt(2.0, 2.0, 2.0, 
//             0.0, 0.0, 0.0, 
//             0.0, 1.0, 0.0);
// glMatrixMode(GL_MODELVIEW);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // clear the color buffer
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();        // make sure transformation is "zero'd"


  /*************************
  ** TRANSFORMATIONS
  *************************/
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
  glLineWidth(2);
  glPointSize(4);
  if (wireframe) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  /* Draw axes */
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(10.0, 0.0, 0.0);
  glEnd();
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 1.0, 0.0);
  glEnd();
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 1.0);
  glEnd();

  glColor3f(1.0, 1.0, 1.0);
  Vector3f p = arms[0]->startpoint - arms[0]->endpoint;
  Matrix3f pcross;
  pcross << 0, -p[2], p[1],
            p[2], 0, -p[0],
            -p[1], p[0], 0;
  Vector3f dp = pcross * rotater;
  glBegin(GL_LINES);
  glVertex3f(arms[0]->startpoint[0], arms[0]->startpoint[1], arms[0]->startpoint[2]);
  Vector3f newendpoint;
  newendpoint << arms[0]->endpoint[0] + dp[0], arms[0]->endpoint[1] + dp[1], arms[0]->endpoint[2] + dp[2];
  arms[0]->endpoint = newendpoint;
  glVertex3f(arms[0]->endpoint[0], arms[0]->endpoint[1], arms[0]->endpoint[2]);
  glEnd();

  glFlush();
  glutSwapBuffers();        // swap buffers (we earlier set double buffer)
  // glutPostRedisplay();
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


//****************************************************
// MAIN
//****************************************************
int main(int argc, char *argv[]) {

  // TODO: create the joints with inputtable values
  Vector3f *pos0 = (new Vector3f(0, 0, 0));
  // Vector3f *pos1 = (new Vector3f(.5, 0, 0));
  Vector3f *pos1 = new Vector3f(5.0, 5.0, 5.0);
  Vector3f *pos2 = (new Vector3f(1.0, 0, 0));
  Vector3f *pos3 = (new Vector3f(1.5, 0, 0));
  Vector3f *pos4 = (new Vector3f(2.0, 0, 0));

  Joint* joint0 = new Joint(*pos0);
    joints.push_back(joint0);
  // Joint* joint1 = new Joint(*pos1);
  //   joints[1] = joint1;
  // Joint* joint2 = new Joint(*pos2);
  //   joints[2] = joint2;
  // Joint* joint3 = new Joint(*pos3);
  //   joints[3] = joint3;
  // Joint* joint4 = new Joint(*pos4);//endpos
  //   joints[4] = joint4;

  Arm* arm0 = new Arm(*pos0, *pos1, .5);
    arms.push_back(arm0);
  cout << arms[0]->endpoint[0] << " " << arms[0]->endpoint[1] << " " << arms[0]->endpoint[2] << endl;
  // Arm* arm1 = new Arm(*pos1, *pos2, .5);
  //   arms.push_back(arm1);
  // Arm* arm2 = new Arm(*pos2, *pos3, .5);
  //   arms.push_back(arm2);
  // Arm* arm3 = new Arm(*pos3, *pos4, .5);
  //   arms.push_back(arm3);



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



