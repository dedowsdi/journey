#ifndef CURRENCYMODEL_H
#define CURRENCYMODEL_H

#include <QAbstractTableModel>
#include <QMap>

class CurrencyModel : public QAbstractTableModel
{
public:
    CurrencyModel(QObject *parent = 0);

    void setCurrencyMap(const QMap<QString, double> &map);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;

private:
    QString currencyAt(int offset) const;

    QMap<QString, double> currencyMap;
};

#endif
