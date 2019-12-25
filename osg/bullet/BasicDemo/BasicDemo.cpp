#include <iostream>

#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgViewer/ViewerEventHandlers>
#include <osg/ShapeDrawable>
#include <osg/os_utils>
#include <osgGA/TrackballManipulator>
#include <BulletClip.h>
#include <DebugDrawer.h>
#include <MotionState.h>
#include <memory>

osg::ref_ptr<osg::Group> root;

osg::Node* createCube(
    const osg::Vec3& pos, const osg::Vec3& halfExtent, GLfloat mass )
{
    auto tf = new osg::MatrixTransform;
    tf->setMatrix( osg::Matrix::translate( pos ) );

    auto leaf = new osg::Geode;
    tf->addChild(leaf);

    auto shape = new osg::ShapeDrawable( new osg::Box(
        osg::Vec3(), halfExtent.x() * 2, halfExtent.y() * 2, halfExtent.z() * 2 ) );
    shape->setUseDisplayList(false);
    leaf->addDrawable(shape);

    auto pyShape = sgbc.createBoxShape( halfExtent );
    bool isDynamic = mass != 0.0f;
    auto localInertia = btVector3( 0, 0, 0 );
    if ( isDynamic ) pyShape->calculateLocalInertia( mass, localInertia );
    auto ms = new MotionState( tf );

    auto pyBody = new btRigidBody( mass, ms, pyShape.get(), localInertia );
    sgbw.addRigidBody( pyBody );

    return tf;
}

int main( int argc, char* argv[] )
{
    auto root = new osg::Group;

    // create ground
    auto ground =
        createCube( osg::Vec3( 0, 0, 0 ),
                osg::Vec3( 100, 100, 2 ), 0 );
    root->addChild( ground );

    // create a cube of cubes
    auto size = 8;
    auto cubeSize = 1.0f;

    auto minCorner = -size * cubeSize * 0.5f;
    auto cubeCenter = osg::Vec3( 0, 0, 50 );
    auto halfExtent = osg::Vec3( cubeSize, cubeSize, cubeSize ) * 0.5f;

    for ( int i = 0; i < size; ++i )
    {
        GLfloat x = minCorner + i * cubeSize;
        for ( int j = 0; j < size; ++j )
        {
            GLfloat y = minCorner + j * cubeSize;
            for ( int k = 0; k < size; ++k )
            {
                GLfloat z = minCorner + k * cubeSize;
                auto pos = cubeCenter + osg::Vec3( x, y, z );
                auto cube = createCube( pos, halfExtent, 1.0f );
                root->addChild( cube );
            }
        }
    }

    osgViewer::Viewer viewer;
    viewer.setSceneData( root );
    sgbc.setDebugRoot( root );

    viewer.realize();

    viewer.addEventHandler( &sgbc );
    return viewer.run();
}
