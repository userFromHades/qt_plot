#ifndef PLOTBEHAVIOR_H
#define PLOTBEHAVIOR_H

#include <QWidget>
#include <QTimer>

class PlotWidget;
class PlotCurve;

class PlotBehavior : public QWidget
{
    Q_OBJECT
public:

    explicit       PlotBehavior (QWidget *parrent = 0);
    explicit       PlotBehavior (PlotWidget* plot,QWidget *parrent = 0);

    virtual       ~PlotBehavior ();
    PlotWidget*    getPlot () const {return plot;}
    void            setPlot (PlotWidget* _) {plot = _;}
protected:
    PlotWidget*      plot;
    bool            doMove;
    bool           doScale;
    bool          doSelect;
    QPoint       mousePose;
    QTimer           timer;
    void   mousePressEvent (QMouseEvent *e) Q_DECL_OVERRIDE;
    void    mouseMoveEvent (QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent (QMouseEvent *e) Q_DECL_OVERRIDE;

};

#endif // PLOTBEHAVIOR_H
