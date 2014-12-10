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
GLfloat rotatehoriz = 0.0;
GLfloat rotatevertical = 0.0;
bool wireframe = true, flat = true;

GLfloat ARM_LENGTH = 5.0;
GLfloat RADIUS = 1.0;
GLint SLICES = 15;
GLint STACKS = 15;
GLfloat OFFSET = 0.6;
Vector3f root = *(new Vector3f(0, 0, 0));
Vector3f currentendpoint = *(new Vector3f(0, 0, 0));//initialize to wherever we want to start
float epsilon = .1;
std::vector< Joint * > joints;
std::vector< Arm * > arms;
float armslength = 0.0;

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

bool canReach(Vector3f goal) {
  for (int i = 0; i < arms.size(); i++) {
    armslength += arms[i]->length;
  }
  float d0 = goal[0] - root[0];
  float d1 = goal[1] - root[1];
  float d2 = goal[2] - root[2];
  float distance = sqrt(pow(d0, 2) + pow(d1, 2) + pow(d2, 2));
  if (abs(distance) > armslength) {
    return false;
  }
  return true;
}

Vector3f newgoal(Vector3f oldgoal) {
  Vector3f goal = oldgoal.normalized();
  Vector3f newgoal = root + goal * armslength;
  return newgoal;
}
//Used code example from Kevin's discussion slide
//Returns true if reaches target. If it can't reach, it approximates

bool update(Vector3f g) {
  Vector3f actualgoal;
  if (!canReach(g)) {
    actualgoal = newgoal(g);
  } else {
    actualgoal = g;
  }
  Vector3f dp = actualgoal - currentendpoint;
  if (dp.norm() > epsilon) {
//     J = system.getJ(); #jacobian
//     svd(J);#singular value decomposition
//     dtheta = svd.solve(dp);

    // updateAngles(dtheta);
    // updateEndpoint();
    return false;
  }
  return true;
}



//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);       // clear the color buffer
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, 1.0, 1.0, 100.0);

  // gluOrtho2D(0, viewport.w, 0, viewport.h);
  // glMatrixMode(GL_MODELVIEW);
  glClearDepth(1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // clear the color buffer
  gluLookAt(0.0, 0.0, 13.0,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /*center is at (0,0,0) */
    0.0, 1.0, 0.0);

  glMatrixMode(GL_MODELVIEW);        // indicate we are specifying camera transformations
  glLoadIdentity();        // make sure transformation is "zero'd"
  // glOrtho(-1, 1, -1, 1, 1, -1);    // resize type = stretch

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


  /*************************
  ** FINALLY DRAW THE STUFF
  *************************/

  /* stuff goes here */
  // glutSolidSphere(RADIUS, SLICES, STACKS);
  // glTranslatef(0.0, 0.0, OFFSET);
  // glutSolidCone(RADIUS, ARM_LENGTH, SLICES, STACKS);
    glColor3f(1.0,1.0,0.0);
    glBegin(GL_LINES);
  for (int i = 0; i < arms.size(); i++) {
    //glutSolidSphere(joints[i]->radius, SLICES, STACKS);
    glVertex3f(arms[i]->startpoint[0], arms[i]->startpoint[1], arms[i]->startpoint[2]);
    glVertex3f(arms[i]->endpoint[0], arms[i]->endpoint[1], arms[i]->endpoint[2]);
    //cout << arms[i]->startpoint[0] << arms[i]->startpoint[1] << arms[i]->startpoint[2] << endl;
    glTranslatef(0.0, 0.0, OFFSET);
    //glutSolidCone(arms[i]->radius, arms[i]->length, SLICES, STACKS);
    //glTranslatef(0.0, 0.0, arms[i]->length + OFFSET);
  }
 //glVertex3f(arms[3]->endpoint[0], arms[3]->endpoint[1], arms[3]->endpoint[2]);
   glEnd();
  glColor3f(1.0,0.0,0.0);
     glBegin(GL_POINTS);
  for (int i = 0; i < joints.size(); i++) {
    //glutSolidSphere(joints[i]->radius, SLICES, STACKS);
   glVertex3f(joints[i]->point[0], joints[i]->point[1], joints[i]->point[2]);
    //glTranslatef(0.0, 0.0, OFFSET);
    //glutSolidCone(arms[i]->radius, arms[i]->length, SLICES, STACKS);
    // glTranslatef(0.0, 0.0, arms[i]->length + OFFSET);
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




//****************************************************
// MAIN
//****************************************************
int main(int argc, char *argv[]) {

  // TODO: create the joints with inputtable values
  Vector3f *pos0 = (new Vector3f(0, 0, 0));
  Vector3f *pos1 = (new Vector3f(.4, 0, 0));
  Vector3f *pos2 = (new Vector3f(1.4, 0, 0));
  Vector3f *pos3 = (new Vector3f(1.8, 0, 0));
  Vector3f *pos4 = (new Vector3f(2.0, 0, 0));

  Joint* joint0 = new Joint(*pos0);
    joints.push_back(joint0);
  Joint* joint1 = new Joint(*pos1);
    joints.push_back(joint1);
  Joint* joint2 = new Joint(*pos2);
    joints.push_back(joint2);
  Joint* joint3 = new Joint(*pos3);
    joints.push_back(joint3);
  Joint* joint4 = new Joint(*pos4);//endpos
    joints.push_back(joint3);

  Arm* arm0 = new Arm(*pos0, *pos1, .4);
    arms.push_back(arm0);
  Arm* arm1 = new Arm(*pos1, *pos2, 1.0);
    arms.push_back(arm1);
  Arm* arm2 = new Arm(*pos2, *pos3, .4);
    arms.push_back(arm2);
  Arm* arm3 = new Arm(*pos3, *pos4, .2);
    arms.push_back(arm3);



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



