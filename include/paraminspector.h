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

    void setGraphItem(OperationBox* b);

signals:

public slots:

    void handleSpinBox(int i);
    void handleTextBox(QString s);
    void handleColorButton();

protected:
    QVBoxLayout* layout;

    QList<QObject*> _instanciated;

    OperationBox* _opBox;
};

#endif // PARAMINSPECTOR_H
