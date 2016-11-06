#include "PlotBehavior.h"

#include <QStackedLayout>
#include <QToolTip>
#include <QMouseEvent>

#include "PlotWidget.h"

PlotBehavior::PlotBehavior(QWidget *parrent)
:QWidget (parrent){
    plot    = new PlotWidget(this);
    doMove  = false;
    doSelect = false;

    QStackedLayout *l = new QStackedLayout ();
    l->addWidget(plot);
    setLayout(l);
    setAttribute(Qt::WA_StaticContents);
    setMouseTracking(true);

    timer.setSingleShot(true);
    connect(&timer,&QTimer::timeout,[this](){
        QPointF p =  plot->screenToSpace(mousePose);
        QToolTip::hideText();
        QToolTip::showText(this->mapToGlobal(mousePose),QString::number(p.x()) + tr("\n") + QString::number(p.y()));
    });
}

PlotBehavior::PlotBehavior (PlotWidget* _plot,QWidget *parrent)
:QWidget (parrent){
    Q_CHECK_PTR(_plot);

    plot    = _plot;
    doMove  = false;
    doScale  = false;
    doSelect = false;

    QStackedLayout *l = new QStackedLayout ();
    l->addWidget(plot);
    setLayout(l);
    setMouseTracking(true);
    installEventFilter(this);

    timer.setSingleShot(true);
    connect(&timer,&QTimer::timeout,[&](){
        QPointF p =  plot->screenToSpace(mousePose);
        QToolTip::hideText();
        QToolTip::showText(this->mapToGlobal(mousePose),QString::number(p.x()) + tr("\n") + QString::number(p.y()));
    });
}
PlotBehavior::~PlotBehavior(){}
void PlotBehavior::mousePressEvent(QMouseEvent *e){

    doMove = false;
    doScale = false;
    doSelect = false;

    mousePose = QPoint(e->x(),e->y());

    if (e->button() == Qt::MouseButton::LeftButton) {
        doSelect = true;
        plot->setSelectRect (QRectF(e->x(),e->y(),0,0));
        plot->setEnableSelectRect (true);
    }
    if (e->button() == Qt::MouseButton::RightButton) {
        doScale = true;
        plot->setEnableSelectRect (false);
    }
    if (e->button() == Qt::MouseButton::MiddleButton){
        doMove = true;
        plot->setEnableSelectRect (false);
    }

    mousePose = QPoint(e->x(),e->y());

    QWidget::mousePressEvent(e);
}
void PlotBehavior::mouseMoveEvent(QMouseEvent *e){
    QRectF viewRect = plot->getViewRect();

    if (doSelect ){

        qreal _x1 = mousePose.rx();
        qreal _y1 = mousePose.ry();

        qreal _x2 = e->x();
        qreal _y2 = e->y();

        qreal x1 =  _x1 < _x2 ? _x1: _x2;
        qreal x2 =  _x1 < _x2 ? _x2: _x1;

        qreal y1 =  _y1 > _y2 ? _y1: _y2;
        qreal y2 =  _y1 > _y2 ? _y2: _y1;

        plot->setSelectRect(QRect (x1,y1,x2-x1,y2-y1));
        plot->setEnableSelectRect ();

    }
    else if (doScale)
    {
        plot->setEnableSelectRect (false);
        qreal scaleH = pow (2.0,(mousePose.ry() -    e->y())/250.0);
        qreal scaleW = pow (2.0,(mousePose.rx() -    e->x())/250.0);
        mousePose.ry() = e->y();
        mousePose.rx() = e->x();

        qreal w = viewRect.width()*scaleW;
        qreal h = viewRect.height()*scaleH;

        plot->setViewRect(QRectF(viewRect.x()+(viewRect.width()-w)/2,
                                 viewRect.y()+(viewRect.height()-h)/2,
                                 w,h));
    }
    else if (doMove){
        qreal x = viewRect.width() * ( mousePose.rx() - e->x()) / this->width() / 0.84;
        qreal y = viewRect.height()* ( e->y() - mousePose.ry()) / this->height() / 0.80;
        plot->setViewRect( QRectF(viewRect.x()+x,viewRect.y()+y,viewRect.width(),viewRect.height()) );
        mousePose = QPoint(e->x(),e->y());
    }

    QWidget::mouseMoveEvent(e);
}
void PlotBehavior::mouseReleaseEvent(QMouseEvent *e){

    if (e->button() == Qt::MouseButton::LeftButton) {

        QRectF viewRect   = plot->getViewRect  ();

        qreal _x1 = mousePose.rx();
        qreal _y1 = mousePose.ry();

        qreal _x2 = e->x();
        qreal _y2 = e->y();

        qreal x1 =  _x1 < _x2 ? _x1: _x2;
        qreal x2 =  _x1 < _x2 ? _x2: _x1;

        qreal y1 =  _y1 < _y2 ? _y1: _y2;
        qreal y2 =  _y1 < _y2 ? _y2: _y1;

        float p = fabs(x2 - x1) * fabs(y2 - y1) / viewRect.width() / viewRect.height();
        if ( p  > 0.0025f ){
            auto p1 = plot->screenToSpace(QPoint(x1, y1));
            auto p2 = plot->screenToSpace(QPoint(x2, y2));
            plot->setViewRect(QRectF( p1.x(),       p2.y(),
                                      p2.x()-p1.x(),p1.y()-p2.y() ) );
        }

        plot->setEnableSelectRect (false);
    }

    mousePose = QPoint(e->x(),e->y());

    timer.start(500);

    doMove = false;
    doScale = false;
    doSelect = false;

    QWidget::mouseReleaseEvent(e);

}
