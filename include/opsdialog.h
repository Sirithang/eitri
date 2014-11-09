#ifndef OPSDIALOG_H
#define OPSDIALOG_H

#include "graphcanvas.h"

#include <QDialog>
#include <QList>
#include <QLineEdit>
#include <QListWidget>

class OpsDialog : public QDialog
{
    Q_OBJECT
public:
    static QList<QString> sAllOps;
    explicit OpsDialog(QWidget *parent, GraphCanvas* graphOwner);

signals:

public slots:
    void lineModified(const QString txt);
    void opSelected(QListWidgetItem* item);

protected:
    QLineEdit*   _line;
    QListWidget* _opList;

    GraphCanvas* _graphOwner;
};

#endif // OPSDIALOG_H
