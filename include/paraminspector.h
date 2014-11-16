#ifndef PARAMINSPECTOR_H
#define PARAMINSPECTOR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QList>

#include "graphitem.h"

class ParamInspector : public QWidget
{
    Q_OBJECT
public:
    explicit ParamInspector(QWidget *parent = 0);

    void setGraphItem(NodeBox* b);

signals:

public slots:

    void handleSpinBox(int i);
    void handleSpinBoxf(double d);
    void handleTextBox(QString s);
    void handleColorButton();

    void randValue();

protected:
    QVBoxLayout* layout;

    QList<QObject*> _instanciated;

    NodeBox* _opBox;
};

#endif // PARAMINSPECTOR_H
