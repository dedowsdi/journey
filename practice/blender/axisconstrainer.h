#ifndef AXISCONSTRAINER_H
#define AXISCONSTRAINER_H

#include <osg/Group>
#include "common.h"
#include "blenderobject.h"
#include <algorithm>

namespace zxd {

class AxisConstrainer : public osg::MatrixTransform {
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
  ConstrainType mType;
  ConstrainFrame mFrame;

public:
  AxisConstrainer() : mType(CT_NONE), mFrame(CF_WORLD) {}
  AxisConstrainer(const AxisConstrainer& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~AxisConstrainer() {}
  META_Object(zxd, AxisConstrainer);

  ConstrainType getType() const { return mType; }
  void settype(ConstrainType v) {
    mType = v;
    resetConstrainLines();
  }

  ConstrainFrame getFrame() const { return mFrame; }
  void setFrame(ConstrainFrame v) {
    mFrame = v;
    resetConstrainLines();
  }

  // none => world x => local x => none
  void addConstrain(ConstrainType type) {
    if (mType != type || mType == CT_NONE) {
      // none => world
      setFrame(CF_WORLD);
      settype(type);
    } else if (mFrame == CF_WORLD) {
      // world => local
      setFrame(CF_LOCAL);
    } else {
      // local => none
      setFrame(CF_WORLD);
      settype(CT_NONE);
    }

    OSG_NOTICE << "change constrain type  to " << mType << std::endl;
    OSG_NOTICE << "change constrain frame  to " << mFrame << std::endl;
  }

protected:
  // recreate lines, reset rotations, but leave translation alone
  void resetConstrainLines() {
    removeChildren(0, getNumChildren());
    osg::ref_ptr<osg::Vec3Array> lines = new osg::Vec3Array();

    switch (mType) {
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

    std::for_each(
      lines->begin(), lines->end(), [&](decltype(*lines->begin()) v) {
        osg::ref_ptr<zxd::InfiniteLine> il = new zxd::InfiniteLine(v, v * 0.9f);
        addChild(il);
      });
  }
};

std::string toString(AxisConstrainer::ConstrainFrame frame);
}

#endif /* AXISCONSTRAINER_H */
