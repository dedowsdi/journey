#include "CanvasWidget.h"
#include <array>
#include <QTimer>

#define BUFFER_OFFSET(bytes) (reinterpret_cast<GLubyte*>(0) + bytes)

CanvasWidget::CanvasWidget(QWidget* parent):
	QOpenGLWidget(parent)
{
	_timer = new QTimer(this);
	_timer->setSingleShot(false);
	_timer->start(16.666);
	connect(_timer, SIGNAL(timeout()), this, SLOT(updateScene()));
}

void CanvasWidget::initializeGL()
{
	initializeOpenGLFunctions();

	// init log if possible
	QOpenGLContext *ctx = QOpenGLContext::currentContext();
	if (ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
	{
		_logger = new QOpenGLDebugLogger(this);
		_logger->initialize();
		_logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
		connect(_logger, &QOpenGLDebugLogger::messageLogged, this, &CanvasWidget::printOpenGLLog);
	}

	glClearColor(0, 0, 0, 1);

	_glf = QOpenGLContext::currentContext()->functions();

	std::array<QVector2D, 6> vertices = {
		QVector2D(-1, -1),
		QVector2D(1, -1),
		QVector2D(1, 1),
		QVector2D(-1, 1)
	};

	std::array<GLuint, 6> indices = {
		0, 1, 2,
		0, 2, 3
	};

	_canvasVao.create();
	_canvasVao.bind();

	_canvasVbo.create();
	_canvasVbo.bind();
	_canvasVbo.allocate(vertices.data(), vertices.size() * sizeof(QVector2D));

	_canvasIbo.create();
	_canvasIbo.bind();
	_canvasIbo.allocate(indices.data(), indices.size() * sizeof(GLuint));

	_glf->glEnableVertexAttribArray(0);
	_glf->glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, 0);

	_program.create();
	_program.addShaderFile(QOpenGLShader::Vertex, "E:\\src\\journey\\qt5\\qt5\\DynamicProgram\\canvas.vert");
	_program.addShaderFile(QOpenGLShader::Fragment, "E:\\src\\journey\\qt5\\qt5\\DynamicProgram\\canvas.frag");
	_program.link();

	if (!_program.isLinked())
	{
		qDebug() << "Link failed";
	}
}

void CanvasWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	_program.bind();
	_canvasVao.bind();
	_canvasIbo.bind();
	_glf->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void CanvasWidget::resizeGL(int w, int h)
{
	qDebug() << "Resize to " << w << " " << h;
	update();
}

void CanvasWidget::printOpenGLLog(const QOpenGLDebugMessage& debugMessage)
{
	qDebug() << debugMessage;
}

void CanvasWidget::updateScene()
{
	_program.update();
	update();
}
