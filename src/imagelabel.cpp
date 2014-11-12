#include "imagelabel.h"

#include <QPainter>
#include <QPaintEvent>

ImageLabel::ImageLabel(QWidget *parent) :
    QWidget(parent)
{
}

void ImageLabel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    if (pix.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QSize pixSize = pix.size();
    pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);

    QPixmap scaledPix = pix.scaled(pixSize,
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation
                                   );

    painter.drawPixmap(QPoint(), scaledPix);

}

const QPixmap* ImageLabel::pixmap() const {
    return &pix;
}

void ImageLabel::setPixmap (const QPixmap &pixmap){
    pix = pixmap;
    update();
}
