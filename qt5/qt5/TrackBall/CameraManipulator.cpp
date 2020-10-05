#include "CameraManipulator.h"

#include <QWheelEvent>

QMatrix4x4 OrbitManipulator::getViewMatrix()
{
	return getInverseViewMatrix().inverted();
}

QMatrix4x4 OrbitManipulator::getInverseViewMatrix()
{
	QMatrix4x4 m;
	m.translate(_center);
	m.rotate(_rotation);
	m.translate(0, 0, _distance);
	return m;
}

void OrbitManipulator::mouseMoveEvent(QMouseEvent* event)
{
	_cursor0 = _cursor1;
	_cursor1 = event->pos();

	if (event->buttons() == Qt::RightButton)
	{
		performMouseDrag();
	}
}

void OrbitManipulator::mousePressEvent(QMouseEvent* event)
{
	_cursor0 = _cursor1;
	_cursor1 = event->pos();
}

void OrbitManipulator::wheelEvent(QWheelEvent* e)
{
	auto delta = e->angleDelta(); // delta is in 1/8 of degree
	_distance *= 1 - delta.y() * _wheelScale;
}

void OrbitManipulator::lookAt(const QVector3D& eye, const QVector3D& center, const QVector3D& up)
{
	_center = center;
	_distance = (eye - center).length();
	_rotation = QQuaternion::fromDirection(eye - center, up); // direction is z, not -z, be careful
}

//--------------------------------------------------------------------
QVector3D wnd2ball(const QVector2D& p, const QVector2D& center, float radius)
{
	auto pos = p - center;
	pos.setY(-pos.y()); // qt use left top as 0, 0
	auto l2 = pos.lengthSquared();
	auto r2 = radius * radius;
	if (l2 <= r2 * 0.5f)
		return QVector3D(pos, -sqrt(r2 - l2));
	else
		return QVector3D(pos, -r2 * 0.5f / sqrt(l2));
}

//--------------------------------------------------------------------
QQuaternion trackBallRotate(
	const QVector2D& p0, const QVector2D& p1, const QVector2D& center, float radius)
{
	auto sp0 = wnd2ball(p0, center, radius);
	auto sp1 = wnd2ball(p1, center, radius);
	return QQuaternion::rotationTo(sp0, sp1);
}

void TrackBallManipulator::performMouseDrag()
{
	_rotation *= trackBallRotate(QVector2D(_cursor0), QVector2D(_cursor1), _ballCenter, _radius);
}

void YawPitchManipulator::performMouseDrag()
{
	auto offset = _cursor1 - _cursor0;
	offset.setY(-offset.y()); // qt use left top as 0,0
	QQuaternion yaw = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), -offset.x() * _rotateScale);
	QQuaternion pitch = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), offset.y() * _rotateScale);
	_rotation = yaw * _rotation * pitch;
}
