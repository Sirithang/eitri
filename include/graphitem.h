#ifndef GRAPHITEMS_H
#define GRAPHITEMS_H

#include <QGraphicsItem>

class OperationBox;
class OperationConnector;
class GraphCanvas;

enum ItemType { TypeConnector = QGraphicsItem::UserType + 1,
                TypeBox = QGraphicsItem::UserType + 2 };

class OperationConnector : public QGraphicsItem
{
public:
    OperationBox* owner;
    int id;
    bool isInput;

    //input will always have AT MOST one connected output
    //output can have multiple input connected to them
    QList<OperationConnector*>  _connectedTo;

    //connecting spline. this is owned by the OUTPUT
    //an input will always have 0 spline. If the input owner is deleted
    //it propagate the deletion to the connected output
    QList<QGraphicsPathItem*>   _splines;

    int type() const { return TypeConnector; }

    OperationConnector();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);


    void updateSpline();
};

class OperationBox : public QGraphicsItem
{
public:
    GraphCanvas* owner;
    bool isOutput;

    QList<OperationConnector*>  inConnectors;
    OperationConnector*         outConnector;

    int op;

    QGraphicsTextItem headerText;

    OperationBox(GraphCanvas* pOwner, int pOp, bool pIsOutput);

    int type() const{ return TypeBox; }

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
};


#endif // GRAPHITEMS_H
