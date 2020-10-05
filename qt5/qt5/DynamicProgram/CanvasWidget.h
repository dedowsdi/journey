#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include "DynamicProgram.h"

class CanvasWidget : public QOpenGLWidget, public QOpenGLFunctions
{
	Q_OBJECT

public:

	CanvasWidget(QWidget* parent = nullptr);

	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;

private slots:
	void updateScene();

	void printOpenGLLog(const QOpenGLDebugMessage &debugMessage);

private:
	QTimer* _timer = 0;
	DynamicProgram _program;
	QOpenGLVertexArrayObject _canvasVao;
	QOpenGLBuffer _canvasVbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	QOpenGLBuffer _canvasIbo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	QOpenGLFunctions* _glf = 0;
	QOpenGLDebugLogger* _logger = 0;
};

