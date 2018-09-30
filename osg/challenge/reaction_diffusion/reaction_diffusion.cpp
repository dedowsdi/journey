/*
 * TODO fullscreen
 * seed data is wrong in fullscreen, why?
 */
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/Switch>
#include <osgViewer/config/SingleWindow>

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Geometry> quad;
osg::ref_ptr<osg::Texture2D> tex0;
osg::ref_ptr<osg::Texture2D> tex1;
osg::Camera* mainCamera;

GLfloat da = 1;
GLfloat db = 0.5;
GLfloat feed = 0.055;
GLfloat kill = 0.062;
GLfloat delta_time = 1;
GLuint width = 250;
GLuint height = 250;
std::vector<osg::Vec2us> pixels;

void createQuad()
{
  quad = osg::createTexturedQuadGeometry(osg::Vec3(-1,-1,0), osg::Vec3(2, 0, 0), osg::Vec3(0, 2, 0));
}

class SwitchUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    osg::Switch* graph = dynamic_cast<osg::Switch*>(node);
    if(graph)
    {
      graph->setValue(0, !graph->getValue(0));
      graph->setValue(1, !graph->getValue(1));
    }
    traverse(node,nv);
  }
};

osg::Camera* createRttCamera(osg::Texture2D* rttTex, osg::Texture2D* diffuseMap, GLuint width, GLuint height)
{
  // setup rtt camera
  osg::Camera* camera = new osg::Camera();
  camera->setRenderOrder(osg::Camera::PRE_RENDER);
  camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
  camera->setViewport(0, 0, width, height);
  camera->attach(osg::Camera::COLOR_BUFFER, rttTex);
  camera->setViewMatrix(osg::Matrix::identity());
  camera->setProjectionMatrix(osg::Matrix::identity());
  camera->setAllowEventFocus(false);
  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(quad);
  osg::StateSet* ss = leaf->getOrCreateStateSet();
  ss->setTextureAttributeAndModes(0, diffuseMap);

  camera->addChild(leaf);

  return camera;
}

class SubgraphStateSetCallback : public osg::StateSet::Callback
{
  virtual void operator() (osg::StateSet* ss, osg::NodeVisitor* nv)
  {
    //ss->addUniform(new osg::Uniform("diffuse_map", 0));
    ss->getUniform("da")->set(da);
    ss->getUniform("db")->set(db);
    ss->getUniform("feed")->set(feed);
    ss->getUniform("kill")->set(kill);
    ss->getUniform("delta_time")->set(delta_time);
    ss->getUniform("tsize")->set(osg::Vec2(width, height));

    traverse(ss, nv);
  }
};

#define CSOURCE "/home/pntandcnt/journey/osg/challenge"

osg::Node* createSubGraph()
{
  osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
  if(!wsi)
  {
    OSG_FATAL << "failed to get window system interface";
  }

  wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
  //width = osg::Image::computeNearestPowerOfTwo(width);
  //height = osg::Image::computeNearestPowerOfTwo(height);

  osg::Switch* subgraph = new osg::Switch();

  osg::ref_ptr<osg::Program> program = new osg::Program();
  osg::ref_ptr<osg::Shader> shader = osg::Shader::readShaderFile(
      osg::Shader::FRAGMENT, CSOURCE + std::string("/reaction_diffusion/rtt.fs.glsl"));
  program->addShader(shader);

  {
    osg::StateSet* ss = subgraph->getOrCreateStateSet();
    ss->setAttributeAndModes(program);
    ss->addUniform(new osg::Uniform("diffuse_map", 0));
    ss->addUniform(new osg::Uniform("da", da));
    ss->addUniform(new osg::Uniform("db", db));
    ss->addUniform(new osg::Uniform("feed", feed));
    ss->addUniform(new osg::Uniform("kill", kill));
    ss->addUniform(new osg::Uniform("delta_time", delta_time));
    ss->addUniform(new osg::Uniform("tsize", osg::Vec2(width, height)));

    ss->setUpdateCallback(new SubgraphStateSetCallback);
  }

  subgraph->addUpdateCallback(new SwitchUpdate);

  
  pixels.reserve(width*height);
  GLuint hw = width / 2;
  GLuint hh = height / 2;
  for (int i = 0; i < height; ++i) 
  {
    for (int j = 0; j < width; ++j) 
    {
      osg::Vec2us pixel(-1, 0);

      if( i >= hh - 50 && i <= hh + 50 && j >= hw - 50 && j <= hw + 50)
      {
        pixel = osg::Vec2us(-1, -1);
      }
      pixels.push_back(pixel);
      //OSG_NOTICE << pixels.back().x() << ":" << pixels.back().y() << std::endl;
    }
  }

  osg::ref_ptr<osg::Image> img  = new osg::Image();
  img->setImage(width, height, 1, GL_RG16, GL_RG, GL_UNSIGNED_SHORT, 
      reinterpret_cast<unsigned char*>(&pixels.front().x()), osg::Image::NO_DELETE);

  tex0 = new osg::Texture2D;
  tex0->setImage(img);
  tex0->setResizeNonPowerOfTwoHint(false);
  tex0->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
  tex0->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
  tex0->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
  tex0->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
  tex0->setInternalFormat(GL_RG16);
  //tex0->setTextureSize(width, height);

  tex1 = new osg::Texture2D;
  tex1->setImage(img);
  tex1->setResizeNonPowerOfTwoHint(false);
  tex1->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
  tex1->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
  tex1->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
  tex1->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
  tex1->setInternalFormat(GL_RG16);
  //tex1->setTextureSize(width, height);

  subgraph->addChild(createRttCamera(tex1, tex0, width, height), true);
  subgraph->addChild(createRttCamera(tex0, tex1, width, height), false);

  return subgraph;
}


osg::Node* createGraph()
{
  osg::Switch* graph = new osg::Switch();

  osg::ref_ptr<osg::Program> program = new osg::Program();
  osg::ref_ptr<osg::Shader> shader = osg::Shader::readShaderFile(
      osg::Shader::FRAGMENT, CSOURCE + std::string("/reaction_diffusion/draw.fs.glsl"));
  program->addShader(shader);

  {
    osg::StateSet* ss = graph->getOrCreateStateSet();
    ss->setAttributeAndModes(program);
    ss->addUniform(new osg::Uniform("quad_map", 0));
  }

  osg::ref_ptr<osg::Geode> leaf0  = new osg::Geode();
  leaf0->addDrawable(quad);
  osg::StateSet* ss = leaf0->getOrCreateStateSet();
  ss->setTextureAttributeAndModes(0, tex1);

  osg::ref_ptr<osg::Geode> leaf1  = new osg::Geode();
  leaf1->addDrawable(quad);
  ss = leaf1->getOrCreateStateSet();
  ss->setTextureAttributeAndModes(0, tex0);

  graph->addChild(leaf0, true);
  graph->addChild(leaf1, false);

  graph->addUpdateCallback(new SwitchUpdate);

  return graph;

}

int main(int argc, char* argv[])
{
  createQuad();

  root = new osg::Group;
  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  
  root->addChild(createSubGraph());
  root->addChild(createGraph());

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  viewer.realize();

  // seed data is wrong in fullscreen, why?
  //viewer.apply(new osgViewer::SingleWindow(100, 100, width, height));

  osg::Camera* camera = viewer.getCamera();
  camera->setAllowEventFocus(false);
  camera->setViewMatrix(osg::Matrix::identity());
  camera->setProjectionMatrix(osg::Matrix::identity());
  camera->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
  camera->setProjectionResizePolicy(osg::Camera::FIXED);
  mainCamera = camera;

  viewer.setLightingMode(osg::View::NO_LIGHT);
  
  return viewer.run();
}
