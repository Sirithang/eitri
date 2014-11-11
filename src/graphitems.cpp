#include "graphitem.h"
#include "graphcanvas.h"

#include <QPointF>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#define HEADER_H 20
#define CIRCLE_SIZE 6

//-----------------------------------------

OperationConnector::OperationConnector()
{
    setFlags(0);
}

QRectF OperationConnector::boundingRect() const
{
    return QRectF(-CIRCLE_SIZE,-CIRCLE_SIZE,CIRCLE_SIZE*2,CIRCLE_SIZE*2);
}

void OperationConnector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF bbox = boundingRect();

    painter->setPen(Qt::black);
    painter->setBrush(Qt::yellow);

    painter->drawEllipse(bbox);
}

void OperationConnector::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _splines.push_back(new QGraphicsPathItem());
    scene()->addItem(_splines.back());

     //QGraphicsItem::mousePressEvent(event);
}

void OperationConnector::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPainterPath p(this->scenePos());

    //p.cubicTo(pos() + QPointF(1,0), event->pos() + QPointF(1,0), event->pos());

    p.lineTo(event->scenePos());

    _splines.back()->setPath(p);

     //QGraphicsItem::mouseMoveEvent(event);
}

void OperationConnector::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    _splines.back()->setVisible(false);
    QGraphicsItem* item = scene()->itemAt(event->scenePos(), QTransform());
    _splines.back()->setVisible(true);


    if(item && item->type() == TypeConnector)
    {
        OperationConnector* c = (OperationConnector*)item;
        if(isInput != c->isInput)
        {//we can't pair input w/ input or output w/ output



            OperationConnector* input = isInput ? this : c;
            OperationConnector* output = isInput ? c : this;

            if(isInput)
            {//this is an input, it shouldn't get ownership of the path, output need
                c->_splines.push_back(_splines.back());
                _splines.removeLast();
            }

            if(input->_connectedTo.count() != 0)
            {//input can only get one connection
                //@TODO : remove input
                input->_connectedTo.clear();
            }

            input->_connectedTo.push_back(output);
            input->_connectedTo.push_back(input);

            return;//exit the function we have handled things
        }
    }


    //if we reach here, mean the spline need to be removed (didn't connect)
    scene()->removeItem(_splines.back());
    delete _splines.back();
    _splines.removeLast();

    //QGraphicsItem::mouseReleaseEvent(event);
}

void OperationConnector::updateSpline()
{
    OperationConnector* target = this;
    if(isInput && _connectedTo.count() != 0)
    {
        target = _connectedTo[0];
    }

    for(int i = 0; i < _splines.count(); ++i)
    {
        QPainterPath p(target->scenePos());

        p.lineTo(_connectedTo[i]->scenePos());

        _splines[i]->setPath(p);
    }
}

//-----------------------------------------

OperationBox::OperationBox(GraphCanvas *pOwner, int pOp, bool pIsOutput)
{
    owner = pOwner;
    op = pOp;
    isOutput = pIsOutput;

    outConnector = NULL;

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

    headerText.setParentItem(this);

    if(isOutput)
    {
        OperationConnector* c = new OperationConnector();
        c->owner = this;
        c->id = 0;
        c->isInput = true;

        c->setParentItem(this);
        c->setPos(0, boundingRect().height() * 0.5f);

        //c->setFlags();

        inConnectors.push_back(c);

        headerText.setPlainText(owner->g->outputs[op].name);
    }
    else
    {
        int opTemplate = owner->g->operations[op].operation;
        int inpCount = eitri_gOpsDB.ops[opTemplate].inputImagesCount;

        int step = boundingRect().height() / inpCount;
        int first = step * 0.5f;

        for(int i = 0 ; i < inpCount; ++i)
        {
            OperationConnector* c = new OperationConnector();
            c->owner = this;
            c->id = i;
            c->isInput = true;

            c->setParentItem(this);
            c->setPos(0, first + step * i);

            inConnectors.push_back(c);
        }


        //OUTPUT
        OperationConnector* c = new OperationConnector();
        c->owner = this;
        c->id = 0;
        c->isInput = false;

        c->setParentItem(this);
        c->setPos(boundingRect().width(), boundingRect().height()*0.5f);

        outConnector = c;

        headerText.setPlainText(eitri_gOpsDB.ops[opTemplate].name);
    }
}

QRectF OperationBox::boundingRect() const
 {
     return QRectF(0, 0, 100, 80);
 }

void OperationBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF bbox = boundingRect();

    if(isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);

    painter->drawRoundedRect(0, 0, bbox.width(), bbox.height(), 5, 5);

    if(isOutput)
        painter->setBrush(QColor(255,100,100));
    else
        painter->setBrush(QColor(100,100,100));

    painter->drawRoundedRect(0,0,bbox.width(),HEADER_H,5,5);
}

//----------------------

void OperationBox::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    for(int i = 0; i < inConnectors.count();++i)
    {
        inConnectors[i]->updateSpline();
    }

    if(outConnector)
        outConnector->updateSpline();

    QGraphicsItem::dragMoveEvent(event);
}
