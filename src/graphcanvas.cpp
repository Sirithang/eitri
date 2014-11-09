#include "graphcanvas.h"
#include "opsdialog.h"

#include <QKeyEvent>
#include <QDebug>

#define CIRCLE_WIDTH 6
#define HEADER_H 20

QRectF OperationBox::boundingRect() const
 {
     qreal penWidth = 1;
     return QRectF(-15, -15,
                   115, 95);
 }

void OperationBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF bbox = boundingRect();
    bbox.setWidth(bbox.width() - 30);
    bbox.setHeight(bbox.height() - 30);

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


    int datas[2] = {inputCount, outputCount};
    int xPos[2] = {0, bbox.width()};

    for(int idx = 0; idx < 2; ++idx)
    {
        if(datas[idx] == 0)
            continue;

        int step = (bbox.height() - HEADER_H) / datas[idx];
        int first = step / 2;

        for(int i = 0; i < datas[idx]; ++i)
        {
            painter->setBrush(Qt::yellow);
            painter->drawEllipse(xPos[idx] - CIRCLE_WIDTH, HEADER_H + first + step * i - CIRCLE_WIDTH, CIRCLE_WIDTH*2, CIRCLE_WIDTH*2);
        }
    }
}



//##################################################


GraphCanvas::GraphCanvas(QWidget *parent) :
    QGraphicsView(parent)
{
    g = new Graph();
    createGraph(g);

    setScene(&_scene);

    setSceneRect(5000 * -0.5f, 5000 * -0.5f, 5000, 5000);
    setDragMode(ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    OpsDialog::sAllOps.clear();
    OpsDialog::sAllOps.append("output");

    _selected = NULL;

    outputsBox.clear();
}

//===========================================


void GraphCanvas::createOutput(QPoint p)
{
    int id = addOutput(g);

    OperationBox* b = new OperationBox();
    b->owner = this;
    b->setPos(mapToScene(mapFromGlobal(p)));
    b->op = id;
    b->inputCount = 1;
    b->outputCount = 0;
    b->isOutput = true;

    b->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    _scene.addItem(b);

    this->outputsBox.push_back(b);
}

//===========================================


void GraphCanvas::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Space)
    {
        OpsDialog* d = new OpsDialog(this, this);

        d->move(QCursor::pos());
        d->show();
    }
    else
    {
        QWidget::keyPressEvent(e);
    }
}

//============================================

void GraphCanvas::wheelEvent(QWheelEvent* event) {

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  // Scale the view / do the zoom
  double scaleFactor = 1.15;
  if(event->delta() > 0) {
      // Zoom in
      scale(scaleFactor, scaleFactor);
  } else {
      // Zooming out
      scale(1.0 / scaleFactor, 1.0 / scaleFactor);
  }

  // Don't call superclass handler here
  // as wheel is normally used for moving scrollbars
}
