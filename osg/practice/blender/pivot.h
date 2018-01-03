#ifndef PIVOT_H
#define PIVOT_H

#include <osg/Vec3>
#include <stdexcept>
#include <osg/Object>
#include <osgText/Text>

namespace zxd {

class Blender;

/*
 * blender pivot, only active element and cursor now.
 */
class Pivot : public osg::Referenced {
public:
  enum PivotType { PT_ACTIVEELEMENT, PT_3DCURSOR };

protected:
  PivotType mType;
  Blender* mBlender;
  osgText::Text* mText;

public:
  Pivot(PivotType type = PT_ACTIVEELEMENT) : mType(type) {}

  PivotType getType() const { return mType; }
  void setType(PivotType v);

  Blender* getBlender() const { return mBlender; }
  void setBlender(Blender* v) { mBlender = v; }

  // always return world space pivot
  osg::Vec3 getPivot();

  osgText::Text* getText() const { return mText; }
  void setText(osgText::Text* v) { mText = v; }

  void updateText();
};

std::string toString(zxd::Pivot::PivotType type);
}

#endif /* PIVOT_H */
