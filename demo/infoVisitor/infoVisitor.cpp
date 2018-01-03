#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <iostream>

class InfoVisitor : public osg::NodeVisitor {
public:
  InfoVisitor() : _level(0) {
    setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
  }
  virtual void apply(osg::Node& node) {
    std::cout << spaces() << node.libraryName() << "::" << node.className()
              << std::endl;
    _level++;
    traverse(node);
    _level--;
  }
  
  //i don't think it's necessary to do override osg::Geode, because drawable is
  //also descendent of node.
  
  //virtual void apply(osg::Geode& geode) {
    //std::cout << spaces() << geode.libraryName() << "::" << geode.className()
              //<< std::endl;
    //_level++;
    //print drawable
    //for (unsigned int i = 0; i < geode.getNumDrawables(); ++i) {
      //osg::Drawable* drawable = geode.getDrawable(i);
      //std::cout << spaces() << drawable->libraryName()
                //<< "::" << drawable->className() << std::endl;
    //}
    //traverse(geode);
    //_level--;
  //}

private:
  std::string spaces() { return std::string(_level * 2, ' '); }

protected:
  unsigned int _level;
};

int main(int argc, char* argv[]) {
  osg::ArgumentParser arguments(&argc, argv);
  osg::ref_ptr<osg::Node> root = osgDB::readNodeFiles(arguments);
  if (!root) {
    OSG_FATAL << arguments.getApplicationName() << ": No data loaded."
              << std::endl;
    return -1;
  }

  InfoVisitor infoVisitor;
  root->accept(infoVisitor);

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();

}
