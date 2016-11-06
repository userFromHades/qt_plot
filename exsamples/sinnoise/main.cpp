
#include <math.h>

#include <QApplication>
#include <QMainWindow>
#include <QFile>

#include <PlotWidget.h>

#include "MainWindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QFile file("style.qss");
    if(file.exists()){
        if(file.open(QFile::ReadOnly)){
            QString strCss = QLatin1String(file.readAll());
            app.QApplication::setStyleSheet(strCss);
        }
    }
/*
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    //format.setVersion(2, 0);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
*/
    MainWindow ui;
    PlotWidget *tWidget = ui.getPlot();

    PlotCurve curve_r ("Red");
    curve_r.setColor(QColor(0xAA,0x40,0x40));
    for (int i = -1000; i < 1001; i++)
        curve_r.addPoint( QPointF(i/100.0, 0.5 + 0.5 * sin(3.1415*i/100.0) + float(rand())/float(RAND_MAX)/10.0 ) );

    tWidget->addCurve(&curve_r);

    PlotCurve curve_g ("Green(Scaled)");
    curve_g.setColor(QColor(0x20,0x88,0x20));
    curve_g.setScale(QPointF(1.0,0.5));
    for (int i = -1000; i < 1001; i++)
        curve_g.addPoint( QPointF(i/100.0,-0.5 + 0.5 * sin(3.1415*i/100.0) + float(rand())/float(RAND_MAX)/10.0 ) );

    tWidget->addCurve(&curve_g);
    tWidget->setMinimumSize(QSize(600,400));

    ui.show();

    return app.exec();
}

