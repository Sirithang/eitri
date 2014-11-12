#include "etrimain.h"
#include "ui_etrimain.h"
#include "eitri.h"

EtriMain::EtriMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EtriMain)
{
    eitri_init();
    ui->setupUi(this);

    ui->graphCanvas->_previewLabel = ui->previewLabel;
    ui->graphCanvas->_paramInspector = ui->paramInsp;
}

EtriMain::~EtriMain()
{
    delete ui;
}
