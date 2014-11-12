#include "paraminspector.h"
#include "graphcanvas.h"
#include "eitri.h"

#include <QSpinBox>
#include <QVBoxLayout>

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
        eitri_Operation* op = &eitri_gOpsDB.ops[b->owner->g->operations[b->op].operation];

        for(int i = 0; i < op->paramsCount; ++i)
        {
            switch(op->params[i].type)
            {
            case EITRI_PARAM_FLOAT:
                {
                    QSpinBox* spinbox = new QSpinBox();
                    layout->addWidget(spinbox);

                    _instanciated.push_back(spinbox);
                }
                break;
            default:
                break;
            }
        }
    }
}

