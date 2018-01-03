#include <osgViewer/Viewer>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include "auxiliary.h"
#include <osg/AutoTransform>
#include "common.h"
#include "auxiliary.h"
#include "zmath.h"

// mini axes size
float axisSize = 50.0f;

// copy world axes in target camera rotation
class MiniAxesCallback : public osg::NodeCallback {
protected:
  osg::Camera* mTargetCamera;

public:
  osg::Camera* getTargetCamera() const { return mTargetCamera; }
  void setTargetCamera(osg::Camera* v) { mTargetCamera = v; }

protected:
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    osg::MatrixTransform* mt = node->asTransform()->asMatrixTransform();

    const osg::Matrix& targetView = mTargetCamera->getViewMatrix();

    osg::Matrix m = mt->getMatrix();
    osg::Vec3 translation;
    osg::Quat rotation;
    osg::Vec3 scale;
    osg::Quat so;
    m.decompose(translation, rotation, scale, so);

    // copy rotation, reserve translation and scale
    m = osg::Matrix::scale(scale) * zxd::Math::getMatR(targetView) *
        osg::Matrix::translate(translation);
    mt->setMatrix(m);

    traverse(node, nv);
  }
};

int main(int argc, char* argv[]) {
  // axes in 3d world
  osg::ref_ptr<osg::Group> root = new osg::Group();
  osg::ref_ptr<zxd::Axes> axes = new zxd::Axes();
  axes->setMatrix(osg::Matrix::scale(osg::Vec3(100.0f, 100.0f, 100.0f)));
  root->addChild(axes);

  // miniaxes in hud, copy rotation from main camera.
  osg::Camera* hudCamera =
    zxd::createHUDCamera(0, -axisSize * 1.5, axisSize * 1.5);
  osg::ref_ptr<zxd::Axes> axes2 = new zxd::Axes();
  axes2->setLabel(true);
  axes2->setMatrix(
    osg::Matrix::scale(osg::Vec3(axisSize, axisSize, axisSize)) *
    osg::Matrix::translate(osg::Vec3(axisSize * 1.35, axisSize * 1.35, 0.0f)));
  hudCamera->addChild(axes2);
  hudCamera->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);

  root->addChild(hudCamera);

  osgViewer::Viewer viewer;

  osg::ref_ptr<MiniAxesCallback> macb = new MiniAxesCallback();
  axes2->setUpdateCallback(macb);
  macb->setTargetCamera(viewer.getCamera());

  viewer.setSceneData(root);
  return viewer.run();
}
