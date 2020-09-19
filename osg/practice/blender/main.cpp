#include <osgViewer/CompositeViewer>
#include "blender.h"
#include <osgDB/ReadFile>

int main(int argc, char* argv[]) {
  osg::ArgumentParser ap(&argc, argv);
  ap.getApplicationUsage()->setApplicationName(ap.getApplicationName());
  ap.getApplicationUsage()->setCommandLineUsage(
    ap.getApplicationName() + " [options] filename ...");

  // load object
  osg::ref_ptr<osg::Node> node = osgDB::readNodeFiles(ap);
  if (!node) node = osgDB::readNodeFile("cow.osgt");

  osg::ref_ptr<zxd::Blender> blender = zxd::Blender::instance();

  osgViewer::CompositeViewer viewer;

  blender->init(&viewer, node);
  osg::DisplaySettings::instance()->setNumMultiSamples(4);

  return viewer.run();
}
