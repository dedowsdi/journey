#include "gridfloor.h"
#include <osg/PolygonMode>
#include <algorithm>
#include <osg/PolygonOffset>

namespace zxd {

//------------------------------------------------------------------------------
GridFloor::GridFloor()
    : mLines(16),
      mScale(1.0f),
      mSubdivisions(4),
      mAxesMask(0b110),
      mGridLineColor(0.35f, 0.35f, 0.35f, 1.0f),
      mGridSubdivisionColor(0.45f, 0.45f, 0.45f, 1.0f) {
  osg::StateSet* ss = getOrCreateStateSet();
  ss->setAttributeAndModes(new osg::PolygonMode(
    osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));

  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

//------------------------------------------------------------------------------
void GridFloor::rebuild() {
  // create axes
  if (!mAxisX) {
    mAxisX = createAxis(osg::X_AXIS, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    mAxisY = createAxis(osg::Y_AXIS, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    mAxisZ = createAxis(osg::Z_AXIS, osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    addDrawable(mAxisX);
    addDrawable(mAxisY);
    addDrawable(mAxisZ);
  }

  // reset axes visibility
  mAxisX->setNodeMask(mAxesMask & 0b100 ? -1 : 0);
  mAxisY->setNodeMask(mAxesMask & 0b010 ? -1 : 0);
  mAxisZ->setNodeMask(mAxesMask & 0b001 ? -1 : 0);

  rebuildGrid();
  rebuildSubdivisions();
}

//------------------------------------------------------------------------------
void GridFloor::rebuildGrid() {
  if (mGrid) removeDrawable(mGrid);

  // build grid as quads
  mGrid = new osg::Geometry;

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->push_back(mGridLineColor);

  GLint numGirdTiles = mLines - mLines % 2;  // downcast to even
  GLint numGridLines = numGirdTiles + 1;

  GLfloat tileSize = mScale;
  GLfloat left = -numGirdTiles * tileSize / 2;
  GLfloat bottom = left;

  for (int row = 0; row < numGridLines; ++row) {
    GLfloat startY = bottom + row * tileSize;
    for (int col = 0; col < numGridLines; ++col) {
      vertices->push_back(osg::Vec3(left + col * tileSize, startY, 0));
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
  // build subdivisions as lines, exnore center lines if xy axes are visible.
  if (mGridSubdivisions) removeDrawable(mGridSubdivisions);

  mGridSubdivisions = new osg::Geometry;

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->push_back(mGridSubdivisionColor);

  GLint numGirdTiles = mLines - mLines % 2;  // downcast to even
  GLint numGridLines = numGirdTiles + 1;
  GLint numSubTile = numGirdTiles / mSubdivisions;

  GLfloat tileSize = mScale;
  GLfloat subdivisionSize = mScale * mSubdivisions;
  GLfloat left = -numGirdTiles * tileSize / 2;
  GLfloat bottom = left;

  GLint halfSubLines = numSubTile / 2 + 1;

  for (int i = 0; i < halfSubLines; ++i) {
    // row
    if (i != 0 || !(mAxesMask & 0b100)) {
      for (int j = 0; j < numGridLines; ++j)
        vertices->push_back(
          osg::Vec3(left + j * tileSize, i * subdivisionSize, 0));
    }
    // flip
    if (i != 0)
      std::transform(vertices->end() - numGridLines, vertices->end(),
        std::back_inserter(*vertices),
        [&](decltype(*vertices->begin()) v) -> osg::Vec3 {
          return osg::Vec3(v.x(), -v.y(), v.z());
        });

    // col
    if (i != 0 || !(mAxesMask & 0b010)) {
      for (int j = 0; j < numGridLines; ++j)
        vertices->push_back(
          osg::Vec3(i * subdivisionSize, bottom + j * tileSize, 0));
    }
    // flip
    if (i != 0)
      std::transform(vertices->end() - numGridLines, vertices->end(),
        std::back_inserter(*vertices),
        [&](decltype(*vertices->begin()) v) -> osg::Vec3 {
          return osg::Vec3(-v.x(), v.y(), v.z());
        });
  }

  mGridSubdivisions->setVertexArray(vertices);
  mGridSubdivisions->setColorArray(colors);
  mGridSubdivisions->setColorBinding(osg::Geometry::BIND_OVERALL);

  GLint numSubLines = vertices->size() / numGridLines;

  for (int i = 0; i < numSubLines; ++i) {
    mGridSubdivisions->addPrimitiveSet(
      new osg::DrawArrays(GL_LINE_STRIP, numGridLines * i, numGridLines));
  }

  addDrawable(mGridSubdivisions);
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
