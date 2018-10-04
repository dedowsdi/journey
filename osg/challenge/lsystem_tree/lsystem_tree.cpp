#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <sstream>
#include "glm.h"
#include "matstack.h"

class LSystemTree;
osg::ref_ptr<osg::Group> root;
osg::ref_ptr<LSystemTree> tree;
osg::ref_ptr<osgText::Text> text;
GLint iterationCount = 1;
GLfloat angle = 25.0f;

struct lsystem
{
  typedef std::map<char, std::string> rule;
  //std::string axiom;
  
  rule rules;

  std::string generate(const std::string& axiom, GLuint count)
  {
    if(count == 0)
      return axiom;

    std::string s;
    for(char c : axiom)
    {
      auto iter = rules.find(c);
      if(iter == rules.end())
        s.push_back(c);
      else
        s += iter->second;
    }

    return generate(s, --count);
  }
};

class LSystemTree : public osg::Geometry
{
protected:
  GLfloat mAngle;
  GLfloat mFLen;

public:
  void compile(const std::string& text)
  {
    osg::Vec2Array& vertices = *new osg::Vec2Array();
    setVertexArray(&vertices);

    vertices.reserve(std::count(text.begin(), text.end(), 'F') * 2);
    GLfloat len = mFLen;

    zxd::MatStack m;

    for(char c : text)
    {
      switch(c)
      {
        // F->FF+[+F-F-F]-[-F+F+F]
        case '[':
          m.push();
          break;

        case ']':
          m.pop();
          break;

        case '+':
          m.rotate(-mAngle, osg::Vec3(0, 0, 1));
          break;

        case '-':
          m.rotate(+mAngle, osg::Vec3(0, 0, 1));
          break;

        case 'F':
          {
            osg::Vec4 v0 = osg::Vec4(0,0,0,1) * m.top();
            osg::Vec4 v1 = osg::Vec4(0,len,0,1) * m.top();

            vertices.push_back(osg::Vec2(v0.x(), v0.y()));
            vertices.push_back(osg::Vec2(v1.x(), v1.y()));
            m.translate(osg::Vec3(0, len, 0));
            //OSG_NOTICE << v0.x() << ":" << v0.y() << std::endl;
          }
          break;

        default:
          OSG_NOTICE << "unknow lsystem character " << c << std::endl;
      }
    }

    if(m.size() != 1)
      OSG_NOTICE << "missing ]" << std::endl;

    osg::ref_ptr<osg::Vec4Array> colors  = new osg::Vec4Array(osg::Array::BIND_OVERALL);
    colors->push_back(osg::Vec4(1,1,1,1));
    setColorArray(colors);

    removePrimitiveSet(0, getNumPrimitiveSets());
    addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, vertices.size()));

    dirtyBound();
    dirtyDisplayList();

  }

  GLfloat angle() const { return mAngle; }
  void angle(GLfloat v){ mAngle = v; }

  GLfloat flen() const { return mFLen; }
  void flen(GLfloat v){ mFLen = v; }

};

void updateTree()
{
  lsystem ls;
  ls.rules['F'] = "FF+[+F-F-F]-[-F+F+F]";
  std::string text = ls.generate("F", iterationCount);

  //std::cout << text << std::endl;
  tree->flen(200.0f / std::pow(2, iterationCount));
  tree->angle(osg::DegreesToRadians(angle));
  tree->compile(text);
}

void updateText()
{
  std::stringstream ss;
  ss << "qQ : iteration count " << iterationCount << std::endl;
  ss << "wW : angle " << angle << std::endl;
  text->setText(ss.str());
}
class TreeGuiEventHandler : public osgGA::GUIEventHandler
{
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    switch (ea.getEventType())
    {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getUnmodifiedKey())
        {
          case osgGA::GUIEventAdapter::KEY_Q:
            iterationCount += ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT ? -1 : 1;
            iterationCount = std::max(1, iterationCount);
            updateText();
            updateTree();
            break;

          case osgGA::GUIEventAdapter::KEY_W:
            angle += ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT ? -1 : 1;
            updateText();
            updateTree();
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
  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.addEventHandler(new TreeGuiEventHandler);
  viewer.setSceneData(root);
  viewer.realize();

  osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
  if(!wsi)
    OSG_FATAL << "fialed to get window system interface" << std::endl;;
  GLuint width, height;
  wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
  
  // tip
  text = new osgText::Text;
  text->setDataVariance(osg::Object::DYNAMIC);
  text->setCharacterSize(20);
  text->setAxisAlignment(osgText::TextBase::XY_PLANE);
  text->setPosition(osg::Vec3(20, height - 30, 0));
  text->setFont(osgText::readFontFile("fonts/arial.ttf"));

  // setup scene
  tree = new LSystemTree;
  updateTree();
  updateText();

  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
  root->addChild(mt);
  mt->setMatrix(osg::Matrix::translate(width * 0.5, 50, 0));

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  mt->addChild(leaf);
  leaf->addDrawable(tree);

  osg::Camera* camera = viewer.getCamera();
  camera->setAllowEventFocus(false);
  camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
  camera->setViewMatrix(osg::Matrix::identity());
  camera->setProjectionMatrixAsOrtho2D(0, width, 0, height);

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);
  root->addChild(text);
  
  return viewer.run();
}
