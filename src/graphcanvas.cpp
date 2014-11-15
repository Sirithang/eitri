#include "graphcanvas.h"
#include "opsdialog.h"

#include <QKeyEvent>
#include <QDebug>
#include <QAction>
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

    addOps(p, id);
}

void GraphCanvas::addOps(QPoint p, int op)
{
    if(op != -1)
    {
        OperationBox* b = new OperationBox(this, op, false);

        b->setPos(mapToScene(mapFromGlobal(p)));
        b->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

        outputsBox.append(b);
        _scene.addItem(b);
    }
}

void GraphCanvas::clearAllNodes()
{
    for(int i = 0; i < outputsBox.count(); ++i)
    {
        delete outputsBox[i];
    }

    outputsBox.clear();
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

void GraphCanvas::recreateGraph()
{
    scene()->clear();
    clearAllNodes();

    for(int i = 0; i < g->operationsCount; ++i)
    {
        //@TODO : better reconstruction than random?
       addOps(QPoint((qrand()%400) - 200, (qrand()%400) - 200), i);
    }

    for(int i = 0; i < outputsBox.count(); ++i)
    {
        for(int j = 0; j < 16; ++j)
        {
            if(g->operations[i].inputs[j] != -1)
            {
                int out = g->operations[i].inputs[j];

                outputsBox[out]->outConnector->connectTo(outputsBox[i]->inConnectors[j]);
            }
        }
    }
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

void GraphCanvas::exportNode()
{
    QAction* a = (QAction*)QObject::sender();

    OperationBox* b = (OperationBox*)a->property("opbox").toInt();

    b->exportResult();
}

void GraphCanvas::saveGraph()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export...", "", "*.eit");

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

void GraphCanvas::importGraph()
{
    QString filename = QFileDialog::getOpenFileName(this, "Import...", "", "*.eit");

    if(!filename.isEmpty())
    {
        QFile f(filename);

        qDebug()<<filename;

        if(!f.open(QIODevice::ReadOnly|QIODevice::Text))
            return;

        QByteArray content = f.readAll();
        eitri_deserializeGraph(g, content.constData());

        recreateGraph();
    }
}

void GraphCanvas::saveProject()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export...", "", "*.eitprj");

    if(!filename.isEmpty())
    {

        QFile f(filename);

        if(!f.open(QIODevice::WriteOnly))
            return;

    #define MAX_BUFFER_SIZE 0x10000
        char buffer[MAX_BUFFER_SIZE];

        eitri_serializeGraph(g, buffer, MAX_BUFFER_SIZE);

        QByteArray b(buffer);

        QDataStream out(&f);
        out.setVersion(QDataStream::Qt_5_0);

        out << b;

        for(int i = 0; i < outputsBox.count(); ++i)
        {
            out << outputsBox[i]->pos();
        }

        f.close();
    }
}

void GraphCanvas::openProject()
{
    QString filename = QFileDialog::getOpenFileName(this, "Import...", "", "*.eitprj");

    if(!filename.isEmpty())
    {
        QFile f(filename);

        qDebug()<<filename;

        if(!f.open(QIODevice::ReadOnly))
            return;

        QDataStream in(&f);
        in.setVersion(QDataStream::Qt_5_0);

        QByteArray b;

        in >> b;

        eitri_deserializeGraph(g, b.constData());

        recreateGraph();

        for(int i = 0; i < outputsBox.count(); ++i)
        {
            QPointF p;
            in >> p;
            outputsBox[i]->setPos(p);
        }

        for(int i = 0; i < outputsBox.count(); ++i)
        {
            if(outputsBox[i]->outConnector)
                outputsBox[i]->outConnector->updateSpline();
        }

        f.close();
    }
}
