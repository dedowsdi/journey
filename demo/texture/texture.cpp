#include <osg/Texture2D>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <common.h>

const std::string helpContent =
  " q to change wrap_s . \n"
  " w to change wrap_t . \n"
  " e to change min_filter . \n"
  " r to change mag_filter . \n";

std::string toString(osg::Texture::WrapMode mode) {
  switch (mode) {
    case osg::Texture::CLAMP:
      return "CLAMP";
    case osg::Texture::CLAMP_TO_EDGE:
      return "CLAMP_TO_EDGE";
    case osg::Texture::CLAMP_TO_BORDER:
      return "CLAMP_TO_BORDER";
    case osg::Texture::REPEAT:
      return "REPEAT";
    case osg::Texture::MIRROR:
      return "MIRROR";
    default:
      return "";
  }
}

std::string toString(osg::Texture::FilterMode mode) {
  switch (mode) {
    case osg::Texture::LINEAR:
      return "LINEAR";
    case osg::Texture::LINEAR_MIPMAP_LINEAR:
      return "LINEAR_MIPMAP_LINEAR";
    case osg::Texture::LINEAR_MIPMAP_NEAREST:
      return "LINEAR_MIPMAP_NEAREST";
    case osg::Texture::NEAREST:
      return "NEAREST";
    case osg::Texture::NEAREST_MIPMAP_LINEAR:
      return "NEAREST_MIPMAP_LINEAR";
    case osg::Texture::NEAREST_MIPMAP_NEAREST:
      return "NEAREST_MIPMAP_NEAREST";
    default:
      return "";
  }
}

void updateTexInfo(osgText::Text* text, osg::Texture* tex) {
  std::string s(helpContent);
  s += "current texture param:  \n";
  s += "    WRAP_S : " + toString(tex->getWrap(osg::Texture::WRAP_S)) + "\n";
  s += "    WRAP_T : " + toString(tex->getWrap(osg::Texture::WRAP_T)) + "\n";
  s += "    MIN_FILTER : " +
       toString(tex->getFilter(osg::Texture::MIN_FILTER)) + "\n";
  s += "    MAG_FILTER : " +
       toString(tex->getFilter(osg::Texture::MAG_FILTER)) + "\n";
  text->setText(s);
}

osg::Texture::WrapMode wrapModes[] = {osg::Texture::CLAMP,
  osg::Texture::CLAMP_TO_EDGE, osg::Texture::CLAMP_TO_BORDER,
  osg::Texture::REPEAT, osg::Texture::MIRROR};

osg::Texture::FilterMode filterModes[] = {osg::Texture::LINEAR,
  osg::Texture::LINEAR_MIPMAP_LINEAR, osg::Texture::LINEAR_MIPMAP_NEAREST,
  osg::Texture::NEAREST, osg::Texture::NEAREST_MIPMAP_LINEAR,
  osg::Texture::NEAREST_MIPMAP_NEAREST};

class MyGuiEventHandler : public osgGA::GUIEventHandler {
private:
  osg::ref_ptr<osg::Texture2D> mTexture;
  osg::ref_ptr<osgText::Text> mText;

public:
  osg::ref_ptr<osg::Texture2D> getTexture() const { return mTexture; }
  void setTexture(osg::ref_ptr<osg::Texture2D> v) { mTexture = v; }

  osg::ref_ptr<osgText::Text> getText() const { return mText; }
  void setText(osg::ref_ptr<osgText::Text> v) { mText = v; }

  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_Q: {
            osg::Texture::WrapMode mode =
              mTexture->getWrap(osg::Texture::WRAP_S);
            auto iter = std::find(wrapModes, wrapModes + 5, mode);
            mTexture->setWrap(osg::Texture::WRAP_S,
              iter == (wrapModes + 4) ? wrapModes[0] : *++iter);
            updateTexInfo(mText, mTexture);
          } break;

          case osgGA::GUIEventAdapter::KEY_W: {
            osg::Texture::WrapMode mode =
              mTexture->getWrap(osg::Texture::WRAP_T);
            auto iter = std::find(wrapModes, wrapModes + 5, mode);
            mTexture->setWrap(osg::Texture::WRAP_T,
              iter == (wrapModes + 4) ? wrapModes[0] : *++iter);
            updateTexInfo(mText, mTexture);
          } break;
          case osgGA::GUIEventAdapter::KEY_E: {
            osg::Texture::FilterMode mode =
              mTexture->getFilter(osg::Texture::MIN_FILTER);
            auto iter = std::find(filterModes, filterModes + 5, mode);
            mTexture->setFilter(osg::Texture::MIN_FILTER,
              iter == (filterModes + 5) ? filterModes[0] : *++iter);
            updateTexInfo(mText, mTexture);
          } break;
          case osgGA::GUIEventAdapter::KEY_R: {
            osg::Texture::FilterMode mode =
              mTexture->getFilter(osg::Texture::MAG_FILTER);
            mTexture->setFilter(osg::Texture::MAG_FILTER,
              mode == osg::Texture::NEAREST ? osg::Texture::LINEAR
                                            : osg::Texture::NEAREST);
            updateTexInfo(mText, mTexture);
          } break;
        }
        break;
      default:
        break;
    }
    return false;  // return true will stop event
  }
};

osg::Geode* createHelp() {
  GLuint sw = 800, sh = 600;
  zxd::getScreenResolution(sw, sh);

  osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/arial.ttf");
  const std::string helpContent =
    " f1 to toggle help \n"
    " 1 to toggle outline. \n"
    " 2 to toggle particle visible \n"
    " 3 to toggle particle frozen \n"
    " 4 to change polygon mode \n"
    " 5 to print \n"
    " ctrl+lmb to select item \n"
    " left/right to change alpha  \n"
    " up/down to change scale \n";

  GLfloat fontSize = 15.0f;  // font height
  GLfloat margin = 10.0f;
  osgText::Text* text = zxd::createText(font,
    osg::Vec3(margin, sh - fontSize - margin, 0.0f), helpContent, fontSize);
  text->setDataVariance(osg::Object::DYNAMIC);
  osg::ref_ptr<osg::Geode> help = new osg::Geode();
  help->addDrawable(text);
  help->getOrCreateStateSet()->setMode(
    GL_LIGHTING, osg::StateAttribute::OFF);

  return help.release();
}

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::Geometry> gm =
    osg::createTexturedQuadGeometry(osg::Vec3(-0.5f, 0, -0.5f),
      osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 1.0f), 5.0f, 5.0f);

  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/lz.rgb");
  texture->setImage(image.get());

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode;
  leaf->addDrawable(gm);
  gm->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());

  osg::Camera* camera = zxd::createHUDCamera();
  //camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  osg::Geode* help = createHelp();
  osgText::Text* text = static_cast<osgText::Text*>(help->getDrawable(0));
  camera->addChild(help);

  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->addChild(leaf);
  root->addChild(camera);

  osg::ref_ptr<MyGuiEventHandler> handler = new MyGuiEventHandler();
  handler->setTexture(texture);
  handler->setText(text);
  updateTexInfo(text, texture);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(handler);
  viewer.setSceneData(root.get());
  return viewer.run();
}
