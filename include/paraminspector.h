#ifndef PARAMINSPECTOR_H
#define PARAMINSPECTOR_H

#include <QWidget>
#include <QLayout>
#include <QList>

#include "graphitem.h"

class ParamInspector : public QWidget
{
    Q_OBJECT
public:
    explicit ParamInspector(QWidget *parent = 0);

    void setGraphItem(OperationBox* b);

signals:

public slots:

protected:
    QLayout* layout;

    QList<QWidget*> _instanciated;

    OperationBox* _opBox;
};

#endif // PARAMINSPECTOR_H
