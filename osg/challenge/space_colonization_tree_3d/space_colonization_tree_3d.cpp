#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/Point>
#include <osg/PolygonMode>
#include <unordered_map>
#include <osgUtil/SmoothingVisitor>
#include "glm.h"

#define NUM_LEAVES 300
#define MIN_ATTRACT 10
#define MAX_ATTRACT 150
#define BRANCH_STEP 8
#define ANIM_TIME 5
#define BRANCH_SCALE 0.96f

#define NUM_CYLINDER_FACE 5
#define TRUNK_RADIUS 2.5f
#define BRANCH_RADIUS 0.35f // radius of branch with no children

class SpaceColonizationTree;
struct Leaf;

typedef std::list<Leaf> LeafList;
osg::ref_ptr<osg::Group> root;
osg::ref_ptr<SpaceColonizationTree> tree;
osg::ref_ptr<osg::Geometry> points;
osg::ref_ptr<osgText::Text> text;
GLfloat drawTimer = 0;
LeafList leaves;

struct Leaf
{
  osg::Vec3 pos;
  Leaf(const osg::Vec3& p):
    pos(p)
  {
  }
};

std::ostream& operator<< (std::ostream& os, const osg::Vec3& v)
{
  os << v.x()  << " " << v.y() << " " << v.z();
  return os;
}

std::ostream& operator<< (std::ostream& os, const osg::Vec4& v)
{
  os << v.x()  << " " << v.y() << " " << v.z() << " " << v.w();
  return os;
}


struct Branch
{
  osg::Vec3 pos;
  osg::Vec3 dir;
  GLfloat size = 1.0;
  GLuint num_children = 0;
  Branch* parent;

  Branch(Branch* _parent, const osg::Vec3& _pos, const osg::Vec3& d):
    parent(_parent),
    pos(_pos),
    dir(d)
  {
    if(parent)
    {
      ++parent->num_children;
      size = parent->size * BRANCH_SCALE;
    }
  }

  inline GLfloat distance2(const Leaf& l) const
  {
    return (l.pos - pos).length2();
  }
};

/*
 * Branch needs to store parent Branch pointer, if i used std::vector<Branch>,
 * all pointer will be invalid if vector is reallocated.
 */
typedef std::vector<Branch*> BranchVector;

class SpaceColonizationTree : public osg::Geometry
{
protected:
  GLfloat mMinAtrract;
  GLfloat mMaxAttract;
  GLfloat mBranchStep;
  GLfloat mTrunkRadius;

  BranchVector mBranches;

public:

  ~SpaceColonizationTree()
  {
    for(auto item : mBranches)
    {
      delete item;
    }
    mBranches.clear();
  }

  void accept(LeafList& leaves)
  {
    growToReach(leaves);
    attractedToLeaves(leaves);
  }

  //create root,  grow from root until in attract range of a Leaf
  void growToReach(LeafList& leaves)
  {
    mBranches.clear();
    mBranches.reserve(1024); // 1024 means nothing

    //GLfloat min2 = mMinAtrract * mMinAtrract;
    GLfloat max2 = mMaxAttract * mMaxAttract;

    // create root
    mBranches.push_back(new Branch(0, osg::Vec3(0, 0, 0), osg::Vec3(0,0,1)));

    // current might cause problem if branches changed(space reallocated)
    Branch* current = mBranches.front();
    bool found = false;
    while(true)
    {
      for(const auto& Leaf : leaves)
      {
        if(current->distance2(Leaf) < max2)
        {
          OSG_NOTICE << "Branch " << current->pos << " reached " << Leaf.pos << std::endl;
          found = true;
          break;
        }
      }

      if(found)
        break;

      // grow
      mBranches.push_back(new Branch(current, current->pos + current->dir * mBranchStep, current->dir));
      current = mBranches.back();
      //OSG_NOTICE << "grow to " << current->pos << std::endl;
    }

  }

  void attractedToLeaves(LeafList& leaves)
  {
    GLfloat min2 = mMinAtrract * mMinAtrract;
    GLfloat max2 = mMaxAttract * mMaxAttract;

    // grow from last one in current Branch
    GLuint start_index = mBranches.size() - 1;

    std::unordered_map<Branch*, osg::Vec3> last_new_branches;
    while(true)
    {
      // loop every Leaf, search it's attract Branch
      std::unordered_map<Branch*, osg::Vec3> new_branches;
      for(auto iter = leaves.begin(); iter != leaves.end();)
      {
        Branch* ab = 0;
        GLfloat min_d2 = 0;
        for (int i = start_index; i < mBranches.size(); ++i) {

          Branch* b = mBranches[i];
          
          GLfloat d2 = b->distance2(*iter);

          // skip if it's too far away
          if(d2 > max2)
            continue;

          // remove reached
          if(d2 < min2)
          {
            //OSG_NOTICE << "remove leave " << iter->pos << std::endl;
            iter = leaves.erase(iter);
            break;
          }

          if(ab == 0 || d2 < min_d2)
          {
            ab = b;
            min_d2 = d2;
          }
        }

        if(ab == 0)
        {
          ++iter;
          continue;
        }

        //OSG_NOTICE << "Branch " << ab->pos << " attracted to Leaf " << iter->pos << std::endl;

        // create new attracted Branch
        // all force are normalized, so the densed area attract more branches
        osg::Vec3 force = (iter->pos - ab->pos);
        force.normalize();
        auto it = new_branches.find(ab);
        if(it == new_branches.end())
          new_branches.insert(std::make_pair(ab, ab->dir + force));
        else
          it->second += force;

        ++iter;
      }

      // update Branch grow start index. swap finished Branch(no Branch grown
      // from this one) with start Branch.
      for (int i = start_index; i < mBranches.size(); ++i) {
        if(new_branches.find(mBranches[i]) == new_branches.end())
        {
          if(i != start_index)
            std::swap(mBranches[i], mBranches[start_index]);
          ++start_index;
        }
      }
      //++start_index;
      
      // handle oscillate problems
      if(!last_new_branches.empty())
      {
        for(auto iter = last_new_branches.begin(); iter != last_new_branches.end(); ++iter)
        {
          auto it = std::find_if(new_branches.begin(), new_branches.end(), 
              [&](const std::pair<Branch*, osg::Vec3>& item)->bool {
              return item.first == iter->first && item.second == iter->second;
              });
          if(it != new_branches.end())
          {
            OSG_NOTICE << "found duplicated Branch, deleted" << std::endl;
            new_branches.erase(it);
          }
        }
      }

      last_new_branches = new_branches;

      if(new_branches.empty())
      {
        OSG_NOTICE << "stop growing, " << leaves.size() << " leaves left" << std::endl;
        break;
      }
      else
      {
        //OSG_NOTICE << "generate " << new_branches.size() << " branches " << std::endl;
      }

      // add new attracted Branch
      for(auto iter = new_branches.begin(); iter != new_branches.end(); ++iter){
        osg::Vec3 dir = iter->second;
        dir.normalize();
        osg::Vec3 pos = iter->first->pos + dir * mBranchStep;
        mBranches.push_back(new Branch(iter->first, pos, dir));
        //OSG_NOTICE << "create new Branch " << iter->first << " " << dir  <<  " " <<   (*mBranches.back()).pos << std::endl;
      }

    }
  }

  std::vector<osg::Vec3> createCircle(Branch* b)
  {
    static std::vector<osg::Vec3> unitCircle;
    if(unitCircle.empty())
    {
      unitCircle.reserve(NUM_CYLINDER_FACE * 6);

      osg::Vec3 next(1, 0, 0);
      osg::Matrix step_rotate = osg::Matrix::rotate(glm::f2pi/NUM_CYLINDER_FACE, glm::pza);
      for (int i = 0; i < NUM_CYLINDER_FACE; ++i) 
      {
        unitCircle.push_back(next);
        next = next * step_rotate;
      }
      unitCircle.push_back(unitCircle.front());
    }

    std::vector<osg::Vec3> circle;

    const osg::Vec3& bottomCenter = b->parent ? b->parent->pos : b->pos - osg::Vec3(0,0,1);
    const osg::Vec3& topCenter = b->pos;
    osg::Vec3 cylinderAxis = topCenter- bottomCenter;
    cylinderAxis.normalize();

    GLfloat xyscale = b->num_children == 0 ? 
      BRANCH_RADIUS : std::max(b->size * TRUNK_RADIUS, BRANCH_RADIUS);

    osg::Matrix m = osg::Matrix::rotate(glm::pza, cylinderAxis);
    m = osg::Matrix::scale(osg::Vec3(xyscale, xyscale, 1)) * m * osg::Matrix::translate(topCenter);

    std::for_each(unitCircle.begin(), unitCircle.end(), 
        [&](const osg::Vec3& v)->void {
          circle.push_back(v * m);
        });

    return circle;
  }

  void compile()
  {
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    setVertexArray(vertices);
    vertices->reserve(mBranches.size() * NUM_CYLINDER_FACE * 6);


    // there will be gaps between Branch and Branch
    for (int i = 0; i < mBranches.size(); ++i) {
      Branch* b = mBranches[i];
      
      if(!b->parent)
        continue;

      std::vector<osg::Vec3> bottomCircle = createCircle(b->parent);
      std::vector<osg::Vec3> topCircle = createCircle(b);

      // need a better way to control xyscale
      //GLfloat xyscale = glm::mix(trunkRadius, BRANCH_RADIUS,
          //glm::smoothstep(0.0, 1.0, static_cast<GLdouble>(i)/mBranches.size()));
      //OSG_NOTICE << xyscale << std::endl;

      for (int i = 0; i < bottomCircle.size() - 1; ++i) {
        vertices->push_back(bottomCircle[i]);
        vertices->push_back(bottomCircle[i+1]);
        vertices->push_back(topCircle[i+1]);

        vertices->push_back(bottomCircle[i]);
        vertices->push_back(topCircle[i+1]);
        vertices->push_back(topCircle[i]);
      }
    }

    removePrimitiveSet(0, getNumPrimitiveSets());
    addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, vertices->size()));

    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    leaf->addDrawable(this);
    osgUtil::SmoothingVisitor sm;
    leaf->accept(sm);

    OSG_NOTICE << "generate " << vertices->size() << " vertices" << std::endl;
  }

  GLuint numBranches(){return mBranches.size() - 1;} // real Branch count, not point

  GLfloat minAttract() const { return mMinAtrract; }
  void minAttract(GLfloat v){ mMinAtrract = v; }

  GLfloat maxAttract() const { return mMaxAttract; }
  void maxAttract(GLfloat v){ mMaxAttract = v; }

  GLfloat branchStep() const { return mBranchStep; }
  void branchStep(GLfloat v){ mBranchStep = v; }

  GLfloat trunkRadius() const { return mTrunkRadius; }
  void trunkRadius(GLfloat v){ mTrunkRadius = v; }
};

void createLeaves()
{
  points = new osg::Geometry;
  osg::ref_ptr<osg::Vec3Array> vertices  = new osg::Vec3Array();
  points->setVertexArray(vertices);

  for (GLuint i = 0; i < NUM_LEAVES; ++i) {
    osg::Vec4 pos = osg::Vec4(glm::ballRand(200.0f), 1) * osg::Matrix::scale(osg::Vec3(0.65, 0.65, 1));
    pos.z() = std::abs(pos.z()) + 100;
    vertices->push_back(osg::Vec3(pos.x(), pos.y(), pos.z()));
    leaves.emplace_back(vertices->back());
  }

  points->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, vertices->size()));
  osg::StateSet* ss = points->getOrCreateStateSet();
  ss->setAttributeAndModes(new osg::Point(4));
  // weird, if i turn on lighting, points will blink during the growing of tree
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

}

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    static GLfloat lastTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat currentTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    drawTimer += deltaTime;
    GLuint drawBranchCount = tree->numBranches() * drawTimer / ANIM_TIME;
    drawBranchCount = std::min(tree->numBranches(), drawBranchCount);

    tree->removePrimitiveSet(0, 1);
    tree->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, drawBranchCount * NUM_CYLINDER_FACE * 6));
    tree->dirtyDisplayList();
    tree->dirtyBound();

    traverse(node,nv);
  }
};


class InputHandler : public osgGA::GUIEventHandler
{
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    switch (ea.getEventType())
    {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getUnmodifiedKey())
        {
          case osgGA::GUIEventAdapter::KEY_Q:
            drawTimer = 0;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    return false; //return true will stop event
  }
};

int main(int argc, char* argv[])
{
  root = new osg::Group;
  //osg::StateSet* ss = root->getOrCreateStateSet();
  //ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));

  tree = new SpaceColonizationTree();
  tree->minAttract(MIN_ATTRACT);
  tree->maxAttract(MAX_ATTRACT);
  tree->branchStep(BRANCH_STEP);
  tree->setDataVariance(osg::Object::DYNAMIC);

  createLeaves();
  tree->accept(leaves);
  tree->compile();

  osg::ref_ptr<osg::Geode> treeLeaf  = new osg::Geode();
  treeLeaf->addDrawable(tree);
  root->addChild(treeLeaf);
  root->addUpdateCallback(new RootUpdate);

  osg::ref_ptr<osg::Geode> pointsLeaf  = new osg::Geode();
  pointsLeaf->addDrawable(points);
  root->addChild(pointsLeaf);
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  viewer.addEventHandler(new InputHandler);
  {
    osg::Camera* camera = viewer.getCamera();
    camera->setCullingMode(camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);
  }

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);

  // hud
  osg::ref_ptr<osg::Camera> camera = new osg::Camera();
  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setAllowEventFocus(false);  // no event
  camera->setClearMask(GL_DEPTH_BUFFER_BIT);
  camera->setRenderOrder(osg::Camera::POST_RENDER);
  camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 800, 0, 800));
  camera->setViewMatrix(osg::Matrix::identity());
  {
    osg::StateSet* ss = camera->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  }
  
  text = new osgText::Text;
  //text->setDataVariance(osg::Object::DYNAMIC);
  text->setCharacterSize(20);
  text->setAxisAlignment(osgText::TextBase::XY_PLANE);
  text->setPosition(osg::Vec3(20, 700, 0));
  text->setFont(osgText::readFontFile("fonts/arial.ttf"));
  text->setText("q : replay");

  osg::ref_ptr<osg::Geode> textLeaf = new osg::Geode();
  textLeaf->addDrawable(text);
  camera->addChild(textLeaf);
  root->addChild(camera);
  
  return viewer.run();
}
