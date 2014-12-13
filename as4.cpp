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
GLfloat zoomamount = .5;
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
Vector3f root(0, 0, 0);
//Vector3f currentendpoint = *(new Vector3f(0, 0, 0));//initialize to wherever we want to start
GLfloat epsilon = .01;
//std::vector< Joint * > joints;
Joint* joints[5];
std::vector< Arm * > arms;
Vector4f angles(0, 0, 0, 0);
float armslength = 2.0;//hardcoded for now, the total length of the arm
Vector3f endpoint(0, 0, 2);//the current endpoint
Vector3f goal(0, .5, 1);


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

//returns the Jacobian of the current system
//algorithm idea from stack overflow
MatrixXf getJ() {
  MatrixXf jacobi(3, 4);
  jacobi.setZero();
  for (int x = 0; x < 4; x++) {
    Vector3f currentroot = joints[x]->point;
    Vector3f rotation_axis;
    rotation_axis << 1, 0, 0;
    Vector3f to_end = endpoint - currentroot;
    //cout << "to end here" << to_end << endl;
    float length_to_end = to_end.norm();
    //cout << length_to_end << "lengthtoend" << endl;
    if (length_to_end > 0) {
      to_end.normalize();
    }
    to_end = to_end.cross(rotation_axis);
    //cout << "after cross" << to_end << endl;
    if (length_to_end > 0) {
      float step = 1.0f;
      to_end *= length_to_end * step * M_PI/180.0f;//point of step?
    }
    cout << to_end << " toend inside" << endl;
    jacobi(0, x) = to_end[0];
    jacobi(1, x) = to_end[1];
    jacobi(2, x) = to_end[2];
    //cout << jacobi(1, x);
    //cout << jacobi(2, x);
  }
  //cout << "jacobi inside" << jacobi << endl;
  return jacobi;
}

void updateAngles(Vector4f dtheta) {
  angles += dtheta * .1;
  if (angles[0] > 360) {
    angles[0] -= 360;
  }
  if (angles[1] > 360) {
    angles[1] -= 360;
  }
  if (angles[2] > 360) {
    angles[2] -= 360;
  }
  if (angles[3] > 360) {
    angles[3] -= 360;
  }
  if (angles[0] < 0) {
    angles[0] += 360;
  }
  if (angles[1] < 0) {
    angles[1] += 360;
  }
  if (angles[2] < 0) {
    angles[2] += 360;
  }
  if (angles[3] < 0) {
    angles[3] += 360;
  }
  // cout << angles << " angles" << endl;
}

bool canReach() {
  float distance = (goal - root).norm();
  //print()
  // cout << distance << " distance " << armslength << " armslength" << endl;
  if (abs(distance) > armslength) {
    return false;
  }
  return true;
}

Vector3f newgoal(Vector3f oldgoal) {
  Vector3f goal = oldgoal.normalized();
  Vector3f newgoal = root + goal * armslength;
  // cout << newgoal << " newgoal" << endl;
  return newgoal;
}


//Credit goes to SigTerm on Stack Overflow
Vector3f getEndPos(int index, const VectorXf& vec){
    Vector3f pos(0, 0, 0);
    while(true){
        Affine3f t;
        //cout << vec[index] << " hi" << endl;
        float radAngle = M_PI * vec[index]/180.0f;
        t = AngleAxisf(radAngle, Vector3f(-1, 0, 0))
            * Translation3f(Vector3f(0, 0, .5));//change .5 to length//keep that for now
        pos = t * pos;

        if (index == 0)
            break;
        index--;
    }
    //cout << pos << " get end pos" << endl;
    return pos;
}

void updateEndpoint() {
  //cout << joints.size() << endl;
  for (int i = 0; i < 4; i++) {
    joints[i+1]->changePoint(getEndPos(i, angles));
    // cout << i + 1 << endl;
  }
  // cout << "updateendpoint " << getEndPos(3, angles) << endl;
  // cout << joints[3]->point << "joint3b4" << endl;
  joints[3]->changePoint(getEndPos(2, angles));
  // cout << joints[3]->point << "joint3after" << endl;
  // cout << joints[4]->point << "joints4" << endl;
  endpoint = joints[4]->point;
  //cout << joints[4]->point << "hey" << endl;
  // cout << endpoint << " new endpoint" << endl;
}



//Used code example from Kevin's discussion slide
//Returns true if reaches target. If it can't reach, it approximates
bool update() {
  //Vector3f actualgoal;
  if (!canReach()) {
    //cout << "can't reach" << endl;
    goal = newgoal(goal);
  } else {
    goal = goal;
  }
  Vector3f dp = goal - endpoint;
  // cout << dp.norm() << " length " << endl;
  if (dp.norm() > epsilon) {
    MatrixXf J = getJ(); //jacobian
    // cout << "J" << J << endl;
    JacobiSVD<MatrixXf> svd (J,ComputeThinU | ComputeThinV);  // singular value decomposition = pseudoinverse
    Vector4f dtheta = svd.solve(dp);
    dtheta = dtheta;
    // cout << dtheta << " dtheta" << endl;
    updateAngles(dtheta);
    updateEndpoint();
    //cout << "returned false" << endl;
    return false;
  }
  // cout << "returned true" << endl;
  return true;
}

void updateSystem() {
  bool done = false;
  while (!done) {
    done = update();
  }
  //cout << "got out" << endl;
}

void renderSystem() {
  glPushMatrix();
  //glMultMatrixf(sys)
  for (int i = 0; i < 4; i++) {
    glRotatef(angles[i], -1, 0, 0);
    glBegin(GL_LINES);
    // if (i == 0) {
    //   glVertex3f(0, 0, 0);
    //   glVertex3f(joints[i + 1]->point[0], joints[i + 1]->point[1], joints[i + 1]->point[2]);
    // } else if (i == 3) {
    //   glVertex3f(joints[i]->point[0], joints[i]->point[1], joints[i]->point[2]);
    //   glVertex3f(endpoint[0], endpoint[1], endpoint[2]);
    // } else {
    //   glVertex3f(joints[i]->point[0], joints[i]->point[1], joints[i]->point[2]);
    //   glVertex3f(joints[i+1]->point[0], joints[i+1]->point[1], joints[i+1]->point[2]);
    // }
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, .5);
      //glVertex3f(arms[i]->endpoint[0], arms[i]->endpoint[1], arms[i]->endpoint[2]);
    glEnd();
    glTranslatef(0, 0, .5);
    // cout << joints[0]->point << " 0" << endl;
    // cout << joints[1]->point << " 01" << endl;
    // cout << joints[2]->point << " 02" << endl;
    // cout << joints[3]->point << " 03" << endl;
    // cout << joints[4]->point << " 04" << endl;

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
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // clear the color buffer
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();        // make sure transformation is "zero'd"
  // setupCamera() :::::
  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();
  // gluPerspective(40.0, 1.0, 1.0, 100.0);
  // // glMatrixMode(GL_MODELVIEW);
  // glClearDepth(1.0);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // clear the color buffer
  // gluLookAt(0.0, 0.0, 13.0,  /* eye is at (0,0,5) */
  //   0.0, 0.0, 0.0,      /*center is at (0,0,0) */
  //   0.0, 1.0, 0.0);



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
    //Vector3f goal = *(new Vector3f(8.0, 0.0, 0.0));
    // cout << "     " << endl;
    // cout << getEndPos(0, angles) << " \n0's end point\n" << endl;
    // cout << getEndPos(1, angles) << " \n1's end point" << endl;
    // cout << "     " << endl;

    updateSystem();
    renderSystem();//ACTUAL STUFF
    glColor3f(1.0,1.0,0.0);



    // glBegin(GL_LINES);
  // for (int i = 0; i < arms.size(); i++) {
  //   //glutSolidSphere(joints[i]->radius, SLICES, STACKS);
  //   glVertex3f(arms[i]->startpoint[0], arms[i]->startpoint[1], arms[i]->startpoint[2]);
  //   glVertex3f(arms[i]->endpoint[0], arms[i]->endpoint[1], arms[i]->endpoint[2]);
  //   //cout << arms[i]->startpoint[0] << arms[i]->startpoint[1] << arms[i]->startpoint[2] << endl;
  //   glTranslatef(0.0, 0.0, OFFSET);
  //   //glutSolidCone(arms[i]->radius, arms[i]->length, SLICES, STACKS);
  //   //glTranslatef(0.0, 0.0, arms[i]->length + OFFSET);
  // }
 //glVertex3f(arms[3]->endpoint[0], arms[3]->endpoint[1], arms[3]->endpoint[2]);
  //  glEnd();
  // glColor3f(1.0,0.0,0.0);
  //    glBegin(GL_POINTS);
  // for (int i = 0; i < joints.size(); i++) {
  //   //glutSolidSphere(joints[i]->radius, SLICES, STACKS);
  //  glVertex3f(joints[i]->point[0], joints[i]->point[1], joints[i]->point[2]);
  //   //glTranslatef(0.0, 0.0, OFFSET);
  //   //glutSolidCone(arms[i]->radius, arms[i]->length, SLICES, STACKS);
  //   // glTranslatef(0.0, 0.0, arms[i]->length + OFFSET);
  // }
  // glEnd();
    // cout << getEndPos(0, angles) << " 0" << endl;
    // cout << getEndPos(1, angles) << " 1" << endl;
  glColor3f(1.0,1.0,0.0);
  updateSystem();
  renderSystem();

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
  Vector3f *pos1 = (new Vector3f(0, 0, .5));
  Vector3f *pos2 = (new Vector3f(0, 0, 1.0));
  Vector3f *pos3 = (new Vector3f(0, 0, 1.5));
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

  Arm* arm0 = new Arm(*pos0, *pos1, .5);
    arms.push_back(arm0);
  Arm* arm1 = new Arm(*pos1, *pos2, .5);
    arms.push_back(arm1);
  Arm* arm2 = new Arm(*pos2, *pos3, .5);
    arms.push_back(arm2);
  Arm* arm3 = new Arm(*pos3, *pos4, .5);
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



