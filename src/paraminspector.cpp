#include "paraminspector.h"
#include "graphcanvas.h"
#include "eitri.h"

#include <QSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QColorDialog>


//---helper func

void setButtonToColor(QPushButton* button, QColor color)
{
    QString s("background: #"
                         + QString(color.red() < 16? "0" : "") + QString::number(color.red(),16)
                         + QString(color.green() < 16? "0" : "") + QString::number(color.green(),16)
                         + QString(color.blue() < 16? "0" : "") + QString::number(color.blue(),16) + ";");
    button->setStyleSheet(s);
    button->update();
}

//----------

ParamInspector::ParamInspector(QWidget *parent) :
    QWidget(parent)
{
    layout = new QVBoxLayout();
    setLayout(layout);
}

void ParamInspector::setGraphItem(OperationBox *b)
{
    while(_instanciated.count() > 0)
    {
        delete _instanciated.back();
        _instanciated.removeLast();
    }

    _opBox = b;

    if(b)
    {
        eitri_OpInstance* inst = &b->owner->g->operations[b->op];
        eitri_Operation* op = &eitri_gOpsDB.ops[b->owner->g->operations[b->op].operation];

        for(int i = 0; i < op->paramsCount; ++i)
        {
            QHBoxLayout* hlayout = new QHBoxLayout();
            QLabel* l = new QLabel();

            l->setText(op->params[i].name);
            hlayout->addWidget(l);

            switch(op->params[i].type)
            {
            case EITRI_PARAM_FLOAT:
                {
                    QSpinBox* spinbox = new QSpinBox();

                    spinbox->setProperty("paramIdx", i);
                    spinbox->setValue(inst->paramsValues[i].fParam);

                    hlayout->addWidget(spinbox),
                    _instanciated.push_back(spinbox);

                    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(handleSpinBox(int)));
                }
                break;
            case EITRI_PARAM_COLOR:
                {
                    QPushButton* button = new QPushButton();
                    button->setProperty("paramIdx", i);

                    QColor c(inst->paramsValues[i].colParam.r
                             ,inst->paramsValues[i].colParam.g
                             ,inst->paramsValues[i].colParam.b
                             ,inst->paramsValues[i].colParam.a);

                    setButtonToColor(button, c);

                    hlayout->addWidget(button);
                    _instanciated.push_back(button);

                    connect(button, SIGNAL(clicked()), this, SLOT(handleColorButton()));
                }
                break;
            default:
                break;
            }

            layout->addLayout(hlayout);

            _instanciated.append(hlayout);
            _instanciated.append(l);
        }
    }
}

void ParamInspector::handleSpinBox(int i)
{
    QSpinBox* s = (QSpinBox*)QObject::sender();

    if(s)
    {
        QVariant v = s->property("paramIdx");

        if(v.isValid())
        {
            _opBox->owner->g->operations[_opBox->op].paramsValues[v.toInt()].fParam = i;

            _opBox->updatePreview();
        }
    }
}

void ParamInspector::handleTextBox(QString s)
{

}

void ParamInspector::handleColorButton()
{
    QPushButton* button = (QPushButton*)QObject::sender();

    if(button)
    {
        QVariant v = button->property("paramIdx");

        if(v.isValid())
        {
            QColor color = QColorDialog::getColor();
            setButtonToColor(button, color);

            eitri_Color* c = &_opBox->owner->g->operations[_opBox->op].paramsValues[v.toInt()].colParam;

            c->r = color.red();
            c->g = color.green();
            c->b = color.blue();
            c->a = color.alpha();
        }
    }
}
