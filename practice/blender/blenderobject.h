#ifndef BLENDEROBJECT_H
#define BLENDEROBJECT_H
namespace zxd {

#include <osg/MatrixTransform>

class BlenderObject : public osg::MatrixTransform {
protected:
  osg::ref_ptr<osg::Switch> mSwitch;      // 0 normal, 1 outline
  osg::ref_ptr<osgFX::Outline> mOutline;  // 2nd child of switch
  osg::ref_ptr<osg::Group> mModel;        // direct parent of loaded file node
  osg::ref_ptr<osg::Group> mLoadPoint;

public:
  BlenderObject() {
    mOutline = new osgFX::Outline;
    mOutline->setColor(outlineColor);
    osg::ref_ptr<osg::Group> plain = new osg::Group;  // paralleo to outline
    mSwitch = new osg::Switch;
    mModel = new osg::Group;
    mLoadPoint = new osg::Group;

    addChild(mSwitch);
    addChild(mLoadPoint);

    mSwitch->addChild(plain, 1);
    mSwitch->addChild(mOutline, 0);

    plain->addChild(mModel);
    mOutline->addChild(mModel);

    // used to identify this is a blender object
    this->setUserValue("BlenderObject", true);
  }
  BlenderObject(const BlenderObject& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~BlenderObject() {}
  META_Object(zxd, BlenderObject);

  void select() {
    mSwitch->setValue(0, 0);
    mSwitch->setValue(1, 1);

    mLoadPoint->removeChildren(0, mLoadPoint->getNumChildren());
    mLoadPoint->addChild(new zxd::Axes(camera));
    mLoadPoint->addChild(new zxd::Dot);
  }

  void deselect() {
    mSwitch->setValue(0, 1);
    mSwitch->setValue(1, 0);
    mLoadPoint->removeChildren(0, mLoadPoint->getNumChildren());
  }

  osg::ref_ptr<osg::Group> getModel() const { return mModel; }
};
}

#endif /* BLENDEROBJECT_H */
