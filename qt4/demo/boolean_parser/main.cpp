#include <QApplication>

#include "boolean_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    BooleanWindow window;
    window.show();
    return app.exec();
}
