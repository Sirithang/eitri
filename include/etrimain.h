#ifndef ETRIMAIN_H
#define ETRIMAIN_H

#include <QMainWindow>

namespace Ui {
class EtriMain;
}

class EtriMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit EtriMain(QWidget *parent = 0);
    ~EtriMain();

private:
    Ui::EtriMain *ui;
};

#endif // ETRIMAIN_H
