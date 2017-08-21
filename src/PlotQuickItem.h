#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QOpenGLShaderProgram>
#include <QQuickPaintedItem>

#include "PlotCurve.h"
#include "PlotCore.h"


class PlotQuickItem : public QQuickPaintedItem, public PlotCore
{
    Q_OBJECT

    PLOT_PROPERTIES

public:

    PlotQuickItem & operator=(const PlotQuickItem&) = delete;
    PlotQuickItem(const PlotQuickItem&) = delete;

    PlotQuickItem(QQuickItem *parrent = 0);
    virtual ~PlotQuickItem();

     Q_INVOKABLE QPointF  screenToSpace (const QPoint& p) const {return PlotCore::screenToSpace(p);}



private:


    void                              paint (QPainter *painter) Q_DECL_OVERRIDE;

};

#endif // PLOTWIDGET_H
