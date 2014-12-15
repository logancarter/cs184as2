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

GLfloat ARM_LENGTH = 5.0;
GLfloat RADIUS = 1.0;
GLint SLICES = 15;
GLint STACKS = 15;
GLfloat OFFSET = 0.6;
Vector3f root(0, 0, 0);
//Vector3f currentendpoint = *(new Vector3f(0, 0, 0));//initialize to wherever we want to start
GLfloat epsilon = .001;
//std::vector< Joint * > joints;
Joint* joints[5];

std::vector< Arm * > arms;
//VectorXf angles;
VectorXf angles(12);
float armslength = 2.0;//hardcoded for now, the total length of the arm
Vector3f endpoint(0, 1, 0);//the current endpoint
// Vector3f goal(0, .5, 1);
Vector3f goal;
GLfloat t = 0;
std::vector< Vector3f* > paths;


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


Vector3f getGoal() {
  t += 0.01;
  GLfloat x = cos(t);
  GLfloat y = sin(t);
  GLfloat z = 0.0;
  return *(new Vector3f(x, y, z));
}

Vector3f* getGoalAt(GLfloat t) {
  GLfloat x = cos(t);
  GLfloat y = sin(t);
  GLfloat z = 0.0;
  return new Vector3f(z, x, y);
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
MatrixXf getJ() {
  MatrixXf jacobi(3, 4);
  jacobi.setZero();
  for (int x = 0; x < 4; x++) {
    Vector3f currentroot = joints[x]->point;
    Vector3f rotation_axis;
    rotation_axis << 1, 0, 0;//CHANGED
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
    // cout << to_end << " toend inside" << endl;
    jacobi(0, x) = to_end[0];
    jacobi(1, x) = to_end[1];
    jacobi(2, x) = to_end[2];
    //cout << jacobi(1, x);
    //cout << jacobi(2, x);
  }
  //cout << "jacobi inside" << jacobi << endl;
  return jacobi;
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
    //cout << "to end here" << to_end << endl;
    float length_to_end = to_end.norm();
    //cout << length_to_end << "lengthtoend" << endl;
    if (length_to_end > 0) {
      to_end.normalize();
    }
    to_end = to_end.cross(rotation_axis);
    // cout << "after cross" << to_end << endl;
    if (length_to_end > 0) {
      float step = 1.0f;
      to_end *= length_to_end * step * M_PI/180.0f;//point of step?
    }
    // cout << to_end << " toend inside22222" << endl;
    jacobi(0, x) = to_end[0];
    jacobi(1, x) = to_end[1];
    jacobi(2, x) = to_end[2];
    //cout << jacobi(1, x);
    //cout << jacobi(2, x);
  }
  // cout << "jacobi inside2222222" << jacobi << endl;
  return jacobi;
}


void updateAngles(VectorXf dtheta, VectorXf angls) {
  angles += dtheta * .1;
  if (angls[0] > 360) {
    angls[0] -= 360;
  }
  if (angls[1] > 360) {
    angls[1] -= 360;
  }
  if (angls[2] > 360) {
    angls[2] -= 360;
  }
  if (angls[3] > 360) {
    angls[3] -= 360;
  }
  if (angls[0] < 0) {
    angls[0] += 360;
  }
  if (angls[1] < 0) {
    angls[1] += 360;
  }
  if (angls[2] < 0) {
    angls[2] += 360;
  }
  if (angls[3] < 0) {
    angls[3] += 360;
  }
   // cout << angls << " angles" << endl;
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
        float one = index * 3;
        float two = index * 3 + 1;
        float three = index * 3 + 2;
        //cout << vec[index] << " hi" << endl;
        float radAngle = M_PI * vec[one]/180.0f;
        float radAngle2 = M_PI * vec[two]/180.0f;
        float radAngle3 =  M_PI * vec[three]/180.0f;
        t = AngleAxisf(radAngle, Vector3f(-1, 0, 0))
         * AngleAxisf(radAngle2, Vector3f(0, -1, 0))
          * AngleAxisf(radAngle3, Vector3f(0, 0, -1))//CHANGED
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
    // MatrixXf J = getJ(); //jacobian
    // // cout << "J" << J << endl;
    // JacobiSVD<MatrixXf> svd (J,ComputeThinU | ComputeThinV);  // singular value decomposition = pseudoinverse
    // Vector4f dtheta = svd.solve(dp);
    // dtheta = dtheta;

    MatrixXf J2 = getJ2();
    JacobiSVD<MatrixXf> svd2 (J2,ComputeThinU | ComputeThinV);
    VectorXf dtheta2 = svd2.solve(dp);
    // cout << dtheta << " dtheta" << endl;
    // updateAngles(dtheta, angles);
    // cout << "befreo updateAngles2" << endl;
    updateAngles(dtheta2, angles);
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
  glColor3f(1.0, 1.0, 0.0);
  //glMultMatrixf(sys)
  for (int i = 0; i < 4; i++) {
    glRotatef(angles[i * 3 + 0], -1, 0, 0);
    glRotatef(angles[i * 3 + 1], 0, -1, 0);
    glRotatef(angles[i * 3 + 2], 0, 0, -1);
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
    // cout << "rederning angle " << i << endl;
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


// //   /*************************
// //   ** FINALLY DRAW THE STUFF
// //   *************************/

// //   /* stuff goes here */
// //   // glutSolidSphere(RADIUS, SLICES, STACKS);
// //   // glTranslatef(0.0, 0.0, OFFSET);
// //   // glutSolidCone(RADIUS, ARM_LENGTH, SLICES, STACKS);
// //     //Vector3f goal = *(new Vector3f(8.0, 0.0, 0.0));
// //     // cout << "     " << endl;
// //     // cout << getEndPos(0, angles) << " \n0's end point\n" << endl;
// //     // cout << getEndPos(1, angles) << " \n1's end point" << endl;
// //     // cout << "     " << endl;

// //     updateSystem();
// //     renderSystem();//ACTUAL STUFF

//     glColor3f(1.0,1.0,0.0);


//   goal = getGoal();
  goal = getGoal();

  glColor3f(1.0,1.0,0.0);
  // cout << "RENDER GOAL: " << goal.transpose() << endl;

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

  initPath();

  glutDisplayFunc(myDisplay);     // function to run when its time to draw something

  glutReshapeFunc(myReshape);     // function to run when the window gets resized
  glutIdleFunc(myFrameMove);
  glutKeyboardFunc(myKeyboard);           // function to run when spacebar is pressed: should exit
  glutSpecialFunc(specialKeys);

  glutMainLoop();   // infinite loop that will keep drawing and resizing

  return 0;
}



