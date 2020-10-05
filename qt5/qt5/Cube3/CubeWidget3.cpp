#include "CubeWidget3.h"

#include <QTimer>
#include <QtMath>
#include <array>

#define BUFFER_OFFSET(bytes) (reinterpret_cast<GLubyte*>(0) + bytes)

CubeWidget3::CubeWidget3(QWidget* parent):
	QOpenGLWidget(parent)
{
	_timer = new QTimer;
	_timer->start(16.6666f);
	connect(_timer, SIGNAL(timeout()), this, SLOT(updateScene()));
}

void CubeWidget3::initializeGL()
{
	initializeOpenGLFunctions();
	glClearColor(0, 0.5, 0.5, 1);
	_glf = QOpenGLContext::currentContext()->functions();

	std::array<QVector3D, 8> vertices;
	// top
	vertices[0] = QVector3D(1, 1, 1);
	vertices[1] = QVector3D(-1, 1, 1);
	vertices[2] = QVector3D(-1, -1, 1);
	vertices[3] = QVector3D(1, -1, 1);

	// bottom
	vertices[4] = QVector3D(1, -1, -1);
	vertices[5] = QVector3D(-1, -1, -1);
	vertices[6] = QVector3D(-1, 1, -1);
	vertices[7] = QVector3D(1, 1, -1);

	std::array<QVector3D, 8> colors;
	colors[0] = QVector3D(rand(), rand(), rand()) / RAND_MAX;
	colors[1] = QVector3D(rand(), rand(), rand()) / RAND_MAX;
	colors[2] = QVector3D(rand(), rand(), rand()) / RAND_MAX;
	colors[3] = QVector3D(rand(), rand(), rand()) / RAND_MAX;
	colors[4] = QVector3D(rand(), rand(), rand()) / RAND_MAX;
	colors[5] = QVector3D(rand(), rand(), rand()) / RAND_MAX;
	colors[6] = QVector3D(rand(), rand(), rand()) / RAND_MAX;
	colors[7] = QVector3D(rand(), rand(), rand()) / RAND_MAX;

	std::array<GLuint, 36> indices = {
		0, 1, 2, 0, 2, 3,
		2, 5, 4, 2, 4, 3,
		4, 7, 0, 4, 0, 3,
		4, 5, 6, 4, 6, 7,
		0, 7, 6, 0, 6, 1,
		2, 1, 6, 2, 6, 5
	};

	// create cube
	_cubeVao.create();
	_cubeVao.bind();
	_cubeVbo.create();
	_cubeVbo.bind();
	auto halfSize = vertices.size() * sizeof(QVector3D);
	_cubeVbo.allocate(halfSize * 2);
	glBufferSubData(GL_ARRAY_BUFFER, 0, halfSize, vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, halfSize, halfSize, colors.data());

	_glf->glEnableVertexAttribArray(0);
	_glf->glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
	_glf->glEnableVertexAttribArray(1);
	_glf->glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(halfSize));

	_cubeIndices.create();
	_cubeIndices.bind();
	_cubeIndices.allocate(indices.data(), indices.size() * sizeof(GLuint));

	resetProgram();
}

void CubeWidget3::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	_program.bind();
	auto mvpMatLocation = _program.uniformLocation("mvp_mat");
	assert(mvpMatLocation != -1);
	_program.setUniformValue(mvpMatLocation, _projMatrix * _viewMatrix );

	QOpenGLVertexArrayObject::Binder vaoBinder(&_cubeVao);
	_cubeIndices.bind();
	_glf->glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void CubeWidget3::resizeGL(int w, int h)
{
	qDebug() << "Resize to " << w << " " << h;
	_projMatrix.setToIdentity();
	_projMatrix.perspective(30, static_cast<float>(w) / h, 0.1f, 1000.0f);
	_viewMatrix.setToIdentity();
	_viewMatrix.lookAt(QVector3D(0, -5, 5), QVector3D(0, 0, 0), QVector3D(0, 0, 1));
	update();
}

void CubeWidget3::resetProgram()
{
	// create program
	_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "E:\\src\\journey\\qt5\\qt5\\Cube3\\data\\shader\\cube.vert");
	_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "E:\\src\\journey\\qt5\\qt5\\Cube3\\data\\shader\\cube.frag");
	_program.link();
	if (!_program.isLinked())
	{
		qDebug() << "Failed to link program " << &_program;
	}
}

void CubeWidget3::updateScene()
{
	update();
}
