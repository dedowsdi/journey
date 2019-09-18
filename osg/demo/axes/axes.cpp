#include <osgViewer/Viewer>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include "auxiliary.h"
#include <osg/AutoTransform>
#include "common.h"
#include "auxiliary.h"
#include "zmath.h"

// mini axes size
const auto axisSize = 50.0f;

int main(int argc, char* argv[]) {
  // axes in 3d world
  auto root = new osg::Group();
  auto axes = new zxd::Axes();
  axes->setMatrix(osg::Matrix::scale(osg::Vec3(100.0f, 100.0f, 100.0f)));
  root->addChild(axes);

  // miniaxes in hud, copy inverse rotation from main camera.
  osg::Camera* hudCamera =
    zxd::createHudCamera(0, -axisSize * 1.5, axisSize * 1.5);
  osg::ref_ptr<zxd::Axes> axes2 = new zxd::Axes();
  axes2->setLabel(true);
  axes2->setMatrix(
    osg::Matrix::scale(osg::Vec3(axisSize, axisSize, axisSize)) *
    osg::Matrix::translate(osg::Vec3(axisSize * 1.35, axisSize * 1.35, 0.0f)));
  hudCamera->addChild(axes2);
  hudCamera->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);

  root->addChild(hudCamera);

  osgViewer::Viewer viewer;
  axes2->bind(viewer.getCamera());

  viewer.setSceneData(root);
  return viewer.run();
}
