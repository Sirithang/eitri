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


    connect(ui->actionExport, SIGNAL(triggered()), ui->graphCanvas, SLOT(saveGraph()));
    connect(ui->actionImport, SIGNAL(triggered()), ui->graphCanvas, SLOT(importGraph()));

    connect(ui->actionSave, SIGNAL(triggered()), ui->graphCanvas, SLOT(saveProject()));
    connect(ui->actionOpen, SIGNAL(triggered()), ui->graphCanvas, SLOT(openProject()));
}

EtriMain::~EtriMain()
{
    delete ui;
}
