#include "etrimain.h"
#include "ui_etrimain.h"

EtriMain::EtriMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EtriMain)
{
    ui->setupUi(this);
}

EtriMain::~EtriMain()
{
    delete ui;
}
