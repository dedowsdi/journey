#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QSurfaceFormat>
#include <QQuaternion>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Dynamic Program");
    QCoreApplication::setOrganizationName("ZXD");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    // setup surface format
    QSurfaceFormat fmt;
    fmt.setSamples(4);
    fmt.setVersion(3, 3);
    fmt.setRedBufferSize(8); // default
    fmt.setGreenBufferSize(8); // default
    fmt.setBlueBufferSize(8); // default
    fmt.setAlphaBufferSize(8);
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile); // default
    fmt.setOption(QSurfaceFormat::DebugContext); // create debug context
    QSurfaceFormat::setDefaultFormat(fmt);

	//QQuaternion q = QQuaternion::fromDirection(QVector3D(0, -1, 0), QVector3D(0, 0, 1));
	//qDebug() << "Mark";
	//qDebug() << q * QVector3D(1, 0, 0);
	//qDebug() << q * QVector3D(0, 1, 0);
	//qDebug() << q * QVector3D(0, 0, 1);

    MainWindow w;
    w.show();
    return a.exec();
}
