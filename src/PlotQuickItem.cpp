#include "PlotQuickItem.h"

#include <QPainter>

#include <QtDebug>

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

PlotQuickItem::PlotQuickItem(QQuickItem *parrent)
: QQuickPaintedItem(parrent)
{
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
}

PlotQuickItem::~PlotQuickItem()
{
}


//-----------------------------------------------------------------------------
//                             Drawing code
//-----------------------------------------------------------------------------

void  PlotQuickItem::paint (QPainter *painter) {
    PlotCore::resize(width(),height());
    PlotCore::paint(painter);
}
