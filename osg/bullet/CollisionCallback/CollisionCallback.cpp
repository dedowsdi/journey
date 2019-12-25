#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <BulletClip.h>
#include <MotionState.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

// static : mass = 0, isKinematic = false
// dynamic : mass > 0, isKinematic = false
// kinematic : mass 0 , isKinematic = true
btRigidBody* createRigidBody( osg::MatrixTransform* node, const osg::Box* shape,
    GLfloat mass, bool isKinematic = false )
{
    auto pyShape = sgbc.createBoxShape( shape->getHalfLengths() );
    bool isDynamic = mass != 0.0f && !isKinematic;
    auto localInertia = btVector3( 0, 0, 0 );
    if ( isDynamic ) pyShape->calculateLocalInertia( mass, localInertia );
    auto ms = new MotionState( node );

    auto body = new btRigidBody( mass, ms, pyShape.get(), localInertia );
    sgbw.addRigidBody( body );

    if ( isKinematic )
    {
        body->setCollisionFlags( body->getCollisionFlags() |
                                 btCollisionObject::CF_KINEMATIC_OBJECT );
        body->setActivationState( DISABLE_DEACTIVATION );
    }

    return body;
}

btGhostObject* createGhostObject(
    const osg::Matrix& transform, const osg::Box* shape )
{
    auto pyShape = sgbc.createBoxShape( shape->getHalfLengths() );
    auto body = new btPairCachingGhostObject;
    body->setCollisionShape( pyShape.get() );
    body->setWorldTransform( to( transform ) );
    body->setCollisionFlags(body->getCollisionFlags() | btGhostObject::CF_NO_CONTACT_RESPONSE );
    sgbw.addCollisionObject( body );

    return body;
}

osg::MatrixTransform* createCube(
    const osg::Vec3& pos, osg::Box* box, const osg::Vec4& color )
{
    auto tf = new osg::MatrixTransform;
    tf->setMatrix( osg::Matrix::translate( pos ) );

    auto leaf = new osg::Geode;
    tf->addChild( leaf );

    auto shape = new osg::ShapeDrawable( box );
    leaf->addDrawable( shape );

    auto mtl = new osg::Material;
    mtl->setDiffuse( osg::Material::FRONT_AND_BACK, color );

    auto ss = leaf->getOrCreateStateSet();
    ss->setAttribute(mtl);

    return tf;
}

int main( int argc, char* argv[] )
{
    auto root = new osg::Group;

    // static ground
    auto scubeBox = new osg::Box( osg::Vec3(), 100, 100, 2 );
    auto scube =
        createCube( osg::Vec3(), scubeBox, osg::Vec4( 0.8, 0.8, 0.8, 1.0f ) );
    root->addChild( scube );
    createRigidBody( scube, scubeBox, 0 );

    // red dynamic cube
    auto dcubeBox = new osg::Box( osg::Vec3(), 5, 5, 5 );
    auto dcube = createCube(
        osg::Vec3( 0, 0, 30 ), dcubeBox, osg::Vec4( 1.0f, 0.0f, 0.0f, 0.0f ) );
    root->addChild( dcube );
    createRigidBody( dcube, dcubeBox, 10 );

    // green kinematic cube
    auto kcubeBox = new osg::Box( osg::Vec3( 0, 0, 0 ), 5, 5, 5 );
    auto kcube = createCube(
        osg::Vec3( 20, 0, 20 ), kcubeBox, osg::Vec4( 0.0f, 1.0f, 0.0f, 0.0f ) );
    root->addChild( kcube );
    createRigidBody( kcube, kcubeBox, 0 );

    // blue ghost cube
    auto gcubeBox = new osg::Box( osg::Vec3( 0, 0, 0 ), 5, 5, 5 );
    auto gcube = createCube( osg::Vec3( -20, 0, 20 ), gcubeBox,
        osg::Vec4( 0.0f, 0.0f, 1.0f, 0.0f ) );
    root->addChild( gcube );
    createGhostObject( gcube->getMatrix(), gcubeBox );

    osgViewer::Viewer viewer;
    viewer.addEventHandler( new osgViewer::StatsHandler );
    viewer.setSceneData( root );

    sgbc.setDebugRoot( root );
    viewer.addEventHandler( &sgbc );

    return viewer.run();
}
