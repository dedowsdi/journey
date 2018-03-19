#include <QApplication>

#include "icon_editor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    IconEditor iconEditor;
    iconEditor.setWindowTitle(QObject::tr("Icon Editor"));
    iconEditor.setIconImage(QImage(":/mouse.png"));
    iconEditor.show();
    return app.exec();
}
