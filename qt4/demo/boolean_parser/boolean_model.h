#ifndef BOOLEANMODEL_H
#define BOOLEANMODEL_H

#include <QAbstractItemModel>

class Node;

class BooleanModel : public QAbstractItemModel
{
public:
    BooleanModel(QObject *parent = 0);
    ~BooleanModel();

    void setRootNode(Node *node);

    virtual QModelIndex index(int row, int column,
                      const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;

private:
    Node *nodeFromIndex(const QModelIndex &index) const;

    Node *rootNode;
};

#endif
