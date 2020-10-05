#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QSurfaceFormat>
#include <QQuaternion>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("TrackBall");
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
#ifdef _DEBUG
    fmt.setOption(QSurfaceFormat::DebugContext); // create debug context
#endif
    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow w;
    w.show();
    return a.exec();
}
