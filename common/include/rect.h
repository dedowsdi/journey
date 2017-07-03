#ifndef RECT_H
#define RECT_H

#include <osg/Vec2>

namespace zxd {

// simple 2d rect
class Rect {
protected:
  float mLeft;
  float mRight;
  float mBottom;
  float mTop;

public:
  Rect(float left, float right, float bottom, float top)
      : mLeft(left), mRight(right), mBottom(bottom), mTop(top) {}

  float getLeft() const { return mLeft; }
  void setLeft(float v) { mLeft = v; }

  float getRight() const { return mRight; }
  void setRight(float v) { mRight = v; }

  float getBottom() const { return mBottom; }
  void setBottom(float v) { mBottom = v; }

  float getTop() const { return mTop; }
  void setTop(float v) { mTop = v; }

  bool contain(const osg::Vec2& p) {return contain(p[0], p[1]);}
  bool contain(float x, float y){
    return x >= mLeft && x < mRight && y >= mBottom && y < mTop;
  };
};
}

#endif /* RECT_H */
