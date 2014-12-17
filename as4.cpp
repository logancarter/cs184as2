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
#include <math.h>

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
GLfloat zoomamount = .6;
GLfloat horizontalshift = 0.0;
GLfloat verticalshift = 0.0;
GLfloat rotatehoriz = 130.0;
GLfloat rotatevertical = -20.0;
bool wireframe = true, flat = true;
Vector3f root(0, 0, 0);
GLfloat epsilon = .1;
Joint* joints[5];
std::vector< Arm * > arms;
VectorXf angles(12);
float armslength = 2.0;//hardcoded for now, the total length of the arm
Vector3f endpoint(0, 0, 2);//the current endpoint
Vector3f goal;
GLfloat t = 0;
std::vector< Vector3f* > paths;
bool canreach = false;


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
GLfloat radius = 1.0;

void initScene(){
  glColor3f(1.0,1.0,0.0);
}

Vector3f getGoal() {
  t += 0.01;
  GLfloat x = radius * cos(t);
  GLfloat y = radius * sin(t);
  GLfloat z = 1.0;
  return *(new Vector3f(x, z, y));
}

Vector3f* getGoalAt(GLfloat t) {
  GLfloat x = radius * cos(t);
  GLfloat y = radius * sin(t);
  GLfloat z = 1.0;
  return new Vector3f(x, z, y);
}

void initPath() {
  int numsteps = 50;
  GLfloat step = (GLfloat) 6.3 / numsteps;
  for (int i = 0; i < numsteps; i++) {
    GLfloat tz = step * i;
    Vector3f *pt = getGoalAt(tz);
    paths.push_back(pt);
  }
}

void renderPath() {
  int numsteps = 50;
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_POINTS);
  GLfloat step = (GLfloat) 6.3 / numsteps;
  for (int i = 0; i < numsteps; i++) {
    Vector3f pt = *(paths[i]);
    glVertex3f(pt[0], pt[1], pt[2]);
  }
  glEnd();
}

//returns the Jacobian of the current system
//algorithm idea from stack overflow
MatrixXf getJ2() {
  MatrixXf jacobi(3, 12);
  jacobi.setZero();
  for (int x = 0; x < 12; x++) {
    Vector3f rotation_axis;
    int location = 3;
    if (x == 0 or x == 3 or x == 6 or x == 9) {
      rotation_axis << 1, 0, 0;
    } else if (x == 1 or x == 4 or x == 7 or x == 10) {
      rotation_axis << 0, 1, 0;
    } else {
      rotation_axis << 0, 0, 1;
    }
    if (x < 3) {
      location = 0;
    } else if (x < 6) {
      location = 1;
    } else if (x < 9) {
      location = 2;
    }
    Vector3f currentroot = joints[location]->point;
    Vector3f to_end = endpoint - currentroot;
    float length_to_end = to_end.norm();
    if (length_to_end > 0) {
      to_end.normalize();
    }
    to_end = to_end.cross(rotation_axis);
    // cout << "after cross" << to_end << endl;
    if (length_to_end > 0) {
      float step = 1.0f;
      to_end *= length_to_end * step * M_PI/180.0f;//point of step?
    }
    jacobi(0, x) = to_end[0];
    jacobi(1, x) = to_end[1];
    jacobi(2, x) = to_end[2];

  }
  return jacobi;
}


void updateAngles(VectorXf dtheta, VectorXf angls) {
  angles += dtheta;
  for (int i = 0; i < 12; i ++) {
    if (angls[i] > 360) {
      angls[i] -= 360;
    } else if (angls[i] < 360) {
      angls[i] += 360;
    }
  }
   // cout << angls << " angles" << endl;
}

bool canReach() {
  float distance = (goal - root).norm();
  if (abs(distance) > armslength) {
    canreach = true;
    return false;
  }
  canreach = true;
  return true;
}

Vector3f newgoal(Vector3f oldgoal) {
  Vector3f goal = oldgoal.normalized();
  Vector3f newgoal = root + goal * armslength;
  cout << "got new goal" << newgoal << endl;
  return newgoal;
}


//Credit goes to SigTerm on Stack Overflow
Vector3f getEndPos(int index, const VectorXf& vec){
    Vector3f pos(0, 0, 0);
    while(true){
        Affine3f t;
        float one = index * 3;
        float two = index * 3 + 1;
        float three = index * 3 + 2;
        float radAngle = M_PI * vec[one]/180.0f;
        float radAngle2 = M_PI * vec[two]/180.0f;
        float radAngle3 =  M_PI * vec[three]/180.0f;
        t = AngleAxisf(radAngle, Vector3f(-1, 0, 0))
         * AngleAxisf(radAngle2, Vector3f(0, -1, 0))
          * AngleAxisf(radAngle3, Vector3f(0, 0, -1))//CHANGED
            * Translation3f(Vector3f(0, 0, arms[index]->length));//change .5 to length//keep that for now
        pos = t * pos;
        if (index == 0)
            break;
        index--;
    }
    return pos;
}

void updateEndpoint() {
  for (int i = 0; i < 4; i++) {
    joints[i+1]->changePoint(getEndPos(i, angles));
  }
  joints[3]->changePoint(getEndPos(2, angles));
  endpoint = joints[4]->point;
}



//Used code example from Kevin's discussion slide
//Returns true if reaches target. If it can't reach, it approximates
bool update() {
  if (!canreach and !canReach()) {
    goal = newgoal(goal);
  } 

  Vector3f dp = goal - endpoint;
  if (dp.norm() > epsilon) {
    MatrixXf J2 = getJ2();
    JacobiSVD<MatrixXf> svd2 (J2,ComputeThinU | ComputeThinV);
    VectorXf dtheta2 = svd2.solve(dp);
    updateAngles(dtheta2, angles);
    updateEndpoint();
    return false;
  }
  return true;
}

void renderSystem();

void updateSystem() {
  bool done = false;
  while (!done) {
    done = update();
    renderSystem();
  }
  canreach = false;
}

void renderSystem() {
  glPushMatrix();
  glColor3f(1.0, 1.0, 0.0);
  for (int i = 0; i < 4; i++) {
    glRotatef(angles[i * 3 + 0], -1, 0, 0);
    glRotatef(angles[i * 3 + 1], 0, -1, 0);
    glRotatef(angles[i * 3 + 2], 0, 0, -1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, arms[i]->length);
    glEnd();
    glTranslatef(0, 0, arms[i]->length);
  }

  glPopMatrix();
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_POINTS);
  for (int x = 0; x < 5; x++) {
    glVertex3f(joints[x]->point[0], joints[x]->point[1], joints[x]->point[2]);
  }
  glEnd();
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_POINTS);
  glVertex3f(goal[0], goal[1], goal[2]);
  glVertex3f(0, 0, 0);
  glEnd();
  // cout << goal << " goal " << endl;

  renderPath();
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // clear the color buffer
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();        // make sure transformation is "zero'd"

  /*************************
  ** TRANSFORMATIONS
  *************************/
  //glTranslatef(horizontalshift, verticalshift, 0.0);
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
  glVertex3f(3.0, 0.0, 0.0);
  glEnd();
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 3.0, 0.0);
  glEnd();
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 3.0);
  glEnd();

// //   /*************************
// //   ** FINALLY DRAW THE STUFF
// //   *************************/

  goal = getGoal();

  glColor3f(1.0,1.0,0.0);

  updateSystem();
  // renderSystem();
  // cout << goal << "current goal" << endl;
  
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
      rotatevertical += 10;
      glutPostRedisplay();
      cout << "rotate: up " << rotatevertical << endl;
      break;
    case GLUT_KEY_DOWN:
      rotatevertical -= 10;
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
  angles << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
  Vector3f *pos0 = (new Vector3f(0, 0, 0));
  Vector3f *pos1 = (new Vector3f(0, 0, .8));
  Vector3f *pos2 = (new Vector3f(0, 0, 1.4));
  Vector3f *pos3 = (new Vector3f(0, 0, 1.8));
  Vector3f *pos4 = (new Vector3f(0, 0, 2.0));

  Joint* joint0 = new Joint(*pos0);
    joints[0] = joint0;
  Joint* joint1 = new Joint(*pos1);
    joints[1] = joint1;
  Joint* joint2 = new Joint(*pos2);
    joints[2] = joint2;
  Joint* joint3 = new Joint(*pos3);
    joints[3] = joint3;
  Joint* joint4 = new Joint(*pos4);//endpos
    joints[4] = joint4;

  Arm* arm0 = new Arm(*pos0, *pos1, .8);
    arms.push_back(arm0);
  Arm* arm1 = new Arm(*pos1, *pos2, .6);
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

  initPath();

  glutDisplayFunc(myDisplay);     // function to run when its time to draw something

  glutReshapeFunc(myReshape);     // function to run when the window gets resized
  glutIdleFunc(myFrameMove);
  glutKeyboardFunc(myKeyboard);           // function to run when spacebar is pressed: should exit
  glutSpecialFunc(specialKeys);

  glutMainLoop();   // infinite loop that will keep drawing and resizing

  return 0;
}


