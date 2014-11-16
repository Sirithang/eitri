#include "graphitem.h"
#include "graphcanvas.h"

#include <QPointF>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsWidget>
#include <QFileDialog>
#include <QMenu>
#include <QDebug>

#define HEADER_H 20
#define CIRCLE_SIZE 6

//-----------------------------------------

NodeConnector::NodeConnector()
{
    setFlags(0);
}

NodeConnector::~NodeConnector()
{
    disconnect();
}

QRectF NodeConnector::boundingRect() const
{
    return QRectF(-CIRCLE_SIZE,-CIRCLE_SIZE,CIRCLE_SIZE*2,CIRCLE_SIZE*2);
}

void NodeConnector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF bbox = boundingRect();

    painter->setPen(Qt::black);
    painter->setBrush(Qt::yellow);

    painter->drawEllipse(bbox);
}

void NodeConnector::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _splines.push_back(new QGraphicsPathItem());
    scene()->addItem(_splines.back());
}

void NodeConnector::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPainterPath p(this->scenePos());

    p.lineTo(event->scenePos());

    _splines.back()->setPath(p);
}

void NodeConnector::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    _splines.back()->setVisible(false);
    QGraphicsItem* item = scene()->itemAt(event->scenePos(), QTransform());
    _splines.back()->setVisible(true);


    if(item && item->type() == TypeConnector)
    {
        NodeConnector* c = (NodeConnector*)item;
        if(isInput != c->isInput)
        {//we can't pair input w/ input or output w/ output

            //remove the spline. connectTo() handle plugin it where it should be
            scene()->removeItem(_splines.last());
            delete _splines.last();
            _splines.removeLast();

            NodeConnector* input = isInput ? this : c;
            NodeConnector* output = isInput ? c : this;

            connectTo(c);

            eitri_connectNode(owner->owner->g, input->owner->op, output->owner->op, input->id);

            return;//exit the function we have handled things
        }
    }


    //if we reach here, mean the spline need to be removed (didn't connect)
    scene()->removeItem(_splines.back());
    delete _splines.back();
    _splines.removeLast();
}

void NodeConnector::updateSpline()
{
    NodeConnector* target = this;
    if(isInput)
    {
        if(_connectedTo.count() != 0)
            target = _connectedTo[0];
        else
            return; //input & no connection nothing to update
    }

    Q_ASSERT(target->_splines.count() == target->_connectedTo.count());

    for(int i = 0; i < target->_splines.count(); ++i)
    {
        QPainterPath p(target->scenePos());

        p.lineTo (target->_connectedTo[i]->scenePos());

        target->_splines[i]->setPath(p);
    }
}

void NodeConnector::connectTo(NodeConnector *other)
{
    NodeConnector* input = isInput ? this : other;
    NodeConnector* output = isInput ? other : this;

    output->_splines.append(new QGraphicsPathItem());
    scene()->addItem(output->_splines.last());

    if(input->_connectedTo.count() != 0)
    {//input can only get one connections
        input->disconnect();
    }

    input->_connectedTo.push_back(output);
    output->_connectedTo.push_back(input);

    output->updateSpline();
}

void NodeConnector::disconnect()
{
    if(isInput)
    {//if input, just disconnect in graph

        if(owner->op != -1)//if op == -1, op was already deleted & disconnected in the Engine
            eitri_disconnectNode(owner->owner->g, owner->op, id);

        if(_connectedTo.count() == 1)
        {
            int idx = _connectedTo[0]->_connectedTo.indexOf(this);

            qDebug()<<"id in corresponding output "<<idx;

            _connectedTo[0]->_connectedTo.removeAt(idx);

            scene()->removeItem(_connectedTo[0]->_splines[idx]);
            delete  _connectedTo[0]->_splines[idx];
             _connectedTo[0]->_splines.removeAt(idx);
        }
    }
    else
    {//if output, disconnect every input it's connected to.

        qDebug() << "deleting output";
        for(int i = 0; i < _connectedTo.count(); ++i)
        {
            _connectedTo[i]->disconnect();
            scene()->removeItem(_splines[i]);
            delete _splines[i];
        }

        _splines.clear();
    }

    _connectedTo.clear();
}

//-----------------------------------------

NodeBox::NodeBox(GraphCanvas *pOwner, int pOp, bool pIsOutput)
{
    owner = pOwner;
    op = pOp;
    isOutput = pIsOutput;
    _cachedDataPtr = NULL;

    outConnector = NULL;

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

    headerText.setParentItem(this);


    int opTemplate = owner->g->nodes[op].operation;
    int inpCount = eitri_gOpsDB.ops[opTemplate].inputImagesCount;

    int step = boundingRect().height() / inpCount;
    int first = step * 0.5f;

    for(int i = 0 ; i < inpCount; ++i)
    {
        NodeConnector* c = new NodeConnector();
        c->owner = this;
        c->id = i;
        c->isInput = true;

        c->setParentItem(this);
        c->setPos(0, first + step * i);

        inConnectors.push_back(c);
    }

    isOutput = owner->g->nodes[op].isOutput;

    if(!isOutput)
    {
        //OUTPUT
        NodeConnector* c = new NodeConnector();
        c->owner = this;
        c->id = 0;
        c->isInput = false;

        c->setParentItem(this);
        c->setPos(boundingRect().width(), boundingRect().height()*0.5f);

        outConnector = c;
    }

    headerText.setPlainText(eitri_gOpsDB.ops[opTemplate].name);

    sizeText.setPlainText(QString::number(owner->g->nodes[op]._cachedResult.w)
                          + QString("x")
                          + QString::number(owner->g->nodes[op]._cachedResult.h));

    sizeText.setParentItem(this);
    sizeText.setPos(boundingRect().left(), boundingRect().bottom() + 3);

    updatePreview();
}

NodeBox::~NodeBox()
{
    qDebug() << "Deleting op box ";
    eitri_deleteNode(owner->g, op);
    op = -1;

    if(outConnector)
    {
        scene()->removeItem(outConnector);
        delete outConnector;
    }

    for(int i = 0; i < inConnectors.count(); ++i)
    {
        scene()->removeItem(inConnectors[i]);
        delete inConnectors[i];
    }

    inConnectors.clear();
}

void NodeBox::updatePreview()
{    
    eitri_doNode(owner->g, op);

    eitri_NodeInstance* inst = &owner->g->nodes[op];

    if(inst->isOutput)
    {
        headerText.setPlainText(owner->g->outputs[inst->isOutput - 1].name);
    }

    sizeText.setPlainText(QString::number(owner->g->nodes[op]._cachedResult.w)
                          + QString("x")
                          + QString::number(owner->g->nodes[op]._cachedResult.h));


    if(inst->_cachedResult.data)
    {
        QImage img((const uchar*)inst->_cachedResult.data, inst->_cachedResult.w, inst->_cachedResult.h, QImage::Format_ARGB32);

        preview = QPixmap::fromImage(img);

        owner->_previewLabel->setPixmap(preview);

        //_cachedDataPtr = inst->_cachedResult.data;
    }

    update();
}

void NodeBox::exportResult()
{
    QString filename = QFileDialog::getSaveFileName(NULL, "Export...", QString(), "*.png");

    if(filename.isEmpty())
        return;

    updatePreview();

    preview.save(filename, "PNG");
}

QRectF NodeBox::boundingRect() const
 {
     return QRectF(0, 0, 100, 80);
 }

void NodeBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF bbox = boundingRect();

    int bodyH = bbox.height() - HEADER_H;

    if(isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);

    painter->setBrush(QColor(200,200,200));
    painter->drawRoundedRect(0, 0, bbox.width(), bbox.height(), 5, 5);

    if(isOutput)
        painter->setBrush(QColor(255,100,100));
    else
        painter->setBrush(QColor(100,100,100));

    painter->drawRoundedRect(0,0,bbox.width(),HEADER_H,5,5);

    painter->drawPixmap(bbox.width()*0.5f - bodyH*0.5f, HEADER_H, bodyH, bodyH, preview);
}

//----------------------

void NodeBox::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    for(int i = 0; i < inConnectors.count();++i)
    {
        inConnectors[i]->updateSpline();
    }

    if(outConnector)
        outConnector->updateSpline();

    QGraphicsItem::mouseMoveEvent(event);
}

void NodeBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    updatePreview();
}

void NodeBox::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *exportAction = menu.addAction("Export...");

    exportAction->setProperty("opbox", QVariant((int)this));

    QObject::connect(exportAction, SIGNAL(triggered()), owner, SLOT(exportNode()));

    menu.exec(event->screenPos());
}
