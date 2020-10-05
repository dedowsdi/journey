#include "MainScene.h"

#include <array>
#include <QTimer>
#include <QKeyEvent>
#include "CameraManipulator.h"

#define BUFFER_OFFSET(bytes) (reinterpret_cast<GLubyte*>(0) + bytes)

MainScene::MainScene(QWidget* parent) :
    QOpenGLWidget(parent)
{
    _timer = new QTimer(this);
    _timer->setSingleShot(false);
    _timer->start(16.666);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateScene()));
}

void glutErrorFunc(const char* fmt, va_list ap)
{
    qDebug() << fmt;
}

void MainScene::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);

    // init log if possible
    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    if (ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
    {
        _logger = new QOpenGLDebugLogger(this);
        _logger->initialize();
        _logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
        connect(_logger, &QOpenGLDebugLogger::messageLogged, this, &MainScene::printOpenGLLog);
    }

    // print extensions
    //auto extensions = ctx->extensions();
    //qInfo() << "*******************************";
    //qInfo() << "Supported extensions:";
    //for (auto& e : extensions)
    //{
    //    qInfo() << e;
    //}
    //qInfo() << "*******************************";

    // init camera manipulator
    //_cameraManipulator = new TrackBallManipulator;
    _cameraManipulator = new YawPitchManipulator();
	_cameraManipulator->lookAt(QVector3D(0, -5, 5), QVector3D(0, 0, 0), QVector3D(0, 0, 1));

	_glf = QOpenGLContext::currentContext()->functions();

    _sphere.setSlices(32);
    _sphere.setStacks(32);
    _sphere.setLegacy(true);

    GLint polygonModes[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonModes);
    _polygonMode = polygonModes[0];
}

void MainScene::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, _polygonMode);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(_projMatrix.data());

    _viewMatrix = _cameraManipulator->getViewMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(_viewMatrix.data());

    // head light
    glPushMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glPopMatrix();

    _sphere.draw();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void MainScene::resizeGL(int w, int h)
{
    qDebug() << "Resize to " << w << " " << h;

    _projMatrix.setToIdentity();
    _projMatrix.perspective(30, static_cast<float>(w) / h, 0.1f, 1000.0f);

    auto tm = dynamic_cast<TrackBallManipulator*>(_cameraManipulator);
    if (tm)
    {
        tm->setRadius(std::min(w, h) * 0.8f);
        tm->setBallCenter(QVector2D(w * 0.5, h * 0.5));
    }

    //_viewMatrix.setToIdentity();
    //_viewMatrix.lookAt(QVector3D(0, -5, 5), QVector3D(0, 0, 0), QVector3D(0, 0, 1));

    update();
}

void MainScene::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
    case Qt::Key_W:
    {
        _polygonMode = GL_POINT + (_polygonMode - GL_POINT + 1) % 3;
        break;
    }
    default:
        break;
    }

    QWidget::keyPressEvent(e);
}

void MainScene::mouseMoveEvent(QMouseEvent* e)
{
    if (_cameraManipulator)
    {
        _cameraManipulator->mouseMoveEvent(e);
    }
}

void MainScene::mousePressEvent(QMouseEvent* e)
{
    if (_cameraManipulator)
    {
        _cameraManipulator->mousePressEvent(e);
    }
}

void MainScene::wheelEvent(QWheelEvent* e)
{
    if (_cameraManipulator)
    {
        _cameraManipulator->wheelEvent(e);
    }
}

void MainScene::printOpenGLLog(const QOpenGLDebugMessage& debugMessage)
{
    qDebug() << debugMessage;
}

void MainScene::updateScene()
{
    update();
}
