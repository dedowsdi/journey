#include <list>
#include <memory>

#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <Config.h>
#include <Game.h>
#include <OsgFactory.h>

int main(int argc, char* argv[])
{
    toy::Game game;
    game.init(argc, argv);
    game.setRequireToyUniforms(true);

    sgc.load("water.config");
    auto cols = sgc.getInt("cols");
    auto rows = cols;
    auto numBalls = sgc.getInt("numBalls");
    OSG_NOTICE << "Create " << numBalls << " balls "<< std::endl;

    auto geometry = new osg::Geometry;
    geometry->setName("Water");

    auto vertices = new osg::Vec3Array();
    vertices->reserve(cols * rows);
    geometry->setVertexArray(vertices);

    osg::Vec3 corner = osg::Vec3(-100, -100, 0);
    auto step = 200.0f / (cols - 1);
    for (auto i = 0; i < cols; ++i)
    {
        auto x = corner.x() + i * step;
        for (auto j = 0; j < rows; ++j)
        {
            auto y = corner.y() + j * step;
            vertices->push_back(osg::Vec3(x, y, 0));
        }
    }
    OSG_NOTICE << vertices->back() << std::endl;
    osgf::addGridElements<osg::DrawElementsUInt>(*geometry, rows - 1, cols - 1, 0, true);

    auto ss = geometry->getOrCreateStateSet();
    ss->setAttributeAndModes(game.createProgram("shader/water.vert", "shader/water.frag"));
    ss->setDefine("NUM_BALLS", numBalls);

    auto hudCamera = game.getHudCamera();
    hudCamera->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
    hudCamera->setProjectionMatrix(osg::Matrix::ortho(-100, 100, -100, 100, -100, 100));
    game.setAutoResizeHud(false);
    hudCamera->addChild(geometry);

    // game.getSceneRoot()->addChild(geometry);

    return game.run();
}
