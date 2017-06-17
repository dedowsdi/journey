/* -*-c++-*- OpenSceneGraph Cookbook
 * Chapter 2 Recipe 6
 * Author: Wang Rui <wangray84 at gmail dot com>
*/

#include <osg/Group>
#include <deque>
#include <osgDB/ReadFile>
#include <osgUtil/PrintVisitor>
#include <iostream>
#include <osg/NodeVisitor>

class BFSVisitor : public osg::NodeVisitor {
protected:
  std::deque<osg::Node*> _pendingNodes;  // first in first out

public:
  BFSVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
  virtual void reset() { _pendingNodes.clear(); }
  // if you change osg::Node to osg::Group,  you might need to override both
  // osg::Node and osg::Group in descendent class
  virtual void apply(osg::Node& node) { traverseBFS(node); }

protected:
  ~BFSVisitor() {}

  void traverseBFS(osg::Node& node) {
    osg::Group* group = node.asGroup();
    if (!group) return;

    for (unsigned int i = 0; i < group->getNumChildren(); ++i) {
      // push to the back of the queue
      _pendingNodes.push_back(group->getChild(i));
    }

    while (_pendingNodes.size() > 0) {
      osg::Node* node = _pendingNodes.front();
      _pendingNodes.pop_front();
      node->accept(*this);
    }
  }
};

class BFSPrintVisitor : public BFSVisitor {
public:
  virtual void apply(osg::Node& node) {
    std::cout << node.libraryName() << "::" << node.className() << std::endl;
    traverseBFS(node);
  }
};

int main(int argc, char** argv) {
  osg::ArgumentParser arguments(&argc, argv);
  osg::ref_ptr<osg::Node> root = osgDB::readNodeFiles(arguments);
  if (!root) root = osgDB::readNodeFile("osgcool.osgt");

  std::cout << "DFS Visitor traversal: " << std::endl;

  osgUtil::PrintVisitor pv(std::cout);
  root->accept(pv);

  std::cout << std::endl;
  std::cout << "BFS Visitor traversal: " << std::endl;

  BFSPrintVisitor bpv;
  root->accept(bpv);
}
