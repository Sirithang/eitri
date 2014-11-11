#include "opsdialog.h"
#include <QDebug>
#include <QVBoxLayout>

QList<QString> OpsDialog::sAllOps;

OpsDialog::OpsDialog(QWidget *parent, GraphCanvas* graphOwner) :
    QDialog(parent, Qt::Popup | Qt::FramelessWindowHint),
    _graphOwner(graphOwner)
{
    setModal(false);
    setAttribute(Qt::WA_DeleteOnClose, true);

    QVBoxLayout *layout = new QVBoxLayout;

    _line = new QLineEdit ();
    _opList = new QListWidget();

    layout->addWidget(_line);
    layout->addWidget(_opList);

    connect(_line, SIGNAL(textEdited(QString)), this, SLOT(lineModified(QString)));
    connect(_opList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(opSelected(QListWidgetItem*)));

    setLayout(layout);

    _line->setFocus();
    lineModified("");
}

void OpsDialog::opSelected(QListWidgetItem *item)
{
    if(item->text() == "output")
    {
        _graphOwner->createOutput(this->pos());
        close();
    }
    else
    {
        _graphOwner->createOps(this->pos(), item->text());
        close();
    }
}

void OpsDialog::lineModified(const QString txt)
{
    _opList->clear();

    for(int i = 0; i < sAllOps.count(); ++i)
    {
        if(sAllOps[i].contains(txt))
            _opList->addItem(sAllOps[i]);
    }
}
