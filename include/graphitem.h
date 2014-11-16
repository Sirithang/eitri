#ifndef GRAPHITEMS_H
#define GRAPHITEMS_H

#include <QGraphicsItem>

class NodeBox;
class NodeConnector;
class GraphCanvas;

enum ItemType { TypeConnector = QGraphicsItem::UserType + 1,
                TypeBox = QGraphicsItem::UserType + 2 };

class NodeConnector : public QGraphicsItem
{
public:
    NodeBox* owner;
    int id;
    bool isInput;

    //input will always have AT MOST one connected output
    //output can have multiple input connected to them
    QList<NodeConnector*>  _connectedTo;

    //connecting spline. this is owned by the OUTPUT
    //an input will always have 0 spline. If the input owner is deleted
    //it propagate the deletion to the connected output
    QList<QGraphicsPathItem*>   _splines;

    int type() const { return TypeConnector; }

    NodeConnector();
    ~NodeConnector();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);


    void updateSpline();

    void connectTo(NodeConnector* other);
    void disconnect();
};

class NodeBox : public QGraphicsItem
{
public:
    GraphCanvas* owner;
    bool isOutput;

    QList<NodeConnector*>  inConnectors;
    NodeConnector*         outConnector;

    QPixmap preview;

    int op;

    QGraphicsTextItem headerText;
    QGraphicsTextItem sizeText;

    NodeBox(GraphCanvas* pOwner, int pOp, bool pIsOutput);
    virtual ~NodeBox();

    void updatePreview();

    int type() const{ return TypeBox; }

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

public slots:
    void exportResult();

protected:
    //used as optim. Wehn updating preview if node cahced data == this value
    //then we didn't changed the image, so do nothing.
    char* _cachedDataPtr;
};


#endif // GRAPHITEMS_H
