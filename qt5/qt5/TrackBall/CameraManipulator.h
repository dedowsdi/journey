#pragma once
#include <QMatrix4x4>
#include <QQuaternion>
#include <QMouseEvent>

class CameraManipulator
{
public:
	virtual QMatrix4x4 getViewMatrix() = 0;
	virtual QMatrix4x4 getInverseViewMatrix() = 0;
	virtual void mouseMoveEvent(QMouseEvent* event) = 0;
	virtual void mousePressEvent(QMouseEvent* event) = 0;
	virtual void wheelEvent(QWheelEvent* e) = 0;
	virtual void lookAt(const QVector3D& eye, const QVector3D& center, const QVector3D& up) = 0;
};

class OrbitManipulator : public CameraManipulator
{
public:
	QMatrix4x4 getViewMatrix() override;

	QMatrix4x4 getInverseViewMatrix() override;

	void mouseMoveEvent(QMouseEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;

	void wheelEvent(QWheelEvent* e) override;

	void lookAt(const QVector3D& eye, const QVector3D& center, const QVector3D& up);

protected:

	virtual void performMouseDrag() = 0;

	float _distance{1};
	float _wheelScale = 0.001f;
	QPoint _cursor0{ 0, 0 }; // mouse point 0
	QPoint _cursor1{ 0, 0 }; // mouse point 1
	QVector3D _center{0, 0, 0};
	QQuaternion _rotation;
};

class TrackBallManipulator : public OrbitManipulator
{
public:
	float radius() const { return _radius; }
	void setRadius(float v) { _radius = v; }

	const QVector2D& ballCenter() const { return _ballCenter; }
	void setBallCenter(const QVector2D& v) { _ballCenter = v; }

protected:
	void performMouseDrag() override;

	float _radius = 256;
	QVector2D _ballCenter{ 256, 256 };
};

class YawPitchManipulator : public OrbitManipulator
{
public:
	void performMouseDrag() override;

	float rotateScale() const { return _rotateScale; }
	void setRotateScale(float v) { _rotateScale = v; }

private:
	float _rotateScale = 0.2;
};
