#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QWidget>
#include <QPainter>
#include <QVector2D>

#include "eitri.h"

class GraphCanvas;

class OperationBox
{
public:
    GraphCanvas* owner;
    QRect box;

    QColor headerColor;
    unsigned int op;

    OperationBox(QColor header, GraphCanvas* pOwner);
    void draw(QPainter* painter);
};


class GraphCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit GraphCanvas(QWidget *parent = 0);

    Graph* g;
    OperationBox* _selected;

    void createOutput(QPoint p);

protected:
    void keyPressEvent(QKeyEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent * e);
    void paintEvent(QPaintEvent* e);

    void drawOperation(QPainter* painter);

    QList<OperationBox*> outputsBox;
    QPoint _pressPos;

signals:

public slots:

};

#endif // GRAPHCANVAS_H
