#include <Game.h>

#include <cassert>

#include <osg/Hint>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

#include <OsgFactory.h>
#include <OsgQuery.h>
#include <Resource.h>
#include <ToyViewer.h>
#include <DebugHandler.h>

namespace toy
{

class GameEventHandler : public osgGA::GUIEventHandler
{
public:
    GameEventHandler(Game* game) : _game(game) {}

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        switch (ea.getEventType())
        {
            case osgGA::GUIEventAdapter::RESIZE:
                _game->resize(ea.getWindowWidth(), ea.getWindowHeight());
                break;

            case osgGA::GUIEventAdapter::FRAME:
                _game->frame(ea, aa);

            default:
                break;
        }
        return false;  // return true will stop event
    }

private:
    Game* _game = 0;
};

void Game::clear() {}

void Game::init(int argc, char* argv[])
{
    _viewer = new ToyViewer;
    _viewer->getCamera()->setName("MainCamera");
    _viewer->addEventHandler(new GameEventHandler(this));

    preInit();

    osg::DisplaySettings::instance()->setMinimumNumStencilBits(1);
    _viewer->realize();

    createRoots();
    _viewer->setSceneData(_root);
    createScene();

    postInit();

    // Add some debug handlers:
    // f1   Stat
    // f2   Print scene
    // f3   Print render stages
    // f4   Save main camera to main.osgt
    // f9   Pause/Continue
    // f10  Step one frame
    // p    capture screen
    // 7    toggle lighting
    // 8    toggle backface culling
    // 9    toggle texture
    // 0    Toggle polygon mode

    _viewer->addEventHandler(new toy::DebugHandler(_viewer->getCamera()));

    auto statsHandler = new osgViewer::StatsHandler;
    statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);

    _viewer->addEventHandler(statsHandler);
    _viewer->addEventHandler(new toy::ViewerDebugHandler(_viewer));

    auto scHander = new osgViewer::ScreenCaptureHandler;
    scHander->setKeyEventTakeScreenShot(osgGA::GUIEventAdapter::KEY_P);
    _viewer->addEventHandler(scHander);

    auto statesetHandler = new osgGA::StateSetManipulator(_root->getOrCreateStateSet());
    statesetHandler->setKeyEventToggleLighting('7');
    statesetHandler->setKeyEventToggleBackfaceCulling('8');
    statesetHandler->setKeyEventToggleTexturing('9');
    statesetHandler->setKeyEventCyclePolygonMode('0');

    _viewer->addEventHandler(statesetHandler);
}

int Game::run()
{
    assert(_viewer);
    return _viewer->run();
}

void Game::resize(int width, int height)
{
    _resolution.x() = width;
    _resolution.y() = height;
    if (_requireToyUniforms)
    {
        _resolutionUniform->set(_resolution);
    }

    if (_autoResizeHud)
    {
        _hudCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, width, 0, height));
    }
    OSG_NOTICE << "Resize to " << width << ":" << height << std::endl;
}

void Game::frame(const osgGA::GUIEventAdapter& ea, const osgGA::GUIActionAdapter& ga)
{
    _mouse = osg::Vec2(ea.getX(), ea.getY());
    if (_requireToyUniforms)
    {
        _mouseUniform->set(_mouse);
    }
}

osg::Vec2i Game::getWindowSize()
{
    auto rect = osgq::getWindowRect(*_viewer);
    return osg::Vec2i(rect.z(), rect.w());
}

osg::Vec2 Game::getWindowCenter()
{
    auto size = getWindowSize();
    return osg::Vec2(size.x() * 0.5f, size.y() * 0.5f);
}

osg::Program* Game::createProgram(const std::string& fragFile, int shaderType)
{
    auto prg = new osg::Program;

    auto fragShader =
        osgDB::readShaderFile(static_cast<osg::Shader::Type>(shaderType), fragFile);
    prg->addShader(fragShader);

#ifdef DEBUG
    _observer->addResource(createShaderResource(fragShader));
#endif /* ifndef DEBUG */

    return prg;
}

osg::Program* Game::createProgram(
    const std::string& vertFile, const std::string& fragFile)
{
    auto prg = new osg::Program;

    auto vertShader = osgDB::readShaderFile(osg::Shader::VERTEX, vertFile);
    prg->addShader(vertShader);

    auto fragShader = osgDB::readShaderFile(osg::Shader::FRAGMENT, fragFile);
    prg->addShader(fragShader);

#ifdef DEBUG
    _observer->addResource(createShaderResource(vertShader));
    _observer->addResource(createShaderResource(fragShader));
#endif /* ifndef DEBUG */

    return prg;
}

osg::Program* Game::createProgram(const std::string& vertFile,
    const std::string& geomFile, const std::string& fragFile, int inputType, int outputType,
    int maxVertices)
{
    auto prg = new osg::Program;
    prg->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, inputType);
    prg->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, outputType);
    prg->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, maxVertices);

    auto vertShader = osgDB::readShaderFile(osg::Shader::VERTEX, vertFile);
    prg->addShader(vertShader);

    auto geomShader = osgDB::readShaderFile(osg::Shader::GEOMETRY, geomFile);
    prg->addShader(geomShader);

    auto fragShader = osgDB::readShaderFile(osg::Shader::FRAGMENT, fragFile);
    prg->addShader(fragShader);

#ifdef DEBUG
    _observer->addResource(createShaderResource(vertShader));
    _observer->addResource(createShaderResource(fragShader));
    _observer->addResource(createShaderResource(geomShader));
#endif /* ifndef DEBUG */

    return prg;
}

void Game::setUseCursor(bool b)
{
    static_cast<osgViewer::GraphicsWindow*>(osgq::getGraphicsWindow(*_viewer))
        ->useCursor(false);
}

void Game::debugDrawLine(const osg::Vec3& from, const osg::Vec3& to,
    const osg::Vec4& fromColor, const osg::Vec4& toColor)
{
    if (!_debugLines)
    {
        _debugLines = new osg::Geometry;
        auto vertices = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
        auto colors = new osg::Vec4Array(osg::Array::BIND_PER_VERTEX);
        _debugLines->setVertexArray(vertices);
        _debugLines->setColorArray(colors);
        _debugLines->setDataVariance(osg::Object::DYNAMIC);

        _debugLines->setName("DebugLines");
        _debugLines->setUseDisplayList(false);
        _debugLines->setUseVertexBufferObjects(true);
        _debugLines->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 0));

        auto ss = _debugLines->getOrCreateStateSet();
        ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
        ss->setMode(
            GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
        ss->setRenderBinDetails(999, "RenderBin");

        _debugRoot->addChild(_debugLines);
    }

    auto vertices = static_cast<osg::Vec3Array*>(_debugLines->getVertexArray());
    auto colors = static_cast<osg::Vec4Array*>(_debugLines->getColorArray());
    vertices->push_back(from);
    vertices->push_back(to);
    colors->push_back(fromColor);
    colors->push_back(toColor);

    vertices->dirty();
    colors->dirty();

    static_cast<osg::DrawArrays*>(_debugLines->getPrimitiveSet(0))
        ->setCount(vertices->size());
    _debugLines->dirtyBound();
}

void Game::debugDrawSphere(const osg::Vec3& pos, float radius, const osg::Vec4& color)
{
    _debugRoot->addChild(osgf::createSphereAt(pos, radius, color));
}

osg::Camera* Game::getMainCamera() const
{
    return _viewer->getCamera();
}

void Game::setRequireToyUniforms(bool v)
{
    _requireToyUniforms = v;
    if (_requireToyUniforms)
    {
        if (!_resolutionUniform)
        {
            auto ss = _root->getOrCreateStateSet();

            _resolutionUniform = new osg::Uniform("resolution", _resolution);
            _mouseUniform = new osg::Uniform("mouse", _mouse);

            ss->addUniform(_resolutionUniform);
            ss->addUniform(_mouseUniform);
        }
    }
}

Game::Game() {}

Game::~Game() {}

void Game::createRoots()
{
    _root = new osg::Group();
    _root->setName("Root");

#ifdef DEBUG
    _observer = new ResourceObserver;
    _root->addUpdateCallback(_observer);
#endif /* ifndef DEBUG */

    _debugRoot = new osg::Group();
    _debugRoot->setName("DebugRoot");

    _root->addChild(_debugRoot);

    _sceneRoot = new osg::Group();
    _sceneRoot->setName("SceneRoot");

    _root->addChild(_sceneRoot);

    _sceneRoot2 = new osg::Group();
    _sceneRoot2->setName("SceneRoot2");

    _root->addChild(_sceneRoot2);

    auto rect = osgq::getWindowRect(*_viewer);
    _resolution.x() = rect.z();
    _resolution.y() = rect.w();

    _hudCamera = osgf::createOrthoCamera(0, rect.z(), 0, rect.w());
    _hudCamera->setName("Hud");
    auto ss = _hudCamera->getOrCreateStateSet();
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    _root->addChild(_hudCamera);
}

}  // namespace toy
