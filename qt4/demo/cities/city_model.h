#ifndef CITYMODEL_H
#define CITYMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>

class CityModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CityModel(QObject *parent = 0);

    void setCities(const QStringList &cityNames);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value,
                 int role);
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    int offsetOf(int row, int column) const;

    QStringList cities;
    QVector<int> distances; // only store strictly lower triangular dates
};

#endif
