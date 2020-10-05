#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class CubeWidget3 : public QOpenGLWidget, public QOpenGLFunctions
{
	Q_OBJECT

public:

	CubeWidget3(QWidget* parent = nullptr);

	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;

private slots:
	void updateScene();

	void resetProgram();

private:
	QTimer* _timer = 0;
	QMatrix4x4 _viewMatrix;
	QMatrix4x4 _projMatrix;
	QOpenGLBuffer _cubeVbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	QOpenGLBuffer _cubeIndices = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	QOpenGLVertexArrayObject _cubeVao;
	QOpenGLShaderProgram _program;
	QOpenGLFunctions* _glf = 0;
};

