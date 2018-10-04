#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/MatrixTransform>
#include "glm.h"

osg::ref_ptr<osg::Group> root;

struct Branch
{
  GLfloat len;
  osg::Matrix transform;

  Branch* left = 0;
  Branch* right = 0;

  Branch(GLfloat l):
    len(l)
  {
  }

  ~Branch()
  {
    if(left) delete left;
    if(right) delete right;
  }

  void spawn(GLfloat minBranchLen, const osg::Vec2& lenScaleRange, const osg::Vec2& leftRotateRange, const osg::Vec2& rightRotateRange)
  {

    GLfloat l = glm::linearRand(lenScaleRange.x(), lenScaleRange.y()) * len;
    if(l >= minBranchLen) 
    {
      GLfloat lr = glm::linearRand(leftRotateRange.x(), leftRotateRange.y());
      left = new Branch(l);
      left->transform = osg::Matrix::rotate(lr, glm::pza) * osg::Matrix::translate(osg::Vec3(0, len, 0));
      left->spawn(minBranchLen, lenScaleRange, leftRotateRange, rightRotateRange);
    }

    l = glm::linearRand(lenScaleRange.x(), lenScaleRange.y()) * len;
    if(l >= minBranchLen) 
    {
      GLfloat rr = glm::linearRand(rightRotateRange.x(), rightRotateRange.y());
      right = new Branch(l);
      right->transform = osg::Matrix::rotate(rr, glm::pza) * osg::Matrix::translate(osg::Vec3(0, len, 0));
      right->spawn(minBranchLen, lenScaleRange, leftRotateRange, rightRotateRange);
    }

  }

  void compile(osg::Vec2Array& vertices, osg::Matrix parent_transform)
  {
    parent_transform = transform * parent_transform;
    osg::Vec4 v0 = osg::Vec4(0,0,0,1) * parent_transform;
    osg::Vec4 v1 = osg::Vec4(0,len,0,1) * parent_transform;
    vertices.push_back(osg::Vec2(v0.x(), v0.y()));
    vertices.push_back(osg::Vec2(v1.x(), v1.y()));

    if(left)
      left->compile(vertices, parent_transform);

    if(right)
      right->compile(vertices, parent_transform);
  }

};

class FractalTree : public osg::Geometry
{
protected:
  Branch* root = 0;
  GLfloat mTrunkLen;
  GLfloat mMinBranchLen;
  osg::Vec2 mLenScaleRange = osg::Vec2(0.6, 0.6);
  osg::Vec2 mLeftRotateRange = osg::Vec2(glm::fpi4, glm::fpi4);
  osg::Vec2 mRightRotateRange = osg::Vec2(-glm::fpi4, -glm::fpi4);

public:

  ~FractalTree()
  {
    if(root)
      delete root;
  }

  FractalTree(GLfloat trunkLen = 1, GLfloat minBranchLen = 0.1):
    mTrunkLen(trunkLen),
    mMinBranchLen(minBranchLen)
  {
  }

  void rebuild()
  {
    if(root)
      delete root;

    root = new Branch(mTrunkLen);
    root->spawn(mMinBranchLen, mLenScaleRange, mLeftRotateRange, mRightRotateRange);
  }

  void compile()
  {
    GLuint spawn_times = std::ceil(std::log2(mTrunkLen/ mMinBranchLen));
    GLuint max_branches = std::pow(2, spawn_times + 1) - 1;

    osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array();
    setVertexArray(vertices);
    vertices->reserve(max_branches * 2);

    Branch* b = root;
    osg::Matrix m = osg::Matrix::identity();
    b->compile(*vertices, m);

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
    colors->push_back(osg::Vec4(1, 1, 1, 1));
    setColorArray(colors);

    removePrimitiveSet(0, getNumPrimitiveSets());
    addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, vertices->size()));
  }

  GLfloat trunkLen() const { return mTrunkLen; }
  void trunkLen(GLfloat v){ mTrunkLen = v; }

  GLfloat minBranchLen() const { return mMinBranchLen; }
  void minBranchLen(GLfloat v){ mMinBranchLen = v; }

  const osg::Vec2& lenScaleRange() const { return mLenScaleRange; }
  void lenScaleRange(const osg::Vec2& v){ mLenScaleRange = v; }

  const osg::Vec2& leftRotateRange() const { return mLeftRotateRange; }
  void leftRotateRange(const osg::Vec2& v){ mLeftRotateRange = v; }

  const osg::Vec2& rightRotateRange() const { return mRightRotateRange; }
  void rightRotateRange(const osg::Vec2& v){ mRightRotateRange = v; }
};

int main(int argc, char* argv[])
{
  root = new osg::Group;
  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

  osg::ref_ptr<FractalTree> tree  = new FractalTree;
  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();

  leaf->addDrawable(tree);
  mt->addChild(leaf);
  root->addChild(mt);
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  viewer.realize();

  // setup  camera
  GLuint width, height;
  osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
  if(!wsi)
  {
    OSG_FATAL << "faled to get window system interface";
  }
  wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);

  // setup tree style
  mt->setMatrix(osg::Matrix::translate(osg::Vec3(width*0.5, 0, 0)));
  tree->trunkLen(height * 0.4);
  tree->minBranchLen(2);
  tree->rebuild();
  tree->compile();

  osg::Camera* camera = viewer.getCamera();
  camera->setAllowEventFocus(false);
  camera->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
  camera->setProjectionResizePolicy(osg::Camera::FIXED);
  camera->setProjectionMatrixAsOrtho2D(0, width, 0, height);
  camera->setViewMatrix(osg::Matrix::identity());

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);
  
  return viewer.run();
}
