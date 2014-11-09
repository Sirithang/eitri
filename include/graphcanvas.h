#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QVector2D>

#include "eitri.h"

class GraphCanvas;

class OperationBox : public QGraphicsItem
{
public:
    GraphCanvas* owner;
    bool isOutput;

    int outputCount;
    int inputCount;

    int op;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};


class GraphCanvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphCanvas(QWidget *parent = 0);

    Graph* g;
    OperationBox* _selected;
    QGraphicsScene _scene;

    void createOutput(QPoint p);

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
