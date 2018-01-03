#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

int main(int argc, char *argv[]) {
  osg::DisplaySettings::instance()->setNumMultiSamples(4);

  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("cessna.osg");
  osgViewer::Viewer viewer;
  viewer.setSceneData(model.get());
  return viewer.run();

}
