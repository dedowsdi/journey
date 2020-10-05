#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QMatrix4x4>
#include "UVSphere.h"

class CameraManipulator;

class MainScene : public QOpenGLWidget, public QOpenGLFunctions
{
	Q_OBJECT
public:
	MainScene(QWidget* parent = nullptr);

	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;

	void keyPressEvent(QKeyEvent* e) override;
	void mouseMoveEvent(QMouseEvent* e) override;
	void mousePressEvent(QMouseEvent* e) override;
	void wheelEvent(QWheelEvent* e) override;

private slots:
	void updateScene();

	void printOpenGLLog(const QOpenGLDebugMessage& debugMessage);

private:
	QTimer* _timer = 0;
	QOpenGLFunctions* _glf = 0;
	QOpenGLDebugLogger* _logger = 0;
	CameraManipulator* _cameraManipulator = 0;

	GLenum _polygonMode;

	QMatrix4x4 _projMatrix;
	QMatrix4x4 _viewMatrix;
	UVSphere _sphere;
};

