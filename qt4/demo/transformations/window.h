#ifndef WINDOW_H
#define WINDOW_H

#include <QList>
#include <QPainterPath>
#include <QWidget>

#include "renderarea.h"

class QComboBox;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

public slots:
    void operationChanged();
    void shapeSelected(int index);

private:
    void setupShapes();

    enum { NumTransformedAreas = 3 };
    RenderArea *originalRenderArea;
    RenderArea *transformedRenderAreas[NumTransformedAreas];
    QComboBox *shapeComboBox;
    QComboBox *operationComboBoxes[NumTransformedAreas];
    QList<QPainterPath> shapes;
};

#endif
