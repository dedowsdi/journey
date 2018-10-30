#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/MatrixTransform>
#include "glm.h"

#define CUBE_COUNT 21
#define CUBE_SIZE 20
#define PERIOD 0.8
#define MAX_SCALE_Z 30.0

osg::ref_ptr<osg::ShapeDrawable> cube;
osg::ref_ptr<osg::Group> root;

void createCube()
{
  cube = new osg::ShapeDrawable;
  osg::ref_ptr<osg::Box> box = new osg::Box();
  box->setHalfLengths(osg::Vec3(CUBE_SIZE*0.5, CUBE_SIZE*0.5, CUBE_SIZE*0.5));
  cube->setShape(box);
}

osg::AnimationPath* createAnimationPath(const osg::Vec3& translation, GLfloat offset)
{
  osg::AnimationPath* ap = new osg::AnimationPath;
  // manual swing with offset
  ap->setLoopMode(osg::AnimationPath::LOOP);

  float h = glm::mix(1.0, MAX_SCALE_Z, offset);
  float t = 0;
  ap->insert(t, osg::AnimationPath::ControlPoint(translation, osg::Quat(), osg::Vec3(1.0, 1.0, h)));
  t += PERIOD*(1-offset);
  ap->insert(t, osg::AnimationPath::ControlPoint(translation, osg::Quat(), osg::Vec3(1.0, 1.0, MAX_SCALE_Z)));
  t += PERIOD;
  ap->insert(t, osg::AnimationPath::ControlPoint(translation, osg::Quat(), osg::Vec3(1.0, 1.0, 1)));
  t += PERIOD*offset;
  ap->insert(t, osg::AnimationPath::ControlPoint(translation, osg::Quat(), osg::Vec3(1.0, 1.0, h)));
  
  return ap;
}

void createWave()
{
  GLfloat offset = -CUBE_SIZE * CUBE_COUNT / 2.0  + CUBE_SIZE * .5;
  osg::Vec3 corner(offset, offset, 0);

  osg::Vec2 sin_center = osg::Vec2((CUBE_COUNT - 1)/2.0, (CUBE_COUNT - 1)/2.0);
  GLfloat max_distance = sin_center.length();
  for (int y = 0; y < CUBE_COUNT; ++y) 
  {
    for (int x = 0; x < CUBE_COUNT; ++x) 
    {
      osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
      root->addChild(mt);
      osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
      mt->addChild(leaf);
      leaf->addDrawable(cube);

      osg::Vec3 translatrion = corner + osg::Vec3(x*CUBE_SIZE, y*CUBE_SIZE, 0);
      mt->setMatrix(osg::Matrix::translate(translatrion));
      float sin_offset = (osg::Vec2(x, y) - sin_center).length() / max_distance;
      mt->addUpdateCallback(new osg::AnimationPathCallback(createAnimationPath(translatrion, sin_offset)));
    }
  }
}


int main(int argc, char* argv[])
{
  root = new osg::Group;
  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
  ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

  createCube();
  createWave();


  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);
  
  return viewer.run();
}
