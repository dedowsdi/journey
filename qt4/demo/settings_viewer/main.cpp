#include <QApplication>

#include "settings_viewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SettingsViewer settingsViewer;
    settingsViewer.show();
    return app.exec();
}
