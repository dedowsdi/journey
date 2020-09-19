#ifndef WHACKAMOLE_BASEGAME_H
#define WHACKAMOLE_BASEGAME_H

#include <set>
#include <vector>
#include <string>

#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/ref_ptr>

// add some frequently used head
#include <ToyViewer.h>
#include <ToyMath.h>
#include <osg/io_utils>

#define TOY_STR(x) TOY_STR2(x)
#define TOY_STR2(x) #x
#define TOY_HERE (__FILE__ ":" TOY_STR(__LINE__) ":")

namespace osg
{
class Group;
class Vec2i;
class Texture2D;
class Camera;
class Geometry;
class Program;
class Shader;
}  // namespace osg

namespace osgGA
{
class CameraManipulator;
}

namespace osgViewer
{
class Viewer;
};

namespace toy
{

class ResourceObserver;
class ToyViewer;

class Game
{
public:
    Game();

    virtual ~Game();

    void clear();

    void init(int argc, char* argv[]);

    int run();

    virtual void resize(int width, int height);

    virtual void frame(const osgGA::GUIEventAdapter& ea, const osgGA::GUIActionAdapter& ga);

    // Root children is fixed, don't add or remove anything to it during gaming.
    // If you want to manipulate children of _sceneRoot, make sure it happens in
    // update or event traversal of root.
    osg::Group* getRoot() { return _root; }

    osg::Group* getDebugRoot() { return _debugRoot; }

    osg::Vec2i getWindowSize();

    osg::Vec2 getWindowCenter();

    // Create program and update it if ctime of shader file changed
    osg::Program* createProgram(const std::string& fragFile, int shaderType);

    osg::Program* createProgram(const std::string& vertFile, const std::string& fragFile);

    osg::Program* createProgram(const std::string& vertFile, const std::string& geomFile,
        const std::string& fragFile, int inputType, int outputType, int maxVertices);

    void setUseCursor(bool b);

    void debugDrawLine(const osg::Vec3& from, const osg::Vec3& to,
        const osg::Vec4& fromColor, const osg::Vec4& toColor);

    void debugDrawSphere(const osg::Vec3& pos, float radius, const osg::Vec4& color);

    osg::Camera* getMainCamera() const;

    ToyViewer* getViewer() { return _viewer; }
    const ToyViewer* getViewer() const { return _viewer; }
    void setViewer(ToyViewer* v) { _viewer = v; }

    osg::Group* getSceneRoot() const { return _sceneRoot; }
    void setSceneRoot(osg::Group* v) { _sceneRoot = v; }

    osg::Group* getSceneRoot2() const { return _sceneRoot2; }
    void setSceneRoot2(osg::Group* v) { _sceneRoot2 = v; }

    osg::Camera* getHudCamera() const { return _hudCamera; }
    void setHudCamera(osg::Camera* v) { _hudCamera = v; }

    bool getRequireToyUniforms() const { return _requireToyUniforms; }
    void setRequireToyUniforms(bool v);

    // Hud use window size to create projection, if this is true, projection is changed
    // everytime window resize happens.
    bool getAutoResizeHud() const { return _autoResizeHud; }
    void setAutoResizeHud(bool v) { _autoResizeHud = v; }

protected:
    virtual void createScene(){}

    virtual void preInit(){}

    virtual void postInit(){}

    void createSound(int argc, char* argv[]);

    void createRoots();

    bool _autoResizeHud = false;

    osg::ref_ptr<osg::Group> _root;
    osg::ref_ptr<osg::Group> _sceneRoot;
    osg::ref_ptr<osg::Group> _sceneRoot2;
    osg::ref_ptr<osg::Group> _debugRoot;
    osg::ref_ptr<osg::Camera> _hudCamera;

    osg::ref_ptr<osg::Geometry> _debugLines;

    osg::ref_ptr<ResourceObserver> _observer;

    osg::ref_ptr<ToyViewer> _viewer;

    bool _requireToyUniforms = false;
    osg::Vec2 _mouse;
    osg::ref_ptr<osg::Uniform> _mouseUniform;
    osg::Vec2 _resolution; // resolution of main window
    osg::ref_ptr<osg::Uniform> _resolutionUniform;
};

}  // namespace toy

#endif  // WHACKAMOLE_BASEGAME_H
