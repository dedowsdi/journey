/*
 * pivot in AnimationPathCallback works like this:
 *    final transform = translate(-pivot) * rotation() * translate(pivot)
 * when rotation() is identity, transform = identity, the animating object
 * always pass local identify tranform position, so if you want to create
 * animation around certain point, you must make sure identify transform is on
 * the path of the animatin.
 */
#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/MatrixTransform>
#include <osg/AnimationPath>

osg::ref_ptr<osg::MatrixTransform> createDrawableTransform(
    osg::ref_ptr<osg::ShapeDrawable> drawable, const osg::Vec3& pos){

  osg::ref_ptr<osg::MatrixTransform> tf = new osg::MatrixTransform();
  tf->setMatrix(osg::Matrix::translate(pos));
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(drawable);
  tf->addChild(leaf);
  return tf;

}

int main(int argc, char *argv[])
{
  osg::ref_ptr<osg::Shape> sphereShape = new osg::Sphere();
  osg::ref_ptr<osg::Shape> cylinderShape = new osg::Cylinder(osg::Vec3(), 0.4f, 15.0f);

  osg::ref_ptr<osg::ShapeDrawable> sphereDrawable = new osg::ShapeDrawable(sphereShape);
  osg::ref_ptr<osg::ShapeDrawable> cylinderDrawable = new osg::ShapeDrawable(cylinderShape);

  osg::ref_ptr<osg::MatrixTransform> tf0 = createDrawableTransform(sphereDrawable, osg::Vec3(0, 0, 0));
  tf0->addUpdateCallback(
      new osg::AnimationPathCallback(osg::Vec3(-5, 0, 0), osg::Vec3(0, 0, 1), 3));
  osg::ref_ptr<osg::MatrixTransform> sphere0 = new osg::MatrixTransform();
  sphere0->setMatrix(osg::Matrix::translate(osg::Vec3(5,0,0)));
  sphere0->addChild(tf0);

  osg::ref_ptr<osg::MatrixTransform> tf1 = createDrawableTransform(sphereDrawable, osg::Vec3(0, 0, 0));
  tf1->addUpdateCallback(
      new osg::AnimationPathCallback(osg::Vec3(5, 0, 0), osg::Vec3(0, 0, 1), 3));
  osg::ref_ptr<osg::MatrixTransform> sphere1 = new osg::MatrixTransform();
  sphere1->setMatrix(osg::Matrix::translate(osg::Vec3(0,0,3)));
  sphere1->addChild(tf1);

  osg::ref_ptr<osg::MatrixTransform> tf2 = createDrawableTransform(sphereDrawable, osg::Vec3(0, 0, 0));
  tf2->addUpdateCallback(
      new osg::AnimationPathCallback(osg::Vec3(-5, 0, 0), osg::Vec3(0, 0, 2), 3));
  osg::ref_ptr<osg::MatrixTransform> sphere2 = new osg::MatrixTransform();
  sphere2->setMatrix(osg::Matrix::translate(osg::Vec3(0,0,-3)));
  sphere2->addChild(tf2);

  osg::ref_ptr<osg::Geode> cylinder = new osg::Geode();
  cylinder->addDrawable(cylinderDrawable);

  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->addChild(sphere0);
  root->addChild(sphere1);
  root->addChild(sphere2);
  root->addChild(cylinder);

  osgViewer::Viewer viewer;
  viewer.setSceneData(root);

  return viewer.run();
}
