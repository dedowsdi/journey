#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QFont>
#include <QList>
#include <QPainterPath>
#include <QRect>
#include <QWidget>

class QPaintEvent;

enum Operation { NoTransformation, Translate, Rotate, Scale };

// control rendering of a given shape
class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);

    void setOperations(const QList<Operation> &operations);
    void setShape(const QPainterPath &shape);

    // give the RenderArea widget a reasonable size within our application.  The
    // default implementations of these functions returns an invalid size if
    // there is no layout for this widget, and returns the layout's minimum size
    // or preferred size, respectively, otherwise.
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    void drawCoordinates(QPainter &painter);
    void drawOutline(QPainter &painter);
    void drawShape(QPainter &painter);
    void transformPainter(QPainter &painter);

    QList<Operation> operations;
    QPainterPath shape;
    QRect xBoundingRect; // rect of 'x'
    QRect yBoundingRect; // rect of 'y'
};

#endif
