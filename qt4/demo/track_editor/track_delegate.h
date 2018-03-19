#ifndef TRACKDELEGATE_H
#define TRACKDELEGATE_H

#include <QItemDelegate>

// Qt 4.4 is expected to introduce a QStyledItemDelegate class and use it as the
// default delegate. Unlike QItemDelegate, QStyledItemDelegate would rely on the
// current style to draw its items
class TrackDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    TrackDelegate(int durationColumn, QObject *parent = 0);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    virtual QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

private slots:
    void commitAndCloseEditor();

private:
    int durationColumn;
};

#endif
