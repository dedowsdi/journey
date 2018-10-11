#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/Material>
#include <osg/LightModel>
#include "glm.h"

#define RADIUS 50.0
#define EXTENT_SIZE 100.0

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Group> spheres;
osg::ref_ptr<osg::ShapeDrawable> geometry;

std::ostream& operator << (std::ostream& os, const osg::Vec3& v)
{
  os << v.x() << " " << v.y() << " " << v.z();
  return os;
}

void createSphere()
{
  osg::ref_ptr<osg::Sphere> sphere  = new osg::Sphere();
  osg::ref_ptr<osg::TessellationHints> hint = new osg::TessellationHints();
  //hint->setDetailRatio(4);

  geometry = new osg::ShapeDrawable;
  geometry->setShape(sphere);
  geometry->setTessellationHints(hint);
}

// generate uniformly distributed sample in the input extent, no samples exists
// in radius r of any sample.
std::vector<osg::Vec3> poisson_disk_sample3(const osg::Vec3& min_ext, const osg::Vec3& max_ext, GLfloat radius, GLubyte k = 30)
{
  typedef std::vector<int> int_grid;
  std::vector<osg::Vec3> samples;
  std::vector<int> actives;
  int_grid grid;

  // step0 : Initialize an n-dimensional background grid for storing samples and
  // accelerating spatial searches. We pick the cell size to  be bounded by
  // r/sqrt(N), so that each grid cell will contain at most one sample
  osg::Vec3 ext = max_ext - min_ext;
  GLfloat cell_size = radius / std::sqrt(3.0);
  GLuint lays = std::ceil(ext.z() / cell_size);
  GLuint rows = std::ceil(ext.y() / cell_size);
  GLuint cols = std::ceil(ext.x() / cell_size);
  GLuint laySize = rows*cols;
  grid.assign(lays*rows*cols, -1);
  actives.reserve(grid.size());
  samples.reserve(grid.size());
  OSG_NOTICE << "init cell size as " << cell_size << ", init " << grid.size() << " grid cells" << std::endl;

  // step1 : Select the initial sample, s0 , randomly chosen uniformly from the
  // domain. Insert it into the background grid, and initialize the “active
  // list”
  osg::Vec3 s0 = glm::linearRand(min_ext, max_ext);
  samples.push_back(s0);
  osg::Vec3i crl0 = osg::Vec3i((s0.x() - min_ext.x()) / cell_size, (s0.y() - min_ext.y())/cell_size, (s0.z() - min_ext.z()) / cell_size);
  grid[laySize * crl0.z() + crl0.y()*cols + crl0.x()] = 0;
  actives.push_back(0);
  OSG_NOTICE << "create first sample " << s0 << std::endl;


  // step 2 : While the active list is not empty, choose a random active_index from it
  // (say i). Generate up to k points chosen uniformly from the spherical
  // annulus between radius r and 2r around x i . For each point in turn, check
  // if it is within distance r of existing samples (using the background grid
  // to only test nearby samples). If a point is adequately far from existing
  // samples, emit it as the next sample and add it to the active list. If after
  // k attempts no such point is found, instead remove i from the active list.
  GLint neighbour_radius = std::ceil(std::sqrt(3.0));
  GLfloat two_radius = radius * 2;
  GLfloat radius2 = radius * radius;
  while(!actives.empty())
  {
    GLuint active_index = glm::linearRand(0.0, 1.0) * actives.size();
    const osg::Vec3& sample = samples[actives[active_index]];
    bool new_sample_emitted = false;
    for (GLubyte kill = k; kill > 0; --kill) {
      osg::Vec3 new_sample = sample + glm::sphericalRand(1.0f) * glm::linearRand(radius, two_radius);
      while(new_sample.x() < min_ext.x() || new_sample.y() < min_ext.y() || new_sample.z() < min_ext.z() 
          || new_sample.x() > max_ext.x() || new_sample.y() > max_ext.y() || new_sample.z() > max_ext.z())
        new_sample = sample + glm::sphericalRand(1.0f) * glm::linearRand(radius, two_radius);

      GLint lay = (new_sample.z() - min_ext.z()) / cell_size;
      GLint row = (new_sample.y() - min_ext.y()) / cell_size;
      GLint col = (new_sample.x() - min_ext.x()) / cell_size;

      // check neighbour of new sample to make sure there has no sample in r
      // radius
      bool valid_new_sample = true;
      for (int l = -neighbour_radius; l < neighbour_radius && valid_new_sample; ++l) {
        GLint nb_lay = lay + l;
        if(nb_lay < 0 || nb_lay >= lays)
          continue;

        for (int i = -neighbour_radius; i <= neighbour_radius && valid_new_sample; ++i) {

          GLint nb_row = row + i;
          if(nb_row < 0 || nb_row >= rows)
            continue;

          for (int j = -neighbour_radius; j <= neighbour_radius; ++j) {
            GLint nb_col = col + j;
            if(nb_col < 0 || nb_col >= cols)
              continue;

            GLuint neighbour_sample_index = grid[nb_lay * laySize + nb_row * cols + nb_col];
            if(neighbour_sample_index != -1 && (samples[neighbour_sample_index] - new_sample).length2() <= radius2)
            {
              valid_new_sample = false;
              break;
            }
          }
        }

      }

      if(!valid_new_sample)
        continue;

      // debug check
      //for (int i = 0; i < samples.size(); ++i) {
      //if(glm::length2(new_sample - samples[i]) <= radius2)
      //{
      //GLint c0 = (new_sample.x - min_ext.x) / cell_size;
      //GLint r0 = (new_sample.y - min_ext.y) / cell_size;
      //GLint c1 = (samples[i].x - min_ext.x) / cell_size;
      //GLint r1 = (samples[i].y - min_ext.y) / cell_size;
      //OSG_NOTICE << "found illegal new_sample " << new_sample  << " " << r0 << ":" << c0 
      //<< ", it's too close to " 
      //<< samples[i]  << " " << r1 << ":" << c1 
      //<< ", it's grid value is " << grid[r1 * cols + c1]
      //<< std::endl;
      //}
      //}

      GLuint new_sample_index = lay * laySize +  row * cols + col;
      grid[new_sample_index] = samples.size();
      actives.push_back(samples.size());
      samples.push_back(new_sample);
      new_sample_emitted = true;
      if(samples.size() > grid.size())
        throw std::runtime_error("something wrong happened, sample size should not be bigger than gird size");
      break;
    }

    if(!new_sample_emitted)
      actives.erase(actives.begin() + active_index);
  }

  OSG_NOTICE << "generate " << samples.size() << " samples" << std::endl;
  return samples;
};

osg::MatrixTransform* createSphereNode(const osg::Vec3& pos)
{
  osg::MatrixTransform* mt = new osg::MatrixTransform;
  mt = new osg::MatrixTransform;
  mt->setMatrix(osg::Matrix::scale(osg::Vec3(RADIUS/2, RADIUS/2,  RADIUS/2)) * osg::Matrix::translate(pos));

  osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();

  mt->addChild(leaf);
  leaf->addDrawable(geometry);

  return mt;
}

void resample()
{
  spheres->removeChild(0, spheres->getNumChildren());
  std::vector<osg::Vec3> translations = poisson_disk_sample3(osg::Vec3(0,0,0), osg::Vec3(EXTENT_SIZE, EXTENT_SIZE, EXTENT_SIZE), RADIUS);
  for (int i = 0; i < translations.size(); ++i) {
    spheres->addChild(createSphereNode(translations[i]));
  }
}

class SampleGuiHandler : public osgGA::GUIEventHandler
{
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    switch (ea.getEventType())
    {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getUnmodifiedKey())
        {
          case osgGA::GUIEventAdapter::KEY_Q:
            resample();
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
  spheres = new osg::Group;
  root->addChild(spheres);
  //osg::StateSet* ss = root->getOrCreateStateSet();
  //ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  //ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  {
    osg::StateSet* ss = spheres->getOrCreateStateSet();
    ss->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Material> mtl  = new osg::Material();
    mtl->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.5, 0.5, 0.5, 1));
    mtl->setShininess(osg::Material::FRONT_AND_BACK, 50);
    ss->setAttribute(mtl);

    osg::ref_ptr<osg::LightModel> lm  = new osg::LightModel();
    lm->setLocalViewer(true);
    ss->setAttribute(lm);
    
  }

  createSphere();
  //root->addChild(createSphereNode(osg::Vec3(0,0,0)));
  resample();


  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  viewer.addEventHandler(new SampleGuiHandler());

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);

  // text
  GLuint width, height;
  osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
  wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
  
  osg::ref_ptr<osg::Camera> camera  = new osg::Camera();
  root->addChild(camera);
  camera->setClearMask(0);
  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setAllowEventFocus(false);
  camera->setViewMatrix(osg::Matrix::identity());
  camera->setViewMatrix(osg::Matrix::ortho2D(0, width, 0, height));
  camera->setRenderOrder(osg::Camera::POST_RENDER);
  {
    osg::StateSet* ss = camera->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  }

  osg::ref_ptr<osgText::Text> text = new osgText::Text();
  text->setFont(osgText::readFontFile("fonts/arial.ttf"));
  text->setCharacterSize(30);
  text->setText("q : resample");
  text->setPosition(osg::Vec3(20, height - 30, 0));

  osg::ref_ptr<osg::Geode> textLeaf = new osg::Geode();
  camera->addChild(textLeaf);
  textLeaf->addDrawable(text);

  return viewer.run();
}
