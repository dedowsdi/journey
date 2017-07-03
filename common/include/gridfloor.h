#ifndef GRIDFLOOR_H
#define GRIDFLOOR_H

#include <osg/Geometry>
#include <osg/Geode>

namespace zxd {

/*
 * basic grid is drawn with GL_QUADS with 1,1 polygon offset
 * subdivisions and axis are plain GL_LINES.
 * There is no direct way to avoid z fight between subdivision line and axis
 * line. So if axis lines exists, corresponding subdivision line won't exists.
 */
class GridFloor : public osg::Geode {
protected:
  bool mShowRowAxis;
  bool mShowColAxis;
  bool mShowNormalAxis;

  GLuint mLines;  // it's actually tiles,  named after blender style
  GLfloat mScale;
  GLuint mSubdivisions;
  GLubyte mAxesMask;  // 111 as xyz visible

  osg::Vec3 mRowVector;
  osg::Vec3 mColVector;
  osg::Vec3 mNormalVector;  // row ^ col

  osg::Vec4 mRowAxisColor;
  osg::Vec4 mColAxisColor;
  osg::Vec4 mNormalAxisColor;

  osg::ref_ptr<osg::Geometry> mGrid;
  osg::ref_ptr<osg::Geometry> mGridSubdivisions;
  osg::ref_ptr<osg::Geometry> mAxisRow;
  osg::ref_ptr<osg::Geometry> mAxisCol;
  osg::ref_ptr<osg::Geometry> mAxisNormal;

  osg::Vec4 mGridLineColor;
  osg::Vec4 mGridSubdivisionColor;

public:
  GridFloor(const osg::Vec3& rowVector, const osg::Vec3& colVector);

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

  bool getShowRowAxis() const { return mShowRowAxis; }
  void setShowRowAxis(bool v) { mShowRowAxis = v; }

  bool getShowColAxis() const { return mShowColAxis; }
  void setShowColAxis(bool v) { mShowColAxis = v; }

  bool getShowNormalAxis() const { return mShowNormalAxis; }
  void setShowNormalAxis(bool v) { mShowNormalAxis = v; }

  const osg::Vec4& getRowAxisColor() const { return mRowAxisColor; }
  void setRowAxisColor(const osg::Vec4& v) { mRowAxisColor = v; }

  const osg::Vec4& getColAxisColor() const { return mColAxisColor; }
  void setColAxisColor(const osg::Vec4& v) { mColAxisColor = v; }

  const osg::Vec4& getNormalAxisColor() const { return mNormalAxisColor; }
  void setNormalAxisColor(const osg::Vec4& v) { mNormalAxisColor = v; }

  inline GLuint getNumTiles() { return mLines - mLines % 2; }
  inline float getSize() { return getNumTiles() * mScale; }
  inline float getHalfSize() { return getSize() * 0.5f; }

protected:
  void rebuildGrid();
  void rebuildSubdivisions();
  osg::Geometry* createAxis(const osg::Vec3& axis, const osg::Vec4& color);
};
}

#endif /* GRIDFLOOR_H */
