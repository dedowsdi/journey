#ifndef BULLETCLIP_TO_H
#define BULLETCLIP_TO_H

#include <array>

#include <LinearMath/btTransform.h>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Matrix>

inline osg::Vec3 to( const btVector3& v )
{
    return osg::Vec3( v.x(), v.y(), v.z() );
}

inline osg::Vec4 to( const btVector4& v )
{
    return osg::Vec4( v.x(), v.y(), v.z(), v.w() );
}

inline btVector3 to( const osg::Vec3& v )
{
    return btVector3( v.x(), v.y(), v.z() );
}

inline btVector4 to( const osg::Vec4& v )
{
    return btVector4( v.x(), v.y(), v.z(), v.w() );
}

inline osg::Matrix to( const btTransform& v )
{
    auto d = std::array<btScalar, 16>();
    v.getOpenGLMatrix( d.data() );

    // Matrix accept both float and double.
    return osg::Matrix( d.data() );
}

namespace detail
{

template <typename MatrixType>
inline btTransform getBulletTransform( const MatrixType& v, std::true_type )
{
    auto tf = btTransform();
    tf.setFromOpenGLMatrix( v.ptr() );
    return tf;
}

template <typename MatrixType>
inline btTransform getBulletTransform( const MatrixType& v, std::false_type )
{
    auto a = v.ptr();
    auto b = std::array<btScalar, 16>();
    std::copy( a, a + 16, b.data() );

    auto tf = btTransform();
    tf.setFromOpenGLMatrix( b.data() );
    return tf;
}

} // namespace detail

inline btTransform to( const osg::Matrix& v )
{
    return detail::getBulletTransform(
        v, std::is_same<osg::Matrix::value_type, btScalar>() );
}

#endif // BULLETCLIP_TO_H
