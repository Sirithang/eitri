#include "etrimain.h"
#include "ui_etrimain.h"
#include "eitri.h"

EtriMain::EtriMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EtriMain)
{
    eitri_init();
    ui->setupUi(this);
}

EtriMain::~EtriMain()
{
    delete ui;
}
