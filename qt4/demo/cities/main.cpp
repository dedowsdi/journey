#include <QApplication>
#include <QHeaderView>
#include <QTableView>

#include "city_model.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList cities;
    cities << "Arvika" << "Boden" << "Eskilstuna" << "Falun"
           << "Filipstad" << "Halmstad" << "Helsingborg" << "Karlstad"
           << "Kiruna" << "Kramfors" << "Motala" << "Sandviken"
           << "Skara" << "Stockholm" << "Sundsvall" << "Trelleborg";

    CityModel cityModel;
    cityModel.setCities(cities);

    QTableView tableView;
    tableView.setModel(&cityModel);
    tableView.setAlternatingRowColors(true);
    tableView.setWindowTitle(QObject::tr("Cities"));

    QRect rect = tableView.geometry();
    rect.setWidth(tableView.verticalHeader()->width() + 2 + tableView.columnWidth(0) * cityModel.columnCount());
    rect.setHeight(tableView.horizontalHeader()->height() + 2 + tableView.rowHeight(0) * cityModel.rowCount());
    tableView.setGeometry(rect);

    //tableView.resizeColumnsToContents(); // not working

    tableView.show();

    return app.exec();
}
