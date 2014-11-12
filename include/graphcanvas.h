#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QVector2D>

#include "graphitem.h"

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

    void createOps(QPoint p, QString op);

protected:
    void keyPressEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* event);

    void drawOperation(QPainter* painter);

    QList<OperationBox*> outputsBox;
    QPoint _pressPos;

signals:

public slots:

};

#endif // GRAPHCANVAS_H
