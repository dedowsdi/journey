#include "pivot.h"
#include "blender.h"

namespace zxd {

//------------------------------------------------------------------------------
void Pivot::setType(PivotType v) {
  if (mType == v) return;

  mType = v;
  updateText();
  mBlender->reputPivotAxes();
}

//------------------------------------------------------------------------------
osg::Vec3 Pivot::getPivot() {
  osg::Vec3 pivot;
  switch (mType) {
    case zxd::Pivot::PT_ACTIVEELEMENT: {
      pivot = mBlender->getCurObject()->getMatrix().getTrans();
    } break;
    case zxd::Pivot::PT_3DCURSOR:
      pivot = mBlender->getCursor()->getMatrix().getTrans();
      break;
    default:
      throw std::runtime_error("unknown pivot type");
  }
  return pivot;
}

//------------------------------------------------------------------------------
void Pivot::updateText() { mText->setText(toString(mType)); }

//------------------------------------------------------------------------------
std::string toString(zxd::Pivot::PivotType type) {
  switch (type) {
    case zxd::Pivot::PT_ACTIVEELEMENT:
      return "Active element";
      break;
    case zxd::Pivot::PT_3DCURSOR:
      return "3D Cursor";
    default:
      throw std::runtime_error("undefined pivot type");
  }
}
}
