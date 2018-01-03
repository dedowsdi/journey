#include <osgDB/ReadFile>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>

#include "common.h"

class TwoDimManipulator : public osgGA::StandardManipulator {
public:
  TwoDimManipulator() : _distance(1.0) {}

  virtual osg::Matrixd getMatrix() const {
    osg::Matrixd matrix;
    matrix.makeTranslate(0.0f, 0.0f, _distance);
    matrix.postMultTranslate(_center);
    return matrix;
  }
  virtual osg::Matrixd getInverseMatrix() const {
    osg::Matrixd matrix;
    matrix.makeTranslate(0.0f, 0.0f, -_distance);
    matrix.preMultTranslate(-_center);
    return matrix;
  }
  virtual void setByMatrix(const osg::Matrixd& matrix) {
    setByInverseMatrix(osg::Matrixd::inverse(matrix));
  }
  virtual void setByInverseMatrix(const osg::Matrixd& matrix) {
    osg::Vec3d eye, center, up;
    matrix.getLookAt(eye, center, up);
    _center = center;
    _center.z() = 0.0f;
    if (_node.valid())
      _distance = std::abs((_node->getBound().center() - eye).z());
    else
      _distance = std::abs((eye - center).length());
  }

  virtual void setTransformation(const osg::Vec3d&, const osg::Quat&) {}
  virtual void setTransformation(
    const osg::Vec3d&, const osg::Vec3d&, const osg::Vec3d&) {}
  virtual void getTransformation(osg::Vec3d&, osg::Quat&) const {}
  virtual void getTransformation(osg::Vec3d&, osg::Vec3d&, osg::Vec3d&) const {}

  virtual void home(double) {
    if (_node.valid()) {
      _center = _node->getBound().center();
      _center.z() = 0.0f;
      _distance = 2.5 * _node->getBound().radius();
    } else {
      _center.set(osg::Vec3());
      _distance = 1.0;
    }
  }
  virtual void home(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
    home(ea.getTime());
  }

protected:
  virtual ~TwoDimManipulator() {}

  virtual bool performMovementLeftMouseButton(
    const double eventTimeDelta, const double dx, const double dy) {
    _center.x() -= 100.0f * dx;
    _center.y() -= 100.0f * dy;
    return false;
  }
  virtual bool performMovementRightMouseButton(
    const double eventTimeDelta, const double dx, const double dy) {
    _distance *= (1.0 + dy);
    if (_distance < 1.0) _distance = 1.0;
    return false;
  }

  osg::Vec3 _center;
  double _distance;
};

int main(int argc, char** argv) {
  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(osgDB::readNodeFile("lz.osg"));

  osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keySwitch =
    new osgGA::KeySwitchMatrixManipulator;
  keySwitch->addMatrixManipulator(
    '1', "Trackball", new osgGA::TrackballManipulator);
  keySwitch->addMatrixManipulator('2', "TwoDim", new TwoDimManipulator);

  const osg::BoundingSphere& bs = root->getBound();
  keySwitch->setHomePosition(
    bs.center() + osg::Vec3(0.0f, 0.0f, bs.radius()), bs.center(), osg::Y_AXIS);

  osgViewer::Viewer viewer;
  viewer.setCameraManipulator(keySwitch.get());
  viewer.setSceneData(root.get());
  return viewer.run();
}
