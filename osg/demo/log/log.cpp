#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

class LogFileHandler : public osg::NotifyHandler {
public:
  LogFileHandler(const std::string& file) : _log(file) {}
  virtual void notify(osg::NotifySeverity severity, const char* msg) {
    _log << msg;
  }

protected:
  std::ofstream _log;
};

int main(int argc, char* argv[]) {
  osg::ArgumentParser arguments(&argc, argv);

  // osg notify message to console by default. This can be changed by
  // setNotifyHandler
  osg::setNotifyLevel(osg::INFO);
  osg::setNotifyHandler(new LogFileHandler("osg.txt"));

  auto root = osgDB::readNodeFiles(arguments);
  if (!root) {
    OSG_FATAL << arguments.getApplicationName() << " : No data loaded"
              << std::endl;
    return -1;
  }

  osgViewer::Viewer viewer;
  viewer.setSceneData(root);

  return viewer.run();
}
