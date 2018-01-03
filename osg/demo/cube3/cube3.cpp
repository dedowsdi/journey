#include <osgViewer/Viewer>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgUtil/SmoothingVisitor>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/FrameStamp>

class LightNodeCallback : public osg::NodeCallback {
public:
  LightNodeCallback():mFrameStamp(0) {}
  osg::FrameStamp* getFrameStamp() const { return mFrameStamp; }
  void setFrameStamp( osg::FrameStamp* v){mFrameStamp = v;}

  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

    //if(mFrameStamp){
      //OSG_NOTICE << "reference time : " << mFrameStamp->getReferenceTime() << std::endl;
      //OSG_NOTICE << "simulation time : " << mFrameStamp->getSimulationTime() << std::endl;
    //}
    osg::MatrixTransform* mt = node->asTransform()->asMatrixTransform();
    mt->setMatrix(osg::Matrix::rotate(osg::PI * mFrameStamp->getReferenceTime(), osg::Z_AXIS));
  }

private:
  osg::FrameStamp* mFrameStamp;
};

osg::ref_ptr<osg::Geometry> createCube(const osg::Vec3& halfSize) {
  osg::ref_ptr<osg::Geometry> cube = new osg::Geometry();

  osg::ref_ptr<osg::Vec3Array> va = new osg::Vec3Array();

  osg::Vec3 x = osg::X_AXIS * halfSize.x();
  osg::Vec3 y = osg::Y_AXIS * halfSize.y();
  osg::Vec3 z = osg::Z_AXIS * halfSize.z();

  // front
  va->push_back(-y);
  va->push_back(x);
  va->push_back(z);
  // back
  va->push_back(y);
  va->push_back(-x);
  va->push_back(z);
  // right
  va->push_back(x);
  va->push_back(-y);
  va->push_back(z);
  // left
  va->push_back(-x);
  va->push_back(y);
  va->push_back(z);
  // top
  va->push_back(z);
  va->push_back(x);
  va->push_back(y);
  // bottom
  va->push_back(-z);
  va->push_back(-x);
  va->push_back(y);

  osg::ref_ptr<osg::Vec3Array> positions = new osg::Vec3Array();

  osg::ref_ptr<osg::DrawElementsUInt> indices =
    new osg::DrawElementsUInt(GL_TRIANGLES);

  for (GLuint i = 0; i < va->size();) {
    const osg::Vec3& center = va->at(i++);
    const osg::Vec3& right = va->at(i++);
    const osg::Vec3& top = va->at(i++);

    // create 2 triangles in counter clock wise order
    positions->push_back(center - right + top);
    positions->push_back(center - right - top);
    positions->push_back(center + right - top);
    positions->push_back(center + right + top);

    GLuint startIndex = positions->size() - 4;

    indices->push_back(startIndex);
    indices->push_back(startIndex + 1);
    indices->push_back(startIndex + 2);

    indices->push_back(startIndex);
    indices->push_back(startIndex + 2);
    indices->push_back(startIndex + 3);
  }

  //cube->setVertexArray(positions);
  cube->setVertexAttribArray(0, positions);
  cube->addPrimitiveSet(indices.get());

  // generate smooth normal
  osg::ref_ptr<osgUtil::SmoothingVisitor> sv = new osgUtil::SmoothingVisitor();
  cube->accept(*sv);


  return cube;
}

int main(int argc, char* argv[]) {
  osgViewer::Viewer viewer;

  osg::ref_ptr<osg::Geode> modelNode = new osg::Geode();
  osg::ref_ptr<osg::Geometry> cube = createCube(osg::Vec3(1, 1, 1));

  modelNode->addDrawable(cube);

  // create a simple light, circle  arround origin
  osg::ref_ptr<osg::Light> light = new osg::Light();
  light->setDiffuse(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
  light->setSpecular(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
  light->setPosition( osg::Vec4(3.0f,0.0f,0.0f,1.0f) );

  osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
  lightSource->setLight(light);

  osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
  transform->addChild(lightSource);

  osg::ref_ptr<LightNodeCallback> cb = new LightNodeCallback();
  cb->setFrameStamp(viewer.getViewerFrameStamp());
  transform->setUpdateCallback(cb.get());

  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->addChild(modelNode);
  root->addChild(transform);

  viewer.setSceneData(root);
  return viewer.run();
}
