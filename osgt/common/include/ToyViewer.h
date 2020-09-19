#ifndef WHACKAMOLE_TOYVIEWER_H
#define WHACKAMOLE_TOYVIEWER_H

#include <osgViewer/Viewer>

namespace toy
{

class ToyViewer : public osgViewer::Viewer
{
public:
    int run() override;

    bool getPause() const { return _pause; }
    void setPause(bool v) { _pause = v; }

    int getDebugSteps() const { return _debugSteps; }
    void setDebugSteps(int v) { _debugSteps = v; }

private:
    int _debugSteps = 0;
    bool _pause = false;
};

class ViewerDebugHandler : public osgGA::GUIEventHandler
{
public:
    ViewerDebugHandler(ToyViewer* viewer) : _viewer(viewer) {}

protected:
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

    ToyViewer* _viewer;
};

}  // namespace toy

#endif  // WHACKAMOLE_TOYVIEWER_H
