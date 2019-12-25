#ifndef BULLETCLIP_BULLETCLIP_H
#define BULLETCLIP_BULLETCLIP_H

#include <memory>
#include <map>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Matrix>
#include <osgGA/GUIEventHandler>
#include <osg/Material>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#define sgbc BulletClip::instance()
#define sgbw BulletClip::instance().getWorld()

class DebugDrawer;

class BulletClip : public osgGA::GUIEventHandler
{
public:
    BulletClip( const BulletClip& ) = delete;
    BulletClip& operator=( const BulletClip& ) = delete;

    ~BulletClip();

    virtual bool handle(
        const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

    std::shared_ptr<btBoxShape> createBoxShape( const osg::Vec3& halfExtent );

    void shootBox(const osg::Vec3& start, const osg::Vec3& dir);

    static BulletClip& instance();

    btDynamicsWorld& getWorld() const { return *_world.get(); }

    btCollisionConfiguration& getConfig() const { return *_config.get(); }

    btDispatcher& getDispatcher() const { return *_dispatcher.get(); }

    btBroadphaseInterface& getBroadPhase() const { return *_broadPhase.get(); }

    btConstraintSolver& getSolver() const { return *_solver.get(); }

    osg::Group* getDebugRoot() const { return _debugRoot; }
    void setDebugRoot(osg::Group* v);

private:
    BulletClip();

    void preTickCallback(btDynamicsWorld* world, btScalar timeStep);
    void postTickCallback(btDynamicsWorld* world, btScalar timeStep);

    bool _debugging = false;

    int _keyEventToggleDebugging = 'd';
    int _keyEventToggleContactNormal = 'n' ;
    int _keyEventToggleContactLifetime = 'l';
    int _keyEventShotBoxInitalSpeedPlus = '+';
    int _keyEventShotBoxInitalSpeedMinus = '-';
    GLfloat _shootBoxInitialSpeed = 256.0f;

    osg::Group* _debugRoot;
    osg::ref_ptr<osg::Group> _shootBoxRoot;
    osg::ref_ptr<DebugDrawer> _debugDrawer;
    osg::ref_ptr<osg::Geometry> _shootBoxGeom;
    osg::ref_ptr<osg::Material> _shootBoxMaterial;

    std::unique_ptr<btDynamicsWorld> _world;
    std::unique_ptr<btCollisionConfiguration> _config;
    std::unique_ptr<btDispatcher> _dispatcher;
    std::unique_ptr<btBroadphaseInterface> _broadPhase;
    std::unique_ptr<btConstraintSolver> _solver;

    std::map<osg::Vec3, std::shared_ptr<btBoxShape>> _boxShapes;

};

#endif // BULLETCLIP_BULLETCLIP_H
