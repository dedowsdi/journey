#include <QApplication>

#include "diagram_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DiagramWindow view;
    view.show();
    return app.exec();
}
