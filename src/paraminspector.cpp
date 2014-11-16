#include "paraminspector.h"
#include "graphcanvas.h"
#include "eitri.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QColorDialog>
#include <QFrame>
#include <QLineEdit>


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

void ParamInspector::setGraphItem(NodeBox *b)
{
    while(_instanciated.count() > 0)
    {
        delete _instanciated.back();
        _instanciated.removeLast();
    }

    _opBox = b;

    if(b)
    {
        eitri_NodeInstance* inst = &b->owner->g->nodes[b->op];
        eitri_Node* op = &eitri_gOpsDB.ops[b->owner->g->nodes[b->op].operation];

         QHBoxLayout* hlayout;
         QLabel* l;

        if(op->size == -1)
        {
            hlayout = new QHBoxLayout();
            l = new QLabel();

            l->setText("Size : ");
            l->setToolTip("Size of the image. W == H at the moment");

            hlayout->addWidget(l);

            QSpinBox* sizeSpin = new QSpinBox();
            sizeSpin->setMinimum(1);
            sizeSpin->setMaximum(8192);
            sizeSpin->setKeyboardTracking(false);

            sizeSpin->setValue(inst->_cachedResult.w);

            hlayout->addWidget(sizeSpin);

            connect(sizeSpin, SIGNAL(valueChanged(int)), this, SLOT(handleImgResize(int)));

            QFrame* line = new QFrame();
            line->setObjectName(QString::fromUtf8("line"));
            line->setGeometry(QRect(320, 150, 118, 3));
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);

            layout->addLayout(hlayout);

            layout->addWidget(line);

            _instanciated.append(sizeSpin);
            _instanciated.append(hlayout);
            _instanciated.append(l);
            _instanciated.append(line);
        }

        if(inst->isOutput)
        {
            eitri_GraphOutput* output =  &b->owner->g->outputs[inst->isOutput-1];

            hlayout = new QHBoxLayout();
            l = new QLabel();

            l->setText("Output Name : ");
            l->setToolTip("output name for query from C lib");

             hlayout->addWidget(l);

             QLineEdit* lineInput = new QLineEdit();
             lineInput->setMaxLength(255);
             lineInput->setText(output->name);

             hlayout->addWidget(lineInput);

             connect(lineInput, SIGNAL(editingFinished()), this, SLOT(handleRenaming()));

             QFrame* line = new QFrame();
             line->setObjectName(QString::fromUtf8("line"));
             line->setGeometry(QRect(320, 150, 118, 3));
             line->setFrameShape(QFrame::HLine);
             line->setFrameShadow(QFrame::Sunken);

             layout->addLayout(hlayout);

             layout->addWidget(line);

             _instanciated.append(hlayout);
             _instanciated.append(lineInput);
             _instanciated.append(l);
             _instanciated.append(line);
        }

        for(int i = 0; i < op->paramsCount; ++i)
        {
            hlayout = new QHBoxLayout();
            l = new QLabel();

            l->setText(op->params[i].name);
            l->setToolTip(op->params[i].tip);
            hlayout->addWidget(l);

            switch(op->params[i].type)
            {
            case EITRI_PARAM_FLOAT:
                {
                    QDoubleSpinBox* spinbox = new QDoubleSpinBox();

                    spinbox->setProperty("paramIdx", i);
                    spinbox->setMaximum(FLT_MAX);
                    spinbox->setKeyboardTracking(false);
                    spinbox->setValue(inst->paramsValues[i].fParam);

                    hlayout->addWidget(spinbox),
                    _instanciated.push_back(spinbox);

                    connect(spinbox, SIGNAL(valueChanged(double)), this, SLOT(handleSpinBoxf(double)));
                }
                break;
            case EITRI_PARAM_INT:
                {
                    QSpinBox* spinbox = new QSpinBox();

                    spinbox->setProperty("paramIdx", i);
                    spinbox->setMaximum(INT_MAX);
                    spinbox->setKeyboardTracking(false);

                    spinbox->setValue(inst->paramsValues[i].iParam);

                    hlayout->addWidget(spinbox),
                    _instanciated.push_back(spinbox);

                    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(handleSpinBox(int)));


                    QPushButton* b = new QPushButton("rand");

                    b->setProperty("linkedSpin", (int)(spinbox));

                    hlayout->addWidget(b);
                    _instanciated.push_back(b);

                    connect(b, SIGNAL(clicked()), this, SLOT(randValue()));
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
            _opBox->owner->g->nodes[_opBox->op].paramsValues[v.toInt()].iParam = i;
            _opBox->owner->g->nodes[_opBox->op].isDirty = 1;

            _opBox->updatePreview();
        }
    }
}

void ParamInspector::handleSpinBoxf(double d)
{
    QSpinBox* s = (QSpinBox*)QObject::sender();

    if(s)
    {
        QVariant v = s->property("paramIdx");

        if(v.isValid())
        {
            _opBox->owner->g->nodes[_opBox->op].paramsValues[v.toInt()].fParam = d;
            _opBox->owner->g->nodes[_opBox->op].isDirty = 1;

            _opBox->updatePreview();
        }
    }
}

void ParamInspector::randValue()
{
    QSpinBox* s = (QSpinBox*)QObject::sender();

    if(s)
    {
        QVariant v = s->property("linkedSpin");

        if(v.isValid())
        {
            QSpinBox* b = (QSpinBox*)v.toInt();

            b->setValue(qrand());
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

            eitri_Color* c = &_opBox->owner->g->nodes[_opBox->op].paramsValues[v.toInt()].colParam;

            c->r = color.red();
            c->g = color.green();
            c->b = color.blue();
            c->a = color.alpha();

            _opBox->owner->g->nodes[_opBox->op].isDirty = 1;
        }
    }
}

void ParamInspector::handleImgResize(int i)
{
    eitri_resizeNode(_opBox->owner->g, _opBox->op, i);
    _opBox->owner->g->nodes[_opBox->op].isDirty = 1;
    _opBox->updatePreview();
}

void ParamInspector::handleRenaming()
{
    QLineEdit* line = (QLineEdit*)QObject::sender();

    eitri_NodeInstance* inst = &_opBox->owner->g->nodes[_opBox->op];

    strncpy(_opBox->owner->g->outputs[inst->isOutput-1].name, (const char*)line->text().toLocal8Bit().constData(), line->text().length()+1);

    _opBox->updatePreview();
}
