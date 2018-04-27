#include <QApplication>

#include "preference_dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PreferenceDialog dialog;
    dialog.show();
    return app.exec();
}
