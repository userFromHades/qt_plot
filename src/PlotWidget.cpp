#include "PlotWidget.h"


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

PlotWidget::PlotWidget(QWidget *parent):
    QOpenGLWidget(parent)
{

}

PlotWidget::~PlotWidget()
{
}


//-----------------------------------------------------------------------------
//                             Drawing code
//-----------------------------------------------------------------------------

void PlotWidget::initializeGL()
{
    initializeOpenGL();
}

void PlotWidget::resizeGL(int width, int height)
{
    PlotCore::resize(width,height);
}

void PlotWidget::paintGL()
{
    QPainter p(this);
    PlotCore::paint(&p);
}
