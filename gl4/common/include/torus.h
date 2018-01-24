#ifndef TORUS_H
#define TORUS_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

/*
 * build sphere with two triangle fans and a bunch of triangle strips
 */
class Torus : public Geometry {
protected:
  GLfloat mInnerRadius;
  GLfloat mOuterRadius;
  GLuint mSides;  // longitiude
  GLuint mRings;  // latitude

  Vec3Vector mVertices;
  Vec3Vector mNormals;
  Vec2Vector mTexcoords;

public:
  Torus() : mInnerRadius(0.25), mOuterRadius(1), mSides(16), mRings(16) {}
  Torus(GLfloat innerRadius, GLfloat outerRadius, GLuint sides, GLuint rings)
      : mInnerRadius(innerRadius),
        mOuterRadius(outerRadius),
        mSides(sides),
        mRings(rings) {}

  void buildVertex(GLuint location);
  // as i'm using strip and fan, normal should be vertex normal.
  void buildNormal(GLuint location);
  void buildTexcoord(GLuint location);

  void draw(GLuint primcount = 1);

  GLfloat getInnerRadius() const { return mInnerRadius; }
  void setInnerRadius(GLfloat v) { mInnerRadius = v; }

  GLfloat getOuterRadius() const { return mOuterRadius; }
  void setOuterRadius(GLfloat v) { mOuterRadius = v; }

  GLuint getSides() const { return mSides; }
  void setSides(GLuint v) { mSides = v; }

  GLuint getRings() const { return mRings; }
  void setRings(GLuint v) { mRings = v; }
};
}

#endif /* TORUS_H */
