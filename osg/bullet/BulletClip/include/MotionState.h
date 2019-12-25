#ifndef BULLETCLIP_MOTIONSTATE_H
#define BULLETCLIP_MOTIONSTATE_H

#include <osg/MatrixTransform>
#include <bullet/LinearMath/btMotionState.h>
#include <to.h>

class MotionState : public btMotionState
{

public:
    MotionState( osg::MatrixTransform* transform ) : _transform( transform ) {}

    void getWorldTransform( btTransform& worldTrans ) const override
    {
        if ( _transform->getNumParents() == 0 ||
             _transform->getReferenceFrame() == osg::Transform::ABSOLUTE_RF )
        {
            worldTrans = to( _transform->getMatrix() );
        }
        else
        {
            auto nodes = _transform->getParentalNodePaths();
            worldTrans = to( osg::computeLocalToWorld( nodes.front() ) );
        }
    }

    // Bullet only calls the update of worldtransform for active objects
    void setWorldTransform( const btTransform& worldTrans ) override
    {
        if ( _transform->getNumParents() == 0 ||
             _transform->getReferenceFrame() == osg::Transform::ABSOLUTE_RF )
        {
            _transform->setMatrix( to( worldTrans ) );
        }
        else
        {
            auto parentTransform = _transform->getParent(0)->getWorldMatrices()[0];
            _transform->setMatrix(
                to( worldTrans ) * osg::Matrix::inverse( parentTransform ) );
        }
    }

private:
    osg::ref_ptr<osg::MatrixTransform> _transform;
};

#endif // BULLETCLIP_MOTIONSTATE_H
