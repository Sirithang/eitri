#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QWidget>

class ImageLabel : public QWidget
{
    Q_OBJECT

public:
    explicit ImageLabel(QWidget *parent = 0);
    const QPixmap* pixmap() const;

public slots:
    void setPixmap(const QPixmap&);

protected:
    void paintEvent(QPaintEvent *);

private:
    QPixmap pix;
};

#endif // IMAGELABEL_H
