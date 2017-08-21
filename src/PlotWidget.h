#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

#include "PlotCurve.h"
#include "PlotCore.h"

class PlotWidget : public QOpenGLWidget, public PlotCore
{
    Q_OBJECT

public:

    PLOT_PROPERTIES

    PlotWidget & operator=(const PlotWidget&) = delete;
    PlotWidget(const PlotWidget&) = delete;
    PlotWidget() = default;

    PlotWidget(QWidget *parent = 0);
    virtual ~PlotWidget();


private:
    float marginForTextH = 0.0;
    float marginForTextW = 0.0;

    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
};

#endif // PLOTWIDGET_H
