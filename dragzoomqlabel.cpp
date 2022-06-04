#include "dragzoomqlabel.h"
#include <QMouseEvent>
#include <QDebug>

dragZoomQLabel::dragZoomQLabel(QWidget *parent) : QLabel(parent)
{

}

void dragZoomQLabel::mousePressEvent(QMouseEvent *ev)
{

    if (ev->button() == Qt::LeftButton) {
        ev->accept();
    }

}

void dragZoomQLabel::mouseMoveEvent(QMouseEvent *ev)
{
    this->isDragged = true;
    emit moving(QPoint(ev->localPos().x(),ev->localPos().y()));
    ev->accept();

}

void dragZoomQLabel::mouseReleaseEvent(QMouseEvent *ev)
{


    if (ev->button() == Qt::LeftButton) {
        if (this->isDragged){
            this->isDragged = false;
            emit this->moveDone();
        }else{
            emit this->clicked(QPoint(ev->localPos().x(),ev->localPos().y()));
        }
        ev->accept();
    }

}

