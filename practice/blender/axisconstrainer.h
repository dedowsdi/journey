#ifndef AXISCONSTRAINER_H
#define AXISCONSTRAINER_H

#include <osg/Group>
#include "common.h"
#include "blenderobject.h"

namespace zxd {

class AxisConstrainer : public osg::Group {
public:
  enum ConstrainType {
    CT_NONE,  // 0
    CT_X,     // 1
    CT_Y,     // 2
    CT_Z,     // 3
    CT_YZ,    // 4
    CT_ZX,    // 5
    CT_XY     // 6
  };
  enum ConstrainFrame { CF_LOCAL, CF_WORLD };

protected:
  ConstrainType mConstrainType;
  ConstrainFrame mConstrainFrame;
  osg::ref_ptr<osg::MatrixTransform> mTarget;
  osg::Matrix mPivot;  // current object initial transform

public:
  AxisConstrainer() {}
  AxisConstrainer(const AxisConstrainer& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::Group(copy, copyop) {}
  ~AxisConstrainer() {}
  META_Object(zxd, AxisConstrainer);

  ConstrainType getConstrainType() const { return mConstrainType; }
  void setConstrainType(ConstrainType v) {
    mConstrainType = v;
    resetConstrainLines();
  }

  ConstrainFrame getConstrainFrame() const { return mConstrainFrame; }
  void setConstrainFrame(ConstrainFrame v) {
    mConstrainFrame = v;
    resetConstrainLines();
  }

  const osg::ref_ptr<osg::MatrixTransform>& getTarget() const {
    return mTarget;
  }

  void setTarget(const osg::ref_ptr<osg::MatrixTransform>& v) {
    mTarget = v;
    mPivot = mTarget->getMatrix();
  }

  // type should be x y or z
  osg::Vec3 getConstrainAxis(ConstrainType type);

  // none => world x => local x => none
  void addConstrain(ConstrainType type) {
    if (mConstrainType == CT_NONE)  // none => x
      setConstrainType(type);
    else if (mConstrainFrame == CF_WORLD)  // world => local
      setConstrainFrame(CF_LOCAL);
    else  // yz => none
    {
      setConstrainType(CT_NONE);
      setConstrainFrame(CF_WORLD);
    }
  }

  osg::ref_ptr<zxd::BlenderObject> getTarget() const { return mTarget; }
  void setTarget(osg::ref_ptr<zxd::BlenderObject> v) { mTarget = v; }

protected:
  ~AxisConstrainer();

  void resetConstrainLines() {
    removeChildren(0, getNumChildren());
    osg::ref_ptr<osg::Vec3Array> lines = new osg::Vec3Array();

    switch (mConstrainType) {
      case CT_NONE:
        break;
      case CT_X:
        lines->push_back(osg::X_AXIS);
        break;
      case CT_Y:
        lines->push_back(osg::Y_AXIS);
        break;
      case CT_Z:
        lines->push_back(osg::Z_AXIS);
        break;
      case CT_YZ:
        lines->push_back(osg::Y_AXIS);
        lines->push_back(osg::Z_AXIS);
        break;
      case CT_ZX:
        lines->push_back(osg::X_AXIS);
        lines->push_back(osg::Z_AXIS);
        break;
      case CT_XY:
        lines->push_back(osg::X_AXIS);
        lines->push_back(osg::Y_AXIS);
        break;
    }

    osg::Matrix m = mConstrainFrame == CF_LOCAL
                      ? mPivot
                      : osg::Matrix::translate(mPivot.getTrans());

    std::for_each(
      lines->begin(), lines->end(), [&](decltype(*lines->begin()) v) {
        osg::ref_ptr<zxd::InfiniteLine> il = new zxd::InfiniteLine(v, v * 0.9f);
        il->setMatrix(m);
        addChild(il);
      });
  }
};
}

#endif /* AXISCONSTRAINER_H */
