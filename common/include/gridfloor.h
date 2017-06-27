#ifndef GRIDFLOOR_H
#define GRIDFLOOR_H

#include <osg/Geometry>
#include <osg/Geode>

namespace zxd {

/*
 * basic grid is drawn with GL_QUADS with 1,1 polygon offset
 * subdivisions and axis are plain GL_LINES
 */
class GridFloor : public osg::Geode {
protected:
  GLuint mLines;  // it's actually tiles,  named after blender style
  GLfloat mScale;
  GLuint mSubdivisions;
  GLubyte mAxesMask;  // 111 as xyz visible
  osg::ref_ptr<osg::Geometry> mGrid;
  osg::ref_ptr<osg::Geometry> mGridSubdivisions;
  osg::ref_ptr<osg::Geometry> mAxisX;
  osg::ref_ptr<osg::Geometry> mAxisY;
  osg::ref_ptr<osg::Geometry> mAxisZ;

  osg::Vec4 mGridLineColor;
  osg::Vec4 mGridSubdivisionColor;

public:
  GridFloor();
  GridFloor(const GridFloor& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::Geode(copy, copyop) {}
  ~GridFloor() {}
  META_Object(zxd, GridFloor);

  GLuint getLines() const { return mLines; }
  void setLines(GLuint v) { mLines = v; }

  GLfloat getScale() const { return mScale; }
  void setScale(GLfloat v) { mScale = v; }

  GLuint getSubDivisions() const { return mSubdivisions; }
  void setSubDivisions(GLuint v) { mSubdivisions = v; }

  GLubyte getAxesMask() const { return mAxesMask; }
  void setAxesMask(GLubyte v) { mAxesMask = v; }

  osg::Vec4 getGridLineColor() const { return mGridLineColor; }
  void setGridLineColor(osg::Vec4 v) { mGridLineColor = v; }

  osg::Vec4 getGridSubdivisionColor() const { return mGridSubdivisionColor; }
  void setGridSubdivisionColor(osg::Vec4 v) { mGridSubdivisionColor = v; }

  void rebuild();

protected:
  void rebuildGrid();
  void rebuildSubdivisions();
  osg::Geometry* createAxis(const osg::Vec3& axis, const osg::Vec4& color);
};
}

#endif /* GRIDFLOOR_H */
