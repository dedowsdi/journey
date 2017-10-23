#ifndef SPLINE_H
#define SPLINE_H

#include <osg/Geometry>
#include <algorithm>
#include "common.h"

namespace zxd {
// clamped b-spline
class Spline : public osg::Geometry {
protected:
  osg::ref_ptr<osg::Vec3Array> mControlPoints;
  osg::ref_ptr<osg::DoubleArray> mKnots;
  GLuint mDegree;
  unsigned int mSegments;  // number of segments

public:
  Spline() : mSegments(50) {
    mControlPoints = new osg::Vec3Array;
    mKnots = new osg::DoubleArray;
  }

  void rebuild() {
    if (!valid()) {
      OSG_NOTICE << "invalid b spline. n : " << n() << ", p : " << mDegree
                 << ", m : " << m() << std::endl;
      return;
    }

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    vertices->reserve(mSegments + 1);
    setVertexArray(vertices);

    GLdouble u0 = mKnots->front();
    GLdouble um = mKnots->back();
    float dt = (um - u0) / mSegments;
    for (uint i = 0; i <= mSegments; ++i) {
      vertices->push_back(get(u0 + dt * i));
    }

    setVertexArray(vertices);

    this->removePrimitiveSet(0, this->getNumPrimitiveSets());
    this->addPrimitiveSet(
      new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size()));
  }

  GLuint getDegree() const { return mDegree; }
  void setDegree(GLuint v) { mDegree = v; }
  void updateDegree() { mDegree = m() - n() - 1; }

  osg::ref_ptr<osg::Vec3Array> getControlPoints() const {
    return mControlPoints;
  }
  void setControlPoints(osg::ref_ptr<osg::Vec3Array> v) { mControlPoints = v; }

  GLfloat getKnot(GLuint index) const { return mKnots->at(index); }
  osg::ref_ptr<osg::DoubleArray> getKnots() const { return mKnots; }
  void setKnots(osg::ref_ptr<osg::DoubleArray> v) { mKnots = v; }

  unsigned int getSegments() const { return mSegments; }
  void setSegments(unsigned int v) { mSegments = v; }

  // get point by De Boor's algorithm
  osg::Vec3 get(GLdouble u) {
    u = std::max(std::min(u, mKnots->back()), mKnots->front());
    // early check
    if (u == mKnots->front())
      return mControlPoints->front();
    else if (u == mKnots->back())
      return mControlPoints->back();

    GLuint k = getKnotSpan(u);                                   // knot span
    GLuint s = u == mKnots->at(k) ? getKnotMultiplicity(k) : 0;  // multiplicity
    GLuint p = mDegree;
    // GLuint pMinusS = p - s;
    GLuint kMinusS = k - s;
    GLuint kMinusP = k - p;

    if (s >= p) {
      // when s>=p it's just  P_k,  P_k-1 P_k-2......P_k-p+1, P_k-p,   P_k-p
      return mControlPoints->at(k - p);
    } else {
      // use de Boor's algoritm to create a knot of multiplicity p to get point

      // init columns 0
      osg::ref_ptr<osg::Vec3Array> points =
        new osg::Vec3Array(mControlPoints->begin() + kMinusP,
          mControlPoints->begin() + (kMinusS + 1));
      GLuint h = points->size();
      // iterate p-s times. leave only 1 element at last column
      for (unsigned int r = 1; r < h; ++r) {
        for (unsigned int i = 0; i < h - r; ++i) {
          GLfloat a = getKnotRatio(u, kMinusP + i + r, r);
          points->at(i) = points->at(i) * (1 - a) + points->at(i + 1) * a;
        }
      }
      return points->at(0);
    }
  }

  // compute pointer directly by b-spline formula
  osg::Vec3 getByCoefs(GLdouble u) {
    osg::ref_ptr<osg::DoubleArray> coefs = computeCoefficients(u);
    osg::Vec3 p;
    for (unsigned int i = 0; i < mControlPoints->size(); ++i) {
      p += mControlPoints->at(i) * coefs->at(i);
    }
    return p;
  }

  void insertKnot(GLdouble u, GLuint times = 1) {
    osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array();

    GLuint k = getKnotSpan(u);                                   // knot span
    GLuint s = u == mKnots->at(k) ? getKnotMultiplicity(k) : 0;  // multiplicity
    GLuint p = mDegree;
    GLuint kMinusP = k - p;
    // GLuint pMinusS = p - s;
    GLuint kMinusS = k - s;

    if (s >= p) {
      // most simple case, just repeat P_k_p multiple times
      osg::Vec3Array::iterator iter = points->begin() + kMinusP;
      points->insert(iter, times, *iter);
    } else {
      // seperate insert to two partes if s + times is greater than p
      GLuint h = s + times >= p ? p - s : times;
      GLuint beg = k - p;
      GLuint end = k - s + 1;

      // iterate related points to generate new points
      Vec3ArrayVec vec = iterate(beg, end, u, h);

      // construct new control points by 5 parts
      // part 1, original [0,k-p]
      points->insert(points->end(), mControlPoints->begin(),
        mControlPoints->begin() + kMinusP + 1);

      // part 2, upper edge of point iterations
      for (unsigned int i = 1; i < vec.size() - 1; ++i)
        points->push_back(vec[i]->front());

      // part 3, right edge of point iterations
      points->insert(points->end(), vec.back()->begin(), vec.back()->end());

      // part 4, down edge of point iterations
      for (unsigned int i = vec.size() - 2; i >= 1; --i)
        points->push_back(vec[i]->back());

      // part 5, original [k-s,n]
      points->insert(points->end(), mControlPoints->begin() + kMinusS,
        mControlPoints->end());

      // second part of this condition
      if (s + times >= p) {
        osg::Vec3Array::iterator iter = points->begin() + kMinusP;
        points->insert(iter, times - (p - s), *iter);
      }

      mControlPoints->assign(points->begin(), points->end());
    }

    mKnots->insert(mKnots->begin() + k, times, u);
  }

  GLuint m() { return mKnots->size() - 1; }
  GLuint n() { return mControlPoints->size() - 1; }
  GLuint p() { return mDegree; }
  bool valid() { return m() == n() + p() + 1; }

  // a knot span is [u_k, u_k+1), there are at most m-2p spans in clamped b
  // spline
  GLuint getKnotSpan(GLdouble u) {
    // special case. Is it right to put um at the last valid span [u_m-p-1, u_m) ????
    if (u == mKnots->back()) return mKnots->size() - mDegree - 2;

    osg::DoubleArray::iterator iter =
      std::upper_bound(mKnots->begin(), mKnots->end(), u);
    if (iter == mKnots->end()) {
      OSG_FATAL << "illigal u" << std::endl;
    }
    return iter == mKnots->end() ? -1 : (iter - mKnots->begin()) - 1;
  }

  GLdouble getMinKnot() { return mKnots->front(); }
  GLdouble getMaxKnot() { return mKnots->back(); }

  GLuint getKnotMultiplicity(GLuint i) {
    GLuint s = 1;
    int i2 = i;
    GLdouble u = mKnots->at(i);
    GLuint size = mKnots->size();

    while (++i < size && mKnots->at(i) == u) ++s;
    while (--i2 >= 0 && mKnots->at(i2) == u) ++s;

    return s;
  }

  void subdivide(GLdouble u, Spline& lc, Spline& rc) {
    GLuint k = getKnotSpan(u);                                   // knot span
    GLuint s = u == mKnots->at(k) ? getKnotMultiplicity(k) : 0;  // multiplicity
    GLuint p = mDegree;
    GLuint kMinusP = k - p;
    // GLuint pMinusS = p - s;
    GLuint kMinusS = k - s;

    Vec3ArrayVec vec = iterate(kMinusP, kMinusS + 1, u, p - s);
    if (vec.back()->size() != 1) {
      OSG_NOTICE << "error, not enough iterations!" << std::endl;
      return;
    }

    osg::Vec3Array* points = lc.getControlPoints();
    osg::DoubleArray* knots = lc.getKnots();
    points->clear();
    knots->clear();

    // part 1, original [0,k-p]
    points->insert(points->end(), mControlPoints->begin(),
      mControlPoints->begin() + kMinusP + 1);

    // part 2, upper edge of point iterations
    for (unsigned int i = 1; i < vec.size(); ++i)
      points->push_back(vec[i]->front());

    knots->assign(mKnots->begin(), mKnots->begin() + (k - s + 1));
    knots->insert(knots->end(), p + 1, u);

    points = rc.getControlPoints();
    knots = rc.getKnots();
    points->clear();
    knots->clear();

    // part 1, down edge of point iterations
    for (int i = vec.size() - 1; i > 0; --i) points->push_back(vec[i]->back());

    // part 2, original [k-s,n]
    points->insert(
      points->end(), mControlPoints->begin() + kMinusS, mControlPoints->end());
    knots->insert(knots->end(), p + 1, u);
    knots->insert(knots->end(), mKnots->begin() + k + 1, mKnots->end());
  }

  Vec3ArrayVec iterate(GLuint beg, GLuint end, GLdouble u, GLuint times) {
    Vec3ArrayVec vec;

    // init columns 0
    vec.push_back(new osg::Vec3Array);
    vec[0]->assign(
      mControlPoints->begin() + beg, mControlPoints->begin() + end);

    GLuint size = end - beg;

    for (GLuint r = 1; r <= times; ++r) {
      // init current column
      vec.push_back(new osg::Vec3Array);

      for (unsigned int i = 0; i < size - r; ++i) {
        GLfloat a = getKnotRatio(u, beg + i + r, r);
        osg::Vec3 p = vec[r - 1]->at(i) * (1 - a) + vec[r - 1]->at(i + 1) * a;
        vec[r]->push_back(p);
      }
    }
    return vec;
  }

protected:
  ~Spline() {}

  osg::ref_ptr<osg::DoubleArray> computeCoefficients(GLdouble u) {
    osg::ref_ptr<osg::DoubleArray> result = new osg::DoubleArray;
    osg::DoubleArray& coefs = *result;

    // init to 0
    coefs.insert(coefs.end(), mDegree + 1, 0);

    // only at most p+1 non zero basis function exists in this span, our job is
    // to find them, only them, others are just 0;

    // rule out special case when k belong to [0,p] or [m-p, m]
    if (u == mKnots->front()) {
      coefs[0] = 1;
      return result;
    } else if (u == mKnots->back()) {
      coefs[n()] = 1;
      return result;
    }

    GLuint p = this->p();
    GLuint k = getKnotSpan(u);

    coefs[k] = 0;  // degree 0 coefficients
    GLuint kplus1 = k + 1;
    GLdouble uk = mKnots->at(k);
    GLdouble ukplus1 = mKnots->at(kplus1);
    GLdouble ukplus1MinusU = ukplus1 - u;
    GLdouble uMinusUk = u - uk;

    // loop from degree 1 to degree p
    for (unsigned int d = 1; d <= p; ++d) {
      // right (south-west corner) term only
      coefs[k - d] =
        ukplus1MinusU * coefs[k - d + 1] / (ukplus1 - mKnots->at(k - d + 1));

      // both temes
      for (unsigned int i = k - d + 1; i < k - 1; ++i) {
        GLdouble ui = mKnots->at(i);
        GLdouble uiplusd = mKnots->at(i + d);
        GLdouble uiplus1 = mKnots->at(i + 1);
        GLdouble uiplusdplus1 = mKnots->at(i + d + 1);

        coefs[i] = (u - ui) * coefs[i] / (uiplusd - ui) +
                   (uiplusdplus1 - u) * coefs[i + 1] / (uiplusdplus1 - uiplus1);
      }

      // left (north-west corner) term only
      coefs[k] = uMinusUk * coefs[k] / (mKnots->at(k + d) - k);
    }
    // according to partion of unity
    // GLfloat c = 0;
    // std::for_each(coefs.begin() + (k - p), coefs.begin() + k,
    //[&](decltype(*coefs.begin()) v) {
    // c+= v;
    //});
    // coefs[k] = 1 - c;
    return result;
  }

  GLfloat getKnotRatio(GLdouble t, GLuint i, GLuint r = 1) {
    return (t - mKnots->at(i)) /
           (mKnots->at(i + mDegree - r + 1) - mKnots->at(i));
  }
};
}

#endif /* SPLINE_H */
