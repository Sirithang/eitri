#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QVector2D>

#include "imagelabel.h"
#include "graphitem.h"
#include "paraminspector.h"

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

    ImageLabel* _previewLabel;
    ParamInspector* _paramInspector;

    //this create a new op in the graph and add it to the scene
    void createOps(QPoint p, QString op);

    //this add an existing op in the graph (of idx op) to the scene
    void addOps(QPoint p, int op);

    void clearAllNodes();

protected:
    void keyPressEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* event);


    void recreateGraph();

    QList<OperationBox*> outputsBox;
    QPoint _pressPos;

signals:

public slots:
    void updateInspector();
    void exportNode();

    void saveGraph();
    void importGraph();

    //-----------------------------

    //Project are binary file saving next to the graph node positions
    //&other param only used in GUI.
    void saveProject();
    void openProject();
};

#endif // GRAPHCANVAS_H
