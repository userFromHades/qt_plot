#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFont>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QQuickView>
#include <QQmlContext>
#include <QTimer>


#include <PlotQuickItem.h>



int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    qmlRegisterType     <PlotQuickItem>     ("qtplot", 0, 1, "PlotWidget");


    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/ui.qml"));

    auto* plot = engine.rootObjects().first()->findChild<PlotQuickItem*>("plot");

    QTimer timer;

    if ( plot != nullptr){

        PlotCurve * curve;

        curve = new PlotCurve("first");
        curve->setColor(QColor("red"));
        plot->addCurve(curve);
        plot->setViewRect(QRectF(-1.0,-1.0,5.0,2.0));

        QObject::connect(&timer,&QTimer::timeout,[&](){

            static int i = 0;

            for (int n = 0; n < 20; n++){
                i++;

                static float p = 0.0;

                p = 0.99 * p;
                for (int j = 0; j < 12; j++)
                    p += 0.02 * (rand()/float(RAND_MAX)-0.5);

                curve->addPoint(i*0.001, p);
            }
            plot->shiftEndTime(i*0.001);
            plot->update();

        });
        timer.start(20);

    }

    return app.exec();
}


