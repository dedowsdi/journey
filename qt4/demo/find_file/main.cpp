#include <QApplication>

#include "find_file_dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FindFileDialog dialog;
    dialog.show();
    return app.exec();
}
