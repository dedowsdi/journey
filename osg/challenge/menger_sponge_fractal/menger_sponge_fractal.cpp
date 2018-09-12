#include <osg/MatrixTransform>
#include <osg/Light>
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osg/LightModel>
#include <osg/Point>

#include <osgViewer/Viewer>

#include <osgUtil/SmoothingVisitor>
#include <osgUtil/PrintVisitor>

#define SPONGE_SIZE 100.0f

// use origin as center
class cell;
typedef std::vector<osg::ref_ptr<cell>> cell_vector;
cell_vector sponge;
osg::ref_ptr<osg::Geometry> cube;
osg::ref_ptr<osg::Group> root;

class cell : public osg::Group
{
public:
  GLfloat m_size;
  osg::Vec3 m_position;
public:
  cell(const osg::Vec3& position, GLfloat size):
    m_size(size),
    m_position(position)
  {
    osg::ref_ptr<osg::MatrixTransform> mt  = new osg::MatrixTransform();
    mt->setMatrix(osg::Matrix::scale(m_size, m_size, m_size) * osg::Matrix::translate(m_position));
    osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();

    leaf->addDrawable(cube);
    mt->addChild(leaf);
    this->addChild(mt);
  }

  // subdivide all cell, 
  cell_vector subdivide()
  {
    cell_vector cv;

    // subdivide each cell face into 9 cells, exclude cell in face center and cube
    // center. there should be 27 - 6 - 1 = 20 cells
    GLfloat size = m_size/3;
    for (int j = -1; j < 2; ++j) {
      for (int k = -1; k < 2; ++k) {
        for (int l = -1; l < 2; ++l) {
          if(j*k || j*l || k*l)
          {
            cv.push_back(osg::ref_ptr<cell>(new cell(m_position + osg::Vec3(j*size, k*size, l*size), size)));
          }
        }
      }
    }


    return cv;
  }

};

osg::ref_ptr<osg::Geometry> createCube(const osg::Vec3& halfSize) {
  osg::ref_ptr<osg::Geometry> cube = new osg::Geometry();

  osg::ref_ptr<osg::Vec3Array> va = new osg::Vec3Array();

  osg::Vec3 x = osg::X_AXIS * halfSize.x();
  osg::Vec3 y = osg::Y_AXIS * halfSize.y();
  osg::Vec3 z = osg::Z_AXIS * halfSize.z();

  // front
  va->push_back(-y); // center
  va->push_back(x);  // right
  va->push_back(z);  // up
  // back
  va->push_back(y);
  va->push_back(-x);
  va->push_back(z);
  // right
  va->push_back(x);
  va->push_back(y);
  va->push_back(z);
  // left
  va->push_back(-x);
  va->push_back(-y);
  va->push_back(z);
  // top
  va->push_back(z);
  va->push_back(x);
  va->push_back(y);
  // bottom
  va->push_back(-z);
  va->push_back(-x);
  va->push_back(y);

  osg::ref_ptr<osg::Vec3Array> positions = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();
  normals->setBinding(osg::Array::BIND_PER_VERTEX);

  for (GLuint i = 0; i < va->size();) {
    const osg::Vec3& center = va->at(i++);
    const osg::Vec3& right = va->at(i++);
    const osg::Vec3& up = va->at(i++);

    // create 2 triangles in counter clock wise order
    positions->push_back(center - right + up);
    positions->push_back(center - right - up);
    positions->push_back(center + right - up);

    positions->push_back(center - right + up);
    positions->push_back(center + right - up);
    positions->push_back(center + right + up);

    osg::Vec3 n = center / center.length();
    for (int i = 0; i < 6; ++i) {
      normals->push_back(n);
    }
  }

  cube->setVertexArray(positions);
  cube->setNormalArray(normals);
  cube->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 36));

  // generate smooth normal
  osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();
  leaf->addDrawable(cube);
  osg::ref_ptr<osgUtil::SmoothingVisitor> sv = new osgUtil::SmoothingVisitor();
  //cube->accept(*sv);
  //leaf->accept(*sv);

  //osg::Vec3Array* na = static_cast<osg::Vec3Array*>(cube->getNormalArray());
  //for (size_t i = 0; i < na->size(); ++i) {
    //osg::Vec3& v = na->at(i);
    //std::cout << v.x() << " " << v.y() << " " << v.z() << std::endl;
  //}


  //cube->setUseDisplayList(false);
  return cube;
}

//rotate light
class LightNodeCallback : public osg::NodeCallback {
public:
  LightNodeCallback() {}

  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    osg::MatrixTransform* mt = node->asTransform()->asMatrixTransform();
    mt->setMatrix(osg::Matrix::rotate(
      osg::PI * nv->getFrameStamp()->getReferenceTime(), osg::Z_AXIS));
  }
};

class SpongeGuiHandler : public osgGA::GUIEventHandler {
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_Q:
            {
              cell_vector copy(sponge);
              sponge.clear();
              sponge.reserve(copy.size() * 20);

              for (size_t i = 0; i < copy.size(); ++i) {
                cell_vector cells = copy[i]->subdivide();
                sponge.insert(sponge.end(), cells.begin(), cells.end());
                root->removeChild(copy[i]);
              }

              for (size_t i = 0; i < sponge.size(); ++i) {
                root->addChild(sponge[i]);
              }
            }
            break;

          case osgGA::GUIEventAdapter::KEY_W:
            {
              osg::ref_ptr<osgUtil::PrintVisitor> pv  = new osgUtil::PrintVisitor(std::cout);
              root->accept(*pv);
              root->getParent(0)->accept(*pv);

              pv->setTraversalMode(osg::NodeVisitor::TRAVERSE_PARENTS);
              root->accept(*pv);
            }
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

int main(int argc, char *argv[])
{
  cube = createCube(osg::Vec3(0.5, 0.5, 0.5));

  root = new osg::Group();

  osg::ref_ptr<osg::Light> light = new osg::Light();
  light->setDiffuse(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
  light->setSpecular(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
  light->setPosition(osg::Vec4(SPONGE_SIZE * 1.5, 0.0f, 0.0f, 1.0f));
  //light->setPosition(osg::Vec4(0, 0.0f, 0.0f, 1.0f));

  osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
  lightSource->setLight(light);

  osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
  transform->addChild(lightSource);

  transform->setUpdateCallback(new LightNodeCallback());

  // light mark
  osg::ref_ptr<osg::Geometry> lightMark  = new osg::Geometry();
  osg::ref_ptr<osg::Vec4Array> vertices = new osg::Vec4Array();
  vertices->push_back(light->getPosition());
  lightMark->setVertexArray(vertices);
  lightMark->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));
  {
    osg::StateSet* ss = lightMark->getOrCreateStateSet();
    osg::ref_ptr<osg::Point> p = new osg::Point(5);
    ss->setAttributeAndModes(p);

    osg::ref_ptr<osg::LightModel> lm  = new osg::LightModel();
    lm->setLocalViewer(true);
    ss->setAttributeAndModes(lm);

  }

  osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();
  leaf->addDrawable(lightMark);
  transform->addChild(leaf);

  sponge.push_back(osg::ref_ptr<cell>(new cell(osg::Vec3(0,0,0), SPONGE_SIZE)));
  root->addChild(sponge.back());
  root->addChild(transform);
  {
    osg::StateSet* ss = root->getOrCreateStateSet();
    ss->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
    ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
    ss->setAttributeAndModes(light);
  }

  osgViewer::Viewer viewer;
  viewer.setSceneData(root);
  viewer.addEventHandler(new SpongeGuiHandler());
  viewer.realize();
  viewer.getCamera()->setCullingMode( viewer.getCamera()->getCullingMode() & ~osg::CullStack::SMALL_FEATURE_CULLING);

  return viewer.run();
}
