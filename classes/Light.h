#ifndef Light_H
#define Light_H

class Light {
  public:
    GLfloat r, g, b, x, y, z;
    void setValues(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat r1, GLfloat g1, GLfloat b1) {
      x = x1; y = y1; z = z1; r = r1; g = g1; b = b1;
    }
    virtual int isDLight() { return 30; }
};

class PointLight: public Light {
  public:
    int isDLight() {
      return 0;
    }
};

class DirectionalLight: public Light {
public:
    int isDLight() {
      return 1;
    }
};

#endif