/*
 * basic blender grab rotate scale
 */
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osgFX/Outline>
#include "auxiliary.h"
#include <osgGA/OrbitManipulator>
#include <osgUtil/IntersectVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osg/ValueObject>
#include "blendermanipulator.h"
#include "objectoperation.h"
#include "axisconstrainer.h"
#include "blenderobject.h"

osg::Camera* camera;
osg::Camera* hudCamera;
osg::ref_ptr<zxd::ObjectOperation> currentOperation;
osg::ref_ptr<zxd::BlenderObject> currentObject;
osg::ref_ptr<osg::Group> root;
osgViewer::Viewer* viewer;

osg::Vec4 outlineColor = osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f);

class GrabEventHandler : public osgGA::GUIEventHandler {
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_X:
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    return false;  // return true will stop event
  }
};

class BlendGuiEventhandler : public osgGA::GUIEventHandler {
  osg::ref_ptr<zxd::ObjectOperation> operation;
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_G:
            createNewOperation<zxd::GrabOperation>(ea);
            break;
          case osgGA::GUIEventAdapter::KEY_S:
            createNewOperation<zxd::ScaleOperation>(ea);
            break;
          case osgGA::GUIEventAdapter::KEY_R:
            createNewOperation<zxd::RotateOperation>(ea);
            break;
          default:
            break;
        }
        break;
      default:
        break;

      case osgGA::GUIEventAdapter::PUSH:
        if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) {
          // select current object
          // pick object
          osgUtil::IntersectionVisitor iv;
          osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi =
            new osgUtil::LineSegmentIntersector(
              osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
          iv.setIntersector(lsi);

          camera->accept(iv);

          if (lsi->containsIntersections()) {
            const osgUtil::LineSegmentIntersector::Intersection& result =
              *lsi->getIntersections().begin();

            for (GLuint i = 0; i < result.nodePath.size(); ++i) {
              osg::Node* node = result.nodePath[i];
              bool isBLenderObject = false;
              if (node->getUserValue("BlenderObject", isBLenderObject)) {
                if (currentObject && currentObject != node)
                  currentObject->deselect();
                currentObject = static_cast<BlenderObject*>(node);
                currentObject->select();
                break;
              }
            }

          } else {
            // do nothing if nothing hitten
            // currentObject->deselect();
          }
        }
    }
    return false;  // return true will stop event
  }

  template <typename T>
  void createNewOperation(const osgGA::GUIEventAdapter& ea) {
    if (currentOperation) {
      // do nothing if curreng operation is the same tyep as T
      if (dynamic_cast<T*>(*currentOperation) != NULL) return;

      // create new operation, use current axis constrainer
      osg::ref_ptr<zxd::AxisConstrainer> ac =
        currentOperation->getAxisConstrainer();

      currentOperation->cancel();

      currentOperation = new T();
      currentOperation->setAxisConstrainer(ac);

    } else {
      currentOperation = new T();
      currentOperation->setAxisConstrainer(new zxd::AxisConstrainer());
    }
    currentOperation->setTarget(currentObject);
    currentOperation->setStartCursor(osg::Vec2(ea.getX(), ea.getY()));
    root->addChild(currentOperation->getAxisConstrainer());

    osg::Node* handle = currentOperation->getHandle() ;
    if(handle)
      root->addChild(handle);
  }
};

int main(int argc, char* argv[]) {
  osg::ArgumentParser ap(&argc, argv);
  ap.getApplicationUsage()->setApplicationName(ap.getApplicationName());
  ap.getApplicationUsage()->setCommandLineUsage(
    ap.getApplicationName() + " [options] filename ...");

  // load object
  osg::ref_ptr<osg::Node> node = osgDB::readNodeFiles(ap);
  if (!node) node = osgDB::readNodeFile("cessna.osg");

  root = new osg::Group;

  osg::ref_ptr<zxd::BlenderObject> bo = new zxd::BlenderObject();
  bo->getModel()->addChild(node);
  root->addChild(bo);

  osgViewer::Viewer v;
  viewer = &v;
  viewer->addEventHandler(new osgViewer::StatsHandler);
  viewer->setSceneData(root);
  osg::ref_ptr<zxd::BlenderManipulator> manipulator =
    new zxd::BlenderManipulator();
  viewer->setCameraManipulator(manipulator);
  camera = viewer->getCamera();

  camera->setCullingMode(
    camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);

  viewer->addEventHandler(new BlendGuiEventhandler);

  return viewer->run();
}
