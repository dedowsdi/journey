#include <list>
#include <memory>

#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <Config.h>
#include <Game.h>
#include <OsgFactory.h>

// update force uniforms every n frames
class DistortionEventHandler : public osgGA::GUIEventHandler
{
public:
    DistortionEventHandler(toy::Game* game, osg::Uniform* forceUniform)
        : _game(game), _forcesUniform(forceUniform)
    {
        _numMousePoints = sgc.getInt("numMousePoints");
    }

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        switch (ea.getEventType())
        {
            case osgGA::GUIEventAdapter::FRAME:
            {
                if (_game->getViewer()->getPause())
                {
                    return false;
                }

                auto pos = osg::Vec2(ea.getX(), ea.getY());

                // add new force, remove the oldest one
                auto size = _game->getWindowSize();
                auto minExtent = std::min(size.x(), size.y());

                pos /= minExtent;

                if (_points.empty())
                {
                    _points.resize(_numMousePoints, pos);
                }

                auto newForce = pos - osg::Vec2(_points.back().x(), _points.back().y());
                _points.push_back(pos);
                _points.pop_front();

                // update force uniforms
                auto iter = _points.begin();
                for (auto i = 0; i < _numMousePoints; ++i)
                {
                    _forcesUniform->setElement(i, *iter++);
                }
            }
            break;
            default:
                break;
        }
        return false;  // return true will stop event
    }

private:
    int _numMousePoints = 1;
    osg::Uniform* _forcesUniform = 0;
    toy::Game* _game = 0;
    std::list<osg::Vec2> _points;
};

int main(int argc, char* argv[])
{
    toy::Game game;
    game.init(argc, argv);
    game.setRequireToyUniforms(true);
    game.getMainCamera()->setClearMask(0);

    sgc.load("distortion.config");
    auto numMousePoints = sgc.getInt("numMousePoints");
    OSG_NOTICE << "Number of mouse points : " << numMousePoints << std::endl;

    // draw full screen ndc quad with custom program
    auto quad = osgf::getNdcQuad();
    auto ss = quad->getOrCreateStateSet();

    auto tex = new osg::Texture2D(osgDB::readImageFile("texture/rayman.jpg"));
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

    ss->setTextureAttributeAndModes(0, tex);
    ss->setAttributeAndModes(
        game.createProgram("shader/distortion.vert", "shader/distortion.frag"));
    ss->setDefine("NUM_MOUSE_POINTS", std::to_string(numMousePoints));

    auto forcesUniform =
        new osg::Uniform(osg::Uniform::FLOAT_VEC2, "mouse_points", numMousePoints);
    ss->addUniform(forcesUniform);

    quad->addEventCallback(new DistortionEventHandler(&game, forcesUniform));

    auto hudCamera = game.getHudCamera();
    hudCamera->setProjectionMatrix(osg::Matrix::ortho2D(-1, 1, -1, 1));
    game.setAutoResizeHud(false);
    hudCamera->addChild(quad);
    hudCamera->setClearMask(0);

    return game.run();
}
