#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QVector2D>

#include "imagelabel.h"
#include "graphitem.h"
#include "paraminspector.h"

#include "eitri.h"

class GraphCanvas;

class GraphCanvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphCanvas(QWidget *parent = 0);

    eitri_Graph* g;
    OperationBox* _selected;
    QGraphicsScene _scene;

    ImageLabel* _previewLabel;
    ParamInspector* _paramInspector;

    void createOps(QPoint p, QString op);

protected:
    void keyPressEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* event);

    QList<OperationBox*> outputsBox;
    QPoint _pressPos;

signals:

public slots:
    void updateInspector();

    void saveGraph();
};

#endif // GRAPHCANVAS_H
