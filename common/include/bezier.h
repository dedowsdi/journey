#ifndef BEZIER_H
#define BEZIER_H

#include <osg/Geometry>
#include <osg/Array>
#include "zmath.h"

namespace zxd {

typedef std::vector<osg::ref_ptr<osg::Vec3Array>> Vec3VecVec;

/*
 * generally bezier class, degree >=1
 */
class Bezier : public osg::Geometry {
protected:
  unsigned int mSegments;  // number of segments
  osg::ref_ptr<osg::Vec3Array> mControlPoints;

public:
  Bezier() : mSegments(20) { mControlPoints = new osg::Vec3Array(); }

  unsigned int getSegments() const { return mSegments; }
  void setSegments(unsigned int v) { mSegments = v; }

  osg::ref_ptr<osg::Vec3Array> getControlPoints() { return mControlPoints; }

  // call this if you changed segments or add, delete, change control points
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

  unsigned int getDegree() { return n(); }
  inline unsigned int n() {
    if (!mControlPoints) return 0;
    return mControlPoints->size() - 1;
  }

  // get point at t
  osg::Vec3 get(GLfloat t) {
    return get(mControlPoints->begin(), mControlPoints->end(), t);
  }

  // get point at ith iteration, jth index
  osg::Vec3 get(GLuint i, GLuint j, GLfloat t) {
    // iteration of points will be an equilateral
    GLuint n = this->n();
    osg::Vec3 p;

    if (i > n) {
      OSG_NOTICE << "iteration over flow" << std::endl;
      return p;
    }
    if (i + j > n) {
      OSG_NOTICE << "index over flow" << std::endl;
      return p;
    }

    return get(
      mControlPoints->begin() + j, mControlPoints->begin() + j + i + 1, t);
  }

  // degree evevate or drop
  void elevate(bool positive = true) {
    GLuint n = this->n();
    if (!positive && n <= 1) return;

    osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array();

    points->push_back(mControlPoints->front());

    if (positive) {
      GLuint r = n + 1;
      points->reserve(r + 1);
      GLfloat rcpNplus1 = 1.0 / r;

      for (unsigned int i = 1; i < r; ++i) {
        points->push_back(mControlPoints->at(i - 1) * i * rcpNplus1 +
                          (mControlPoints->at(i) * (1 - i * rcpNplus1)));
      }
    } else {
      // degree drop, unstable. Graph might be changed as lower degree can't
      // entirely represent higher degree bezier
      GLuint r = n - 1;
      points->reserve(r + 1);
      GLfloat rcpN = 1.0 / n;

      if (r >= 2) {
        points->push_back(
          (mControlPoints->at(1) - points->front() * rcpN) / (1 - rcpN));

        for (unsigned int i = 2; i < r; ++i) {
          points->push_back((mControlPoints->at(i) + mControlPoints->at(i - 1) -
                              points->at(i - 2) * (i - 1) * rcpN -
                              points->at(i - 1) * (1 + rcpN)) /
                            (1 - i * rcpN));
        }
      }
    }

    points->push_back(mControlPoints->back());
    mControlPoints = points;
  }

  // derivative of a bezier curve is still a bezier curve
  osg::ref_ptr<Bezier> derivative(GLuint level = 1) {
    osg::ref_ptr<Bezier> curve = new Bezier();
    osg::ref_ptr<osg::Vec3Array> points = curve->getControlPoints();

    if (level == 0) return curve;
    if (level >= n()) {
      OSG_NOTICE << "too high level of derivative" << std::endl;
      return curve;
    }

    GLuint k = n() - level;
    points->reserve(k + 1);

    GLuint c = Math::pi(n() - k + 1, 1, k);

    for (unsigned int i = 0; i < k; ++i) {
      points->push_back(D(i, k) * c);
    }
    return curve;
  }

  // get tanget at t
  osg::Vec3 tangent(GLfloat t) {
    Vec3VecVec vv = iterateAll(*mControlPoints, t);
    GLuint s = vv.size();

    osg::Vec3 v = vv[s - 2]->at(1) - vv[s - 2]->at(0);
    v.normalize();
    return v;
  }

  // subdivide according to current point
  void subdivide(GLfloat t, osg::ref_ptr<Bezier> lc, osg::ref_ptr<Bezier> rc) {
    osg::ref_ptr<osg::Vec3Array> lp = lc->getControlPoints();
    osg::ref_ptr<osg::Vec3Array> rp = rc->getControlPoints();
    lp->clear();
    rp->clear();
    GLuint n = this->n();
    lp->reserve(n + 1);
    rp->reserve(n + 1);

    // for (uint i = 0; i < lp->capacity(); ++i) {
    // lp->push_back(get(i, 0, t));
    // rp->push_back(get(i, n - i, t));
    //}

    // get all iterations is faster than use get one by one
    Vec3VecVec vv = iterateAll(*mControlPoints, t);
    for (uint i = 0; i < lp->capacity(); ++i) {
      lp->push_back(vv[i]->at(0));
      rp->push_back(vv[i]->at(n - i));
    }
  }

  static Vec3VecVec iterateAll(osg::Vec3Array& controlPoints, float t) {
    Vec3VecVec vv;

    osg::ref_ptr<osg::Vec3Array> va = &controlPoints;
    vv.push_back(va);
    while (va->size() > 1) {
      va = iterate(*va, t);
      vv.push_back(va);
    }
    return vv;
  }

  // De Casteljau's algorithm
  static osg::ref_ptr<osg::Vec3Array> iterate(
    osg::Vec3Array& controlPoints, float t) {
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

    if (controlPoints.empty()) {
      OSG_NOTICE << "not enough control points : " << controlPoints.size()
                 << std::endl;
      return vertices;
    }

    if (controlPoints.size() == 1) {
      vertices->push_back(*controlPoints.begin());
      return vertices;
    }

    t = osg::clampTo(t, 0.0f, 1.0f);
    vertices->reserve(controlPoints.size() - 1);
    for (unsigned int i = 0; i < vertices->capacity(); ++i) {
      vertices->push_back(
        controlPoints.at(i) * (1 - t) + controlPoints.at(i + 1) * t);
    }
    return vertices;
  }

  static osg::Vec3 get(
    osg::Vec3Array::iterator beg, osg::Vec3Array::iterator end, GLfloat t) {
    t = std::max(std::min(t, 1.0f), 0.0f);

    GLuint p = end - beg - 1;
    float oneMinusT = 1 - t;

    // get by definition, binomial might cause problem as degree gets too bigger
    // for (uint i = 0; i <= degree; ++i) {
    // p += *(beg + i) * ((std::pow(oneMinusT, degree - i) * std::pow(t, i) *
    // zxd::Math::binomial(degree, i)));
    //}

    // get by de Casteljau's algorithm
    osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array(beg, end);
    for (unsigned int i = 0; i < p; ++i) {
      for (unsigned int j = 0; j < p - i; ++j) {
        points->at(j) = points->at(j) * oneMinusT + points->at(j + 1) * t;
      }
    }

    return points->front();
  }

private:
  // special function used to derivative
  osg::Vec3 D(GLuint i, GLuint k) {
    if (k == 0)
      return mControlPoints->at(i + 1) - mControlPoints->at(i);
    else
      return D(i + 1, k - 1) - D(i, k - 1);
  }
};
}

#endif /* BEZIER_H */
