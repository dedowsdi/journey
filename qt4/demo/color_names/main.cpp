#include <QApplication>

#include "color_names_dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ColorNamesDialog dialog;
    dialog.show();
    return app.exec();
}
