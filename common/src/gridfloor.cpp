#include "gridfloor.h"
#include <osg/PolygonMode>
#include <algorithm>
#include <osg/PolygonOffset>

namespace zxd {

//------------------------------------------------------------------------------
GridFloor::GridFloor(const osg::Vec3& rowVector, const osg::Vec3& colVector)
    : mLines(16),
      mScale(1.0f),
      mSubdivisions(4),
      mAxesMask(0b110),
      mGridLineColor(0.35f, 0.35f, 0.35f, 1.0f),
      mGridSubdivisionColor(0.5f, 0.5f, 0.5f, 1.0f),
      mRowVector(rowVector),
      mColVector(colVector),
      mRowAxisColor(osg::Vec4(rowVector, 1.0f)),
      mColAxisColor(osg::Vec4(colVector, 1.0f)),
      mShowRowAxis(true),
      mShowColAxis(true),
      mShowNormalAxis(false)

{
  osg::StateSet* ss = getOrCreateStateSet();
  ss->setAttributeAndModes(new osg::PolygonMode(
    osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));

  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

//------------------------------------------------------------------------------
void GridFloor::rebuild() {
  while (this->getNumDrawables() != 0) removeDrawable(0);

  // reset all geometry
  mAxisRow = createAxis(mRowVector, mRowAxisColor);
  mAxisCol = createAxis(mColVector, mColAxisColor);
  mAxisNormal = createAxis(mNormalVector, mNormalAxisColor);

  if (mShowRowAxis) addDrawable(mAxisRow);
  if (mShowColAxis) addDrawable(mAxisCol);
  if (mShowNormalAxis) addDrawable(mAxisNormal);

  rebuildGrid();
  rebuildSubdivisions();
}

//------------------------------------------------------------------------------
void GridFloor::rebuildGrid() {
  // build grid as quads
  mGrid = new osg::Geometry;

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->push_back(mGridLineColor);

  GLint numGirdTiles = getNumTiles();  // downcast to even
  GLint numGridLines = numGirdTiles + 1;

  GLfloat tileSize = mScale;
  osg::Vec3 stepRow = mColVector * tileSize;
  osg::Vec3 stepCol = mRowVector * tileSize;

  float halfSize = numGirdTiles * tileSize / 2.0f;
  osg::Vec3 leftBottom = mRowVector * -halfSize + mColVector * -halfSize;

  for (int row = 0; row < numGridLines; ++row) {
    osg::Vec3 startCol = leftBottom + stepRow * row;
    for (int col = 0; col < numGridLines; ++col) {
      vertices->push_back(startCol + stepCol * col);
    }
  }

  // create quad elements
  osg::ref_ptr<osg::DrawElementsUShort> elements =
    new osg::DrawElementsUShort();
  elements->setMode(GL_QUADS);

  for (int row = 0; row < numGirdTiles; ++row) {
    GLuint lineIndex = numGridLines * row;
    GLuint nextLineIndex = lineIndex + numGridLines;
    for (int col = 0; col < numGirdTiles; ++col) {
      elements->push_back(lineIndex + col);
      elements->push_back(lineIndex + col + 1);
      elements->push_back(nextLineIndex + col + 1);
      elements->push_back(nextLineIndex + col);
    }
  }

  mGrid->setVertexArray(vertices);
  mGrid->setColorArray(colors);
  mGrid->setColorBinding(osg::Geometry::BIND_OVERALL);
  mGrid->addPrimitiveSet(elements);

  addDrawable(mGrid);

  osg::StateSet* ss = mGrid->getOrCreateStateSet();
  ss->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f));
}

//------------------------------------------------------------------------------
void GridFloor::rebuildSubdivisions() {
  mGridSubdivisions = new osg::Geometry;

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->push_back(mGridSubdivisionColor);

  GLint numGirdTiles = getNumTiles();  // downcast to even
  // GLint numGridLines = numGirdTiles + 1;
  GLint numSubTile = numGirdTiles / mSubdivisions;
  numSubTile -= numSubTile % 2;  // must be even number
  GLint numSubLines = numSubTile + 1;
  GLint numHalfSubLines = numSubLines / 2;
  GLint numHalfSubTile = numSubTile / 2;

  GLfloat tileSize = mScale;
  GLfloat subdivisionSize = mScale * mSubdivisions;

  osg::Vec3 stepRow = mColVector * subdivisionSize;
  osg::Vec3 stepCol = mRowVector * subdivisionSize;

  float halfSize = numGirdTiles * tileSize / 2.0f;
  // osg::Vec3 leftBottom = mRowVector * -halfSize + mColVector * -halfSize;
  osg::Vec3 left = mRowVector * -halfSize;
  osg::Vec3 bottom = mColVector * -halfSize;
  osg::Vec3 width = -left * 2;
  osg::Vec3 height = -bottom * 2;

  osg::Vec3 rowBottom = stepRow * -numHalfSubTile;
  osg::Vec3 colLeft = stepCol * -numHalfSubTile;

  osg::Vec3 v0, v1;
  for (int i = 0; i < numSubLines; ++i) {
    /*
     * there is no way to avoid  z fight between lines. If axis exists,
     * corresponding subline in the same position will not exist
     */

    // row
    if (i != numHalfSubLines || !mShowRowAxis) {
      {
        v0 = rowBottom + stepRow * i + left;
        v1 = v0 + width;
        vertices->push_back(v0);
        vertices->push_back(v1);
      }

      if (i != numHalfSubLines || !mShowColAxis) {
        // col
        v0 = colLeft + stepCol * i + bottom;
        v1 = v0 + height;
        vertices->push_back(v0);
        vertices->push_back(v1);
      }
    }

    mGridSubdivisions->setVertexArray(vertices);
    mGridSubdivisions->setColorArray(colors);
    mGridSubdivisions->setColorBinding(osg::Geometry::BIND_OVERALL);

    for (int i = 0; i < numSubLines; ++i) {
      mGridSubdivisions->addPrimitiveSet(
        new osg::DrawArrays(GL_LINES, 0, vertices->size()));
    }

    addDrawable(mGridSubdivisions);
  }
}

//------------------------------------------------------------------------------
osg::Geometry* GridFloor::createAxis(
  const osg::Vec3& axis, const osg::Vec4& color) {
  GLfloat halfSize = mLines * mScale / 2;

  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  vertices->push_back(axis * halfSize);
  vertices->push_back(-axis * halfSize);
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->push_back(color);

  geometry->setVertexArray(vertices);
  geometry->setColorArray(colors);
  geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
  geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));

  return geometry.release();
}
}
