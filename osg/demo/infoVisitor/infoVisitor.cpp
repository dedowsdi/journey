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
              << " : " << &node
              << (node.getName().empty() ? "" : " : " + node.getName())
              << std::endl;
    _level++;
    traverse(node);
    _level--;
  }

private:
  std::string spaces() { return std::string(_level * 2, ' '); }

protected:
  unsigned int _level;
};

int main(int argc, char* argv[]) {
  auto root = new osg::Group;
  root->setName("root");
  auto cessna = osgDB::readNodeFile("cessna.osg");
  cessna->setName("cessna");

  root->addChild(cessna);
  root->addChild(cessna);

  InfoVisitor infoVisitor;
  root->accept(infoVisitor);

  osgViewer::Viewer viewer;
  viewer.setSceneData(root);
  return viewer.run();

}
