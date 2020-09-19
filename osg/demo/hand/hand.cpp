#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <iomanip>
#include <random>

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Shader>
#include <osg/Program>
#include <osgAnimation/Skeleton>
#include <osgAnimation/Bone>
#include <osgAnimation/Animation>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/UpdateBone>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedQuaternionElement>
#include <osg/Point>
#include <osgUtil/PrintVisitor>
#include <osgAnimation/RigGeometry>
#include <osgAnimation/RigTransformHardware>

GLfloat linearRand(GLfloat min = 0.0f, GLfloat max = 1.0f)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<> dist(0.0f, 1.0f);
  return min + (max - min) * dist(gen);
}

osgAnimation::Bone* createBone(
  const std::string& name, const osg::Matrix& m, osgAnimation::Bone* parent)
{
  auto update = new osgAnimation::UpdateBone(name);
  update->getStackedTransforms().push_back(new osgAnimation::StackedTranslateElement("init_translation", m.getTrans()));
  update->getStackedTransforms().push_back(new osgAnimation::StackedQuaternionElement("init_rotation", m.getRotate()));
  update->getStackedTransforms().push_back(new osgAnimation::StackedQuaternionElement("quat", osg::Quat{}));

  auto bone = new osgAnimation::Bone(name);
  bone->setUpdateCallback(update);

  if (parent)
  {
    parent->addChild(bone);
    std::cout << "add bone : " << name  << " to " << parent->getName()  << std::endl;
  }

  return bone;
}

osgAnimation::Bone* createFinger(const std::string& baseName,
  const osg::Vec3& pos, GLfloat yaw, GLfloat roll, int knuckles, GLfloat knuckleLength,
  osgAnimation::Bone* parent)
{
  auto rootTransform =
    osg::Matrix::rotate(osg::DegreesToRadians(roll), osg::Z_AXIS) *
    osg::Matrix::rotate(osg::DegreesToRadians(yaw), osg::Y_AXIS) *
    osg::Matrix::translate(pos);
  auto parentBone = createBone(baseName + std::to_string(0), rootTransform, parent);
  osgAnimation::Bone* root = parentBone;;

  for (auto i = 1; i <= knuckles; ++i)
  {
    parentBone = createBone(baseName + std::to_string(i),
      osg::Matrix::translate(0, 0, knuckleLength), parentBone);
  }

  return root;
}

osgAnimation::Channel* createAnimChannel(
  const std::string& targetName, GLfloat degreeAngle, GLfloat period)
{
  auto channel = new osgAnimation::QuatSphericalLinearChannel;
  channel->setName("quat");
  channel->setTargetName(targetName);
  auto keyframes = channel->getOrCreateSampler()->getOrCreateKeyframeContainer();

  const auto steps = 4;
  auto stepAngle = osg::DegreesToRadians(degreeAngle) / steps;
  auto stepTime = period / steps;
  for (auto i = 0; i <= steps; ++i)
  {
    keyframes->push_back(osgAnimation::QuatKeyframe(stepTime * i, osg::Quat(stepAngle * i, osg::X_AXIS)));
  }
  std::cout << "add channel : " << targetName << " : " << "quat" << std::endl;
  return channel;
}

void createFingerAnimChannel(osgAnimation::Animation& anim, const std::string& baseName, int knuckles,
  float knuckle0Angle, float knuckle1Angle, float knuckle2Angle, GLfloat period)
{
  assert(knuckles <= 3);

  std::vector<float> angles {knuckle0Angle, knuckle1Angle, knuckle2Angle};
  for (auto i = 0; i < knuckles; ++i)
  {
    anim.addChannel(createAnimChannel(baseName + std::to_string(i), angles[i], period));
  }

}

osg::Node* createDot(GLfloat size, const osg::Vec4& color)
{
  auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
  vertices->push_back(osg::Vec3{});
  auto colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  colors->push_back(color);

  auto geom = new osg::Geometry;
  geom->setVertexArray(vertices);
  geom->setColorArray(colors);
  geom->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));

  auto leaf = new osg::Geode;
  leaf->addDrawable(geom);
  leaf->getOrCreateStateSet()->setAttributeAndModes(new osg::Point(size));

  return leaf;
}

class AttachBoneVisitor : public osg::NodeVisitor
{
public:
  AttachBoneVisitor(osg::Node* node)
      : NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), _node(node)
  {
    setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
  }

  void apply(osg::MatrixTransform& node) override
  {
    auto bone = dynamic_cast<osgAnimation::Bone*>(&node);
    if (bone)
      bone->addChild(_node);

    traverse(node);
  }
private:
  osg::ref_ptr<osg::Node> _node;
};

// update boneMatrixInSkeleton before animation linked
class UpdateBoneVisitor : public osg::NodeVisitor
{
public:
  UpdateBoneVisitor():NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
  {
  }

  void apply(osg::MatrixTransform& node) override
  {
    auto bone = dynamic_cast<osgAnimation::Bone*>(&node);
    if (bone)
    {
      auto updater = dynamic_cast<osgAnimation::UpdateBone*>(bone->getUpdateCallback());
      if (!updater)
        OSG_FATAL << "failed to get UpdateBone from bone : " << bone->getName() << std::endl;

      updater->getStackedTransforms().update();
      auto& m = updater->getStackedTransforms().getMatrix();

      auto parentBone = bone->getBoneParent();

      if (parentBone)
        bone->setMatrixInSkeletonSpace(m * parentBone->getMatrixInSkeletonSpace());
      else
        bone->setMatrixInSkeletonSpace(m);
    }

    traverse(node);
  }
};


// set invBindMatrixInSkeleton to inverse ov MatrixInSkeleton
class InvBindBoneVisitor : public osg::NodeVisitor
{
public:
  InvBindBoneVisitor():NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
  {
  }

  void apply(osg::MatrixTransform& node) override
  {
    auto bone = dynamic_cast<osgAnimation::Bone*>(&node);
    if (bone)
    {
      bone->setInvBindMatrixInSkeletonSpace(osg::Matrix::inverse(bone->getMatrixInSkeletonSpace()));
    }

    traverse(node);
  }
};

// ass cylinder style skin
class SkinVisitor : public osg::NodeVisitor
{
public:
  SkinVisitor():NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
  {
  }

  void apply(osg::MatrixTransform& node) override
  {
    auto bone = dynamic_cast<osgAnimation::Bone*>(&node);
    if (bone)
    {
      auto vertices = static_cast<osg::Vec3Array*>(_skin->getSourceGeometry()->getVertexArray());
      auto vim = _skin->getInfluenceMap();
      (*vim)[bone->getName()].setName(bone->getName());

      auto pos0 = osg::Vec3(bone->getMatrixInSkeletonSpace().getTrans());

      // add vertices from this knuckle to all it's children
      for (auto i = 0; i < bone->getNumChildren(); ++i)
      {
        auto childBone = dynamic_cast<osgAnimation::Bone*>(bone->getChild(i));
        if (!childBone)
          continue;
        auto pos1 = osg::Vec3(childBone->getMatrixInSkeletonSpace().getTrans());

        auto dir = pos1 - pos0;
        auto len = dir.length();
        dir.normalize();

        // translate to bone0, rotate z axis to dir
        auto m =  osg::Matrix::rotate(osg::Z_AXIS, dir) * osg::Matrix::translate(pos0);

        auto vertexSetStart = vertices->size();
        for (auto i = 0; i < _knuckleVertices; ++i)
        {
          auto h = linearRand(0.0f, len);
          auto phi = linearRand(0.0f, osg::PIf);
          osg::Vec3 vertex(cos(phi) * _radius, sin(phi) * _radius, h);
          vertices->push_back(vertex * m);

          (*vim)[bone->getName()].push_back(std::make_pair(vertexSetStart + i, 1.0f));
        }

      }
    }

    traverse(node);
  }

  GLfloat get_radius() const { return _radius; }
  void set_radius(GLfloat v){ _radius = v; }

  GLint get_knuckleVertices() const { return _knuckleVertices; }
  void set_knuckleVertices(GLint v){ _knuckleVertices = v; }

  osgAnimation::RigGeometry* get_skin() const { return _skin; }
  void set_skin(osgAnimation::RigGeometry* v){ _skin = v; }

private:
  osgAnimation::RigGeometry* _skin;
  GLint _knuckleVertices {100};
  GLfloat _radius {1.0f};
};

int main(int argc, char* argv[])
{
  auto skeleton = new osgAnimation::Skeleton;
  auto wrist = createBone("wrist", osg::Matrix{}, 0);
  auto palm = createBone("palm", osg::Matrix::translate(0, 0, 10), wrist);
  createFinger("thumb"       , osg::Vec3(10 , 0, 5)  , 60    , 45  , 2, 4  , wrist);
  createFinger("indexFinger" , osg::Vec3(5  , 0, 6)  , 10.0f , 0.0f, 3, 3.2, palm);
  createFinger("middleFinger", osg::Vec3(0  , 0, 7)  , 0.0f  , 0.0f, 3, 4  , palm);
  createFinger("ringFinger"  , osg::Vec3(-5 , 0, 6)  , -8.0f , 0.0f, 3, 3  , palm);
  createFinger("littleFinger", osg::Vec3(-10, 0, 4.5), -13.0f, 0.0f, 3, 2.5, palm);

  skeleton->addChild(wrist);
  skeleton->setDefaultUpdateCallback();
  skeleton->setInitialBound(osg::BoundingSphere(osg::Vec3(0, 0, 10), 40));

  const float animPeriod = 1.0f;
  const float knuckle0Angle = 50;
  const float knuckle1Angle = 110;
  const float knuckle2Angle = 70;

  auto anim = new osgAnimation::Animation;
  anim->setPlayMode(osgAnimation::Animation::PPONG);
  anim->addChannel(createAnimChannel( "palm", 40, animPeriod) );
  createFingerAnimChannel(*anim, "thumb",        2, knuckle0Angle, knuckle1Angle, knuckle2Angle, animPeriod);
  createFingerAnimChannel(*anim, "indexFinger",  3, knuckle0Angle, knuckle1Angle, knuckle2Angle, animPeriod);
  createFingerAnimChannel(*anim, "middleFinger", 3, knuckle0Angle, knuckle1Angle, knuckle2Angle, animPeriod);
  createFingerAnimChannel(*anim, "ringFinger",   3, knuckle0Angle, knuckle1Angle, knuckle2Angle, animPeriod);
  createFingerAnimChannel(*anim, "littleFinger", 3, knuckle0Angle, knuckle1Angle, knuckle2Angle, animPeriod);

  auto animMgr = new osgAnimation::BasicAnimationManager;
  animMgr->registerAnimation(anim);
  animMgr->playAnimation(anim);
  
  auto root = new osg::Group;
  root->addChild(skeleton);
  root->addUpdateCallback(animMgr);

  AttachBoneVisitor visitor(createDot(5, osg::Vec4(1, 0, 0, 1)));
  wrist->accept(visitor);

  // osgUtil::PrintVisitor printer(std::cout);
  // root->accept(printer);

  UpdateBoneVisitor ubv;
  wrist->accept(ubv);

  InvBindBoneVisitor ibbv;
  wrist->accept(ibbv);

  auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
  auto geometry = new osg::Geometry;
  geometry->setVertexArray(vertices);

  auto skin = new osgAnimation::RigGeometry;
  skin->setRigTransformImplementation(new osgAnimation::RigTransformHardware);
  skin->setSourceGeometry(geometry);
  skin->setSkeleton(skeleton);
  skin->setInfluenceMap(new osgAnimation::VertexInfluenceMap());
  // auto leaf = new osg::Geode;
  // leaf->addDrawable(skin);
  // root->addChild(leaf);

  auto tf = new osg::MatrixTransform;
  // root->addChild(tf);
  // tf->setMatrix(osg::Matrix::translate(osg::Vec3(10, 10, 10)));
  skeleton->addChild(tf);
  // tf->setMatrix(osg::Matrix::translate(osg::Vec3(10, 10, 10)));
  skeleton->setMatrix(osg::Matrix::translate(osg::Vec3(10, 10, 10)));
  {
      auto leaf = new osg::Geode;
      tf->addChild(leaf);
      leaf->addDrawable(skin);
  }

  SkinVisitor sv;
  sv.set_skin(skin);
  wrist->accept(sv);
  geometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, vertices->size()));

  osgViewer::Viewer viewer;
  viewer.setLightingMode(osg::View::NO_LIGHT);
  viewer.setSceneData(root);
  auto camera = viewer.getCamera();
  viewer.getCamera()->setCullingMode(
    camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);

  return viewer.run();
}
