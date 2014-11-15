#include "graphcanvas.h"
#include "opsdialog.h"

#include <QKeyEvent>
#include <QDebug>
#include <QFileDialog>

//##################################################


GraphCanvas::GraphCanvas(QWidget *parent) :
    QGraphicsView(parent)
{
    g = new eitri_Graph();
    eitri_createGraph(g);

    setScene(&_scene);

    setSceneRect(5000 * -0.5f, 5000 * -0.5f, 5000, 5000);
    setDragMode(ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    OpsDialog::sAllOps.clear();

    qDebug() <<"Db have : "<<eitri_gOpsDB.opsCount;

    for(int i = 0; i < eitri_gOpsDB.opsCount; ++i)
    {
        OpsDialog::sAllOps.append(eitri_gOpsDB.ops[i].name);
    }

    _selected = NULL;

    outputsBox.clear();

    connect(&_scene,SIGNAL(selectionChanged ()), this, SLOT(updateInspector()));
}

//===========================================


void GraphCanvas::createOps(QPoint p, QString op)
{
    int id = eitri_addOperation(g, op.toLocal8Bit());

    if(id != -1)
    {
        OperationBox* b = new OperationBox(this, id, false);

        b->setPos(mapToScene(mapFromGlobal(p)));
        b->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

        _scene.addItem(b);
    }
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
    else if(e->key() == Qt::Key_Delete)
    {
        if(_selected)
        {
            scene()->removeItem(_selected);
            delete _selected;
        }
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

//=============================================

void GraphCanvas::updateInspector()
{
    QList<QGraphicsItem*> selectedItems = _scene.selectedItems();
    if(selectedItems.count() == 0)
    {
        _selected = NULL;
        _paramInspector->setGraphItem(NULL);
        return;
    }

    blockSignals(true);

    while(selectedItems.count() > 1)
    {
        selectedItems.front()->setSelected(false);
        selectedItems.pop_front();
    }

    _selected = (OperationBox*)selectedItems.front();

    blockSignals(false);

    _paramInspector->setGraphItem(_selected);
}

void GraphCanvas::saveGraph()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save...", "", "*.etrprj");

    if(!filename.isEmpty())
    {
    #define MAX_BUFFER_SIZE 0x10000
        char buffer[MAX_BUFFER_SIZE];

        eitri_serializeGraph(g, buffer, MAX_BUFFER_SIZE);

        QFile f(filename);

        if(!f.open( QIODevice::WriteOnly|QIODevice::Text))
            return;

        f.write(buffer);

        f.close();
    }
}
