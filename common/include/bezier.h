#ifndef BEZIER_H
#define BEZIER_H

#include <osg/Geometry>
#include <osg/Array>
#include "zmath.h"

namespace zxd {

/*
 * generally bezier class, degree 1-n
 */
class Bezier : public osg::Geometry {
protected:
  unsigned int mSegments;
  osg::ref_ptr<osg::Vec3Array> mControlPoints;

public:
  Bezier() : mSegments(20) {}

  unsigned int getSegments() const { return mSegments; }
  void setSegments(unsigned int v) { mSegments = v; }

  void setControlPoints(osg::Vec3Array* controlPoints) {
    mControlPoints = controlPoints;
  }
  osg::Vec3Array* getControlPoints() { return mControlPoints; }

  // call this if you changed segments or add, delete vertices
  void rebuild() {
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    vertices->reserve(mSegments + 1);
    setVertexArray(vertices);

    float dt = 1.0f / mSegments;
    for (uint i = 0; i <= mSegments; ++i) {
      vertices->push_back(get(dt * i));
    }

    setVertexArray(vertices);

    this->removePrimitiveSet(0, this->getNumPrimitiveSets());
    this->addPrimitiveSet(
      new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size()));
  }

  unsigned int getDegree() {
    if (!mControlPoints) return 0;
    return mControlPoints->size() - 1;
  }

  osg::Vec3 get(GLfloat t) {
    t = std::max(std::min(t, 1.0f), 0.0f);
    float oneMinusT = 1 - t;
    osg::Vec3 p;
    uint degree = getDegree();

    for (uint i = 0; i <= degree; ++i) {
      p += mControlPoints->at(i) *
           ((std::pow(oneMinusT, degree - i) * std::pow(t, i) *
             zxd::Math::binomial(degree, i)));
    }

    return p;
  }

  static osg::ref_ptr<osg::Vec3Array> getIntermediatePoints(
    osg::Vec3Array* controlPoints, float t) {
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    if (controlPoints->size() < 2) {
      OSG_NOTICE << "not enough control points : " << controlPoints->size()
                 << std::endl;
      return vertices;
    }

    t = osg::clampTo(t, 0.0f, 1.0f);
    vertices->reserve(controlPoints->size() - 1);
    for (unsigned int i = 0; i < vertices->capacity(); ++i) {
      vertices->push_back(
        controlPoints->at(i) * (1 - t) + controlPoints->at(i + 1) * t);
    }
    return vertices;
  }
};
}

#endif /* BEZIER_H */
