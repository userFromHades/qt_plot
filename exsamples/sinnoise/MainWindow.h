#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QLabel>
#include <vector>

#include <PlotBehavior.h>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    QPushButton  *moveButton;
    QPushButton *scaleButton;
    QPushButton  *saveButton;
    QPushButton  *loadButton;
    PlotBehavior         *plotUi;

    explicit    MainWindow (QWidget *parrent = 0);
    explicit    MainWindow (PlotBehavior*, QWidget *parrent = 0);
    explicit    MainWindow (PlotWidget*, QWidget *parrent = 0);
    virtual    ~MainWindow ();
    PlotWidget*     getPlot () const { return plotUi ? plotUi->getPlot(): nullptr;}
    PlotBehavior* getPlotUi () const { return plotUi;}
    void           setPlotUi (PlotBehavior* _) {plotUi = _;}

private:
    void              makeUi (PlotBehavior*);
public slots:
    void       hRescalePress ();
    void       vRescalePress ();
    void       propertyPress ();

};

class   PlotCurve;
class     QCheckBox;
class   QPushButton;
class   QGridLayout;
class CurveProperty : public QObject  {
    Q_OBJECT

public:
    PlotCurve        *curve;
    QLabel           *nameLabel;
    QCheckBox  *visibleCheckBox;
    QPushButton    *selectColor;

    CurveProperty (PlotCurve*,QGridLayout*,const int&);

signals:
    void propertyChange ();

public slots:
    void visibleChanged ( int state );
    void colorChanged ();
};

class PlotProperty : public QWidget{
    Q_OBJECT
    PlotWidget* plot;
public :
    PlotProperty (PlotWidget*);
    ~PlotProperty();
private:
    std::vector <CurveProperty*> curvesProperty;
public slots:
    void curvePropertyChanged ();
};

#endif//MAINWINDOW_H
