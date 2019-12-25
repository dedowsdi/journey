#include <iostream>

#include <MotionState.h>
#include <BulletClip.h>
#include <to.h>
#include <DebugDrawer.h>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/Material>

//--------------------------------------------------------------------
BulletClip::~BulletClip()
{
    for ( auto i = _world->getNumCollisionObjects() - 1; i >= 0; i-- )
    {
        btCollisionObject* obj = _world->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast( obj );
        if ( body && body->getMotionState() )
        {
            delete body->getMotionState();
        }
        _world->removeCollisionObject( obj );
        delete obj;
    }
}

//--------------------------------------------------------------------
bool BulletClip::handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    switch ( ea.getEventType() )
    {
        case osgGA::GUIEventAdapter::FRAME:
        {
            static auto t0 = aa.asView()->getFrameStamp()->getReferenceTime();
            auto t1 = aa.asView()->getFrameStamp()->getReferenceTime();

            if ( _debugging ) _debugDrawer->begin();

            sgbw.stepSimulation( t1 - t0, 2 );
            t0 = t1;

            if ( _debugging )
            {
                sgbw.debugDrawWorld();
                _debugDrawer->end();
            }
        }
        break;

        case osgGA::GUIEventAdapter::KEYDOWN:
        {
            if ( ea.getKey() == _keyEventToggleDebugging )
            {
                if ( _debugRoot )
                {
                    _debugging ^= 1;
                    if ( _debugging )
                    {
                        _debugRoot->addChild( _debugDrawer );
                        std::cout << "turn on debugging" << std::endl;
                    }
                    else
                    {
                        _debugRoot->removeChild( _debugDrawer );
                        std::cout << "turn off debugging" << std::endl;
                    }
                }
            }
            else if ( ea.getKey() == _keyEventToggleContactLifetime )
            {
                _debugDrawer->setDrawCotactLifeTime(
                    !_debugDrawer->getDrawCotactLifeTime() );
            }
            else if ( ea.getKey() == _keyEventToggleContactNormal )
            {
                _debugDrawer->setDrawContactNormal(
                    !_debugDrawer->getDrawContactNormal() );
            }
            else if ( ea.getKey() == _keyEventShotBoxInitalSpeedPlus )
            {
                _shootBoxInitialSpeed += 10;
                OSG_NOTICE << "shoot box initial speed : "
                           << _shootBoxInitialSpeed << std::endl;
            }
            else if ( ea.getKey() == _keyEventShotBoxInitalSpeedMinus )
            {
                _shootBoxInitialSpeed -= 10;
                OSG_NOTICE << "shoot box initial speed : "
                           << _shootBoxInitialSpeed << std::endl;
            }
        }
        break;

        case osgGA::GUIEventAdapter::PUSH:
        {
            if ( ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
            {
                auto view = aa.asView();
                auto camera = view->getCamera();
                auto wMat = camera->getViewport()->computeWindowMatrix();
                auto vpwMat = camera->getViewMatrix() *
                              camera->getProjectionMatrix() *
                              camera->getViewport()->computeWindowMatrix();
                auto vpwMatI = osg::Matrix::inverse( vpwMat );

                // near plane start point, far plane end point.
                auto start = osg::Vec3( ea.getX(), ea.getY(), 0 ) * vpwMatI;
                auto end = osg::Vec3( ea.getX(), ea.getY(), 1 ) * vpwMatI;
                auto dir = end - start;
                dir.normalize();
                shootBox( start, dir );
            }
        }
        break;

        default:
            break;
    }
    return false; // return true will stop event
}

std::shared_ptr<btBoxShape> BulletClip::createBoxShape(
    const osg::Vec3& halfExtent )
{
    auto it = _boxShapes.find( halfExtent );
    if ( it != _boxShapes.end() ) return it->second;

    auto shape = std::make_shared<btBoxShape>( to( halfExtent ) );
    _boxShapes.insert( std::make_pair( halfExtent, shape ) );
    return shape;
}

//--------------------------------------------------------------------
void BulletClip::shootBox( const osg::Vec3& start, const osg::Vec3& dir )
{
    auto tf = new osg::MatrixTransform;
    _shootBoxRoot->addChild( tf );
    tf->setMatrix( osg::Matrix::translate( start ) );

    auto leaf = new osg::Geode;
    tf->addChild( leaf );
    leaf->addDrawable( _shootBoxGeom );

    auto pyShape = createBoxShape( osg::Vec3( 0.5, 0.5, 0.5 ) );
    auto mass = 1.0f;
    auto localInertia = btVector3( 0, 0, 0 );
    pyShape->calculateLocalInertia( mass, localInertia );
    auto ms = new MotionState( tf );

    auto pyBody = new btRigidBody( mass, ms, pyShape.get(), localInertia );
    _world->addRigidBody( pyBody );
    pyBody->setLinearVelocity( to( dir * _shootBoxInitialSpeed ) );
    pyBody->setAngularVelocity( btVector3( 0, 0, 0 ) );
    pyBody->setCcdMotionThreshold( 0.5 );
    pyBody->setCcdSweptSphereRadius(
        0.4f ); // value should be smaller (embedded) than the half extends of
                // the box
}

BulletClip& BulletClip::instance()
{
    static osg::ref_ptr<BulletClip> instance;
    if ( !instance )
    {
        instance = new BulletClip;
    }
    return *instance;
}

void BulletClip::setDebugRoot( osg::Group* v )
{
    _debugRoot = v;
    _debugRoot->addChild( _shootBoxRoot );
}

BulletClip::BulletClip()
{
    // init bullet world
    _config.reset( new btDefaultCollisionConfiguration );
    _dispatcher.reset( new btCollisionDispatcher( _config.get() ) );
    _broadPhase.reset( new btDbvtBroadphase );
    _solver.reset( new btSequentialImpulseConstraintSolver );
    _world.reset( new btDiscreteDynamicsWorld(
        _dispatcher.get(), _broadPhase.get(), _solver.get(), _config.get() ) );
    _world->setGravity( btVector3( 0, 0, -10 ) );

    _debugDrawer = new DebugDrawer;
    _world->setDebugDrawer( _debugDrawer.get() );

    // tick callback
    auto preTick = []( btDynamicsWorld* world, btScalar timeStep ) {
        auto bc = static_cast<BulletClip*>( world->getWorldUserInfo() );
        bc->preTickCallback( world, timeStep );
    };
    _world->setInternalTickCallback( preTick, this, true );

    auto postTick = []( btDynamicsWorld* world, btScalar timeStep ) {
        auto bc = static_cast<BulletClip*>( world->getWorldUserInfo() );
        bc->postTickCallback( world, timeStep );
    };
    _world->setInternalTickCallback( postTick, this, false );

    // shoot box
    _shootBoxGeom =
        new osg::ShapeDrawable( new osg::Box( osg::Vec3(), 1, 1, 1 ) );

    _shootBoxMaterial = new osg::Material;
    _shootBoxMaterial->setDiffuse(
        osg::Material::FRONT_AND_BACK, osg::Vec4( 1, 0, 0, 1 ) );

    _shootBoxRoot = new osg::Group;
    _shootBoxRoot->getOrCreateStateSet()->setAttribute( _shootBoxMaterial );
}

//--------------------------------------------------------------------
void BulletClip::preTickCallback( btDynamicsWorld* world, btScalar timeStep )
{

}

//--------------------------------------------------------------------
void BulletClip::postTickCallback( btDynamicsWorld* world, btScalar timeStep )
{
    auto numManifolds = world->getDispatcher()->getNumManifolds();
    for ( auto i = 0; i < numManifolds; i++ )
    {
        auto contactManifold =
            world->getDispatcher()->getManifoldByIndexInternal( i );
        auto obA = contactManifold->getBody0();
        auto obB = contactManifold->getBody1();

        auto numContacts = contactManifold->getNumContacts();
        for ( auto j = 0; j < numContacts; j++ )
        {
            auto& pt = contactManifold->getContactPoint( j );
            if ( pt.getDistance() < 0.f )
            {
                auto ptA = pt.getPositionWorldOnA();
                auto ptB = pt.getPositionWorldOnB();
                auto& normalOnB = pt.m_normalWorldOnB;
                if (pt.getLifeTime() == 1)
                {
                    std::cout << "collition detected" << std::endl;
                }
                break;
            }
        }
    }
}
