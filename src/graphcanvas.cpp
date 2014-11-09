#include "graphcanvas.h"
#include "opsdialog.h"

#include <QKeyEvent>
#include <QDebug>


OperationBox::OperationBox(QColor header, GraphCanvas* pOwner):
    owner(pOwner)
{
    box = QRect(0,0,100,80);
    headerColor = header;
}

void OperationBox::draw(QPainter *painter)
{
    painter->save();

    if(owner->_selected == this)
        painter->setPen(Qt::yellow);
    else
        painter->setPen(Qt::black);

    //body
    painter->drawRoundedRect(box, 5.0f, 5.0f);

    painter->setBrush(headerColor);
    //body
    painter->drawRoundedRect(box.x(), box.y(), box.width(), 30, 5.0f, 5.0f);

    painter->restore();
}



//##################################################


GraphCanvas::GraphCanvas(QWidget *parent) :
    QWidget(parent)
{
    g = new Graph();
    createGraph(g);

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    setFocusPolicy(Qt::ClickFocus);

    OpsDialog::sAllOps.clear();
    OpsDialog::sAllOps.append("output");

    _selected = NULL;

    outputsBox.clear();
}

//===========================================

void GraphCanvas::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    qDebug() << "paint";

    for(int i = 0; i < this->outputsBox.count(); ++i)
    {
        this->outputsBox[i]->draw(&p);
    }
}

void GraphCanvas::createOutput(QPoint p)
{
    int id = addOutput(g);

    OperationBox* b = new OperationBox(Qt::red, this);
    b->box.moveTo(mapFromGlobal(p));
    b->op = id;

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

void GraphCanvas::mousePressEvent(QMouseEvent *e)
{
    _pressPos = e->pos();
    _selected = NULL;

    update();

    for(int i = 0; i < outputsBox.count(); ++i)
    {
        if(outputsBox[i]->box.contains(e->pos()))
        {
            _selected = outputsBox[i];
            return;
        }
    }


    QWidget::mousePressEvent(e);
}

void GraphCanvas::mouseReleaseEvent(QMouseEvent *e)
{

}

void GraphCanvas::mouseMoveEvent(QMouseEvent* e)
{
    QPoint delta = e->pos() - _pressPos;

    _pressPos = e->pos();

    if(_selected)
    {
        _selected->box.translate(delta);
        update();
        return;
    }

    QWidget::mouseMoveEvent(e);
}
