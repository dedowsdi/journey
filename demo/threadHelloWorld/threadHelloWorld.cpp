#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osgText/Text>
#include <osgViewer/Viewer>
#include <iostream>

//read user input in another thread in console.
class DataReceiverThread : public OpenThreads::Thread {
protected:
  OpenThreads::Mutex _mutex;
  std::string _content;
  bool _done;
  bool _dirty;

public:
  static DataReceiverThread* instance() {
    static DataReceiverThread s_thread;
    return &s_thread;
  }
  // cancelling process of this thread
  virtual int cancel() {
    _done = true;
    while (isRunning()) YieldCurrentThread();
    return 0;
  }
  // what action happens from beginning to end
  virtual void run() {
    _done = false;
    _dirty = true;
    do {
      // yields the current thread and lets other threads take over the control
      // of the processor
      YieldCurrentThread();
      int ch = 0;
      ch = std::cin.get();
      switch (ch) {
        case 0:
          break;  // We don't want '\0' to be added
        case 9:
          _done = true;
          break;  // ASCII code of Tab = 9
        default:
          addToContent(ch);
          break;
      }
    } while (!_done);
  }
  void addToContent(int ch) {
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    _content += ch;
    _dirty = true;
  }
  bool getContent(std::string& str) {
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    if (_dirty) {
      str += _content;
      _dirty = false;
      return true;
    }
    return false;
  }
};

//change text content
class UpdateTextCallback : public osg::Drawable::UpdateCallback {
public:
  virtual void update(osg::NodeVisitor* nv, osg::Drawable* drawable) {
    osgText::Text* text = static_cast<osgText::Text*>(drawable);
    if (text) {
      std::string str("# ");
      if (DataReceiverThread::instance()->getContent(str)) text->setText(str);
    }
  }
};

int main(int argc, char* argv[]) {

  osg::ref_ptr<osgText::Text> text = new osgText::Text;
  text->setFont("fonts/arial.ttf");
  text->setAxisAlignment(osgText::TextBase::SCREEN);
  text->setDataVariance(osg::Object::DYNAMIC);
  text->setInitialBound(
    osg::BoundingBox(osg::Vec3(), osg::Vec3(400.0f, 20.0f, 20.0f)));
  text->setUpdateCallback(new UpdateTextCallback);

  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable(text.get());
  geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osgViewer::Viewer viewer;
  viewer.setSceneData(geode.get());
  viewer.setUpViewInWindow(50, 50, 640, 480);

  DataReceiverThread::instance()->startThread();
  viewer.run();
  DataReceiverThread::instance()->cancel();
  return 0;
}
