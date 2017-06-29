#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include "auxiliary.h"
#include <osgGA/FlightManipulator>
#include <osg/AutoTransform>
#include "common.h"
#include <osgGA/TrackballManipulator>

int main(int argc, char* argv[]) {
  // create graphics context
  osg::ref_ptr<osg::GraphicsContext::Traits> traits =
    new osg::GraphicsContext::Traits();

  GLuint width = 800, height = 600;
  osg::GraphicsContext::WindowingSystemInterface* wsi =
    osg::GraphicsContext::getWindowingSystemInterface();
  if (!wsi) OSG_FATAL << "failed to get window system interface " << std::endl;
  wsi->getScreenResolution(
    osg::GraphicsContext::ScreenIdentifier(0), width, height);

  traits->x = 0;
  traits->y = 0;
  traits->width = width;
  traits->height = height;
  traits->doubleBuffer = true;
  traits->sharedContext = 0;
  traits->windowDecoration = false;
  // traits->alpha = 8;

  osg::GraphicsContext* gc =
    osg::GraphicsContext::createGraphicsContext(traits);
  if (!gc) OSG_FATAL << "failed to creaate graphics context " << std::endl;

  osg::ref_ptr<osg::Group> root = new osg::Group();

  osgViewer::Viewer viewer;
  osg::Camera* camera = viewer.getCamera();
  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setAllowEventFocus(false);  // no event
  camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  camera->setProjectionMatrix(
    osg::Matrix::ortho(0, width, 0, height, -300, 300));
  camera->setViewMatrix(osg::Matrix::identity());
  camera->setGraphicsContext(gc);
  camera->setViewport(0, 0, width, height);
  osg::ref_ptr<osgText::Text> text = zxd::createText(osg::Vec3(), "text", 20);

  camera->addChild(text);

  /*
   * compute near far doesn't include text, it must be canceled
   */
  camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
  //camera->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);

  // osg::Camera* camera = zxd::createHUDCamera();
  // camera->setCullingActive(false);
  // camera->setCullingMode(camera->getCullingMode() &
  // ~osg::CullSettings::SMALL_FEATURE_CULLING);
  camera->setCullingMode(osg::CullSettings::VIEW_FRUSTUM_CULLING);

  osg::ref_ptr<zxd::Axes> axes = new zxd::Axes;
  axes->setLabel(true);
  axes->setMatrix(osg::Matrix::scale(200.0f, 200.0f, 200.0f) *
                  osg::Matrix::rotate(osg::PI_4, osg::X_AXIS) *
                  // osg::Matrix::rotate(-osg::PI_4, osg::Z_AXIS) *
                  osg::Matrix::translate(200.0f, 200.0f, 0));

  root->addChild(axes);
  viewer.setSceneData(root);

  // no cam man
  while (!viewer.done()) viewer.frame();
}
