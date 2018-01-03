#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

int main(int argc, char *argv[]) {

  //get model name, if you don't need --model, you can pass parser to
  //readNodeFile directly
  std::string filename; 
  osg::ArgumentParser parser(&argc, argv);
  parser.read("--model", filename);

  //log
  osg::notify(osg::NOTICE) << "load model "  << filename << std::endl;

  osgViewer::Viewer viewer;
	viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(osgDB::readNodeFile(filename));

  return viewer.run();
}
