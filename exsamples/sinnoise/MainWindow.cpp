#include <math.h>

#include <qcolor.h>
#include <qpainter.h>

#include <QMouseEvent>
#include <QPaintEvent>
#include <QMutexLocker>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QTextStream>
#include <QWidget>
#include <QToolTip>
#include <QDialog>

#include "MainWindow.h"

#include <PlotWidget.h>


MainWindow::MainWindow (QWidget *parrent):
QWidget (parrent){
    makeUi (new PlotBehavior (new PlotWidget(this)));
}
MainWindow::MainWindow (PlotBehavior* plotUi, QWidget *parrent):
QWidget (parrent){
    makeUi (plotUi);
}
MainWindow::MainWindow (PlotWidget* plot, QWidget *parrent):
QWidget (parrent){
    //Q_ASSERT(plot);
    makeUi (new PlotBehavior (plot));
}
void MainWindow::makeUi (PlotBehavior* _plotUi){
    //Q_ASSERT(_plotUi);
    plotUi = _plotUi;
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    {
        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        hLayout->setAlignment(Qt::AlignLeft);

        QPushButton *hRescaleButton = new QPushButton ();
        hRescaleButton->setFlat(true);
        hRescaleButton->setMinimumSize (QSize(32,32));
        hRescaleButton->setIcon(QIcon(":res/sizeh.png"));
        QObject::connect(hRescaleButton, SIGNAL(clicked()),
                        this, SLOT(hRescalePress()));
        hLayout->addWidget(hRescaleButton);

        QPushButton *vRescaleButton = new QPushButton ();
        vRescaleButton->setFlat(true);
        vRescaleButton->setMinimumSize (QSize(32,32));
        vRescaleButton->setIcon(QIcon(":res/sizev.png"));
        QObject::connect(vRescaleButton, SIGNAL(clicked()),
                        this, SLOT(vRescalePress()));
        hLayout->addWidget(vRescaleButton);

        QPushButton *propertyButton = new QPushButton ();
        propertyButton->setFlat(true);
        propertyButton->setMinimumSize (QSize(32,32));
        propertyButton->setIcon(QIcon(":res/settings.png"));
        QObject::connect(propertyButton, SIGNAL(clicked()),
                        this, SLOT(propertyPress()));
        hLayout->addWidget(propertyButton);

        hLayout->setAlignment (Qt::AlignLeft);
        layout->addLayout(hLayout);
    }
    layout->addWidget(plotUi);
    setLayout (layout);
}
MainWindow::~MainWindow (){}

void MainWindow::hRescalePress(){
    PlotWidget *plot = plotUi->getPlot();

    QRectF fullRect;
    bool haveCurve = false;
    for (auto curve : plot->getCurves()){
        if ( curve->isEmpty() or not curve->isVisible())
            continue;

        auto curveRect =  curve->getRect();
        auto scale = curve->getScale();
        curveRect = QRectF (curveRect.x()     * scale.x(), curveRect.y()      * scale.y(),
                            curveRect.width() * scale.x(), curveRect.height() * scale.y());
        if (haveCurve)
            fullRect = fullRect.united(curveRect);
        else
            fullRect = curveRect;
        haveCurve = true;
    }

    QRectF viewRect = plot->getViewRect();
    viewRect.setX(fullRect.x());
    viewRect.setWidth(fullRect.width());
    plot->setViewRect(viewRect);
}
void MainWindow::vRescalePress(){
    PlotWidget *plot = plotUi->getPlot();

    QRectF fullRect;
    bool haveCurve = false;
    for (auto curve : plot->getCurves()){
        if ( curve->isEmpty() or not curve->isVisible())
            continue;

        auto curveRect =  curve->getRect();
        auto scale = curve->getScale();
        curveRect = QRectF (curveRect.x()     * scale.x(), curveRect.y()      * scale.y(),
                            curveRect.width() * scale.x(), curveRect.height() * scale.y());
        if (haveCurve)
            fullRect = fullRect.united(curveRect);
        else
            fullRect = curveRect;
        haveCurve = true;
    }

    QRectF viewRect = plot->getViewRect();
    viewRect.setY(fullRect.y());
    viewRect.setHeight(fullRect.height());
    plot->setViewRect(viewRect);
}

void MainWindow::propertyPress(){
    auto* dialog = new QDialog ();
    auto* l = new QStackedLayout( );
    l ->addWidget(new PlotProperty (getPlot()));
    dialog->setLayout(l);
    dialog->setModal(true);
    dialog->show();
}

//------------------------------------------------------------------------------
// PlotProperty
//------------------------------------------------------------------------------
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QPalette>

PlotProperty::PlotProperty(PlotWidget* _plot){
    plot = _plot;
    auto curves = plot->getCurves();

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(new QLabel(tr("Name")),0,0);
    layout->addWidget(new QLabel(tr("Visible")),0,1);
    layout->addWidget(new QLabel(tr("Color")),0,2);
    int i = 0;
    for (auto curve : curves){
        CurveProperty *property = new  CurveProperty(curve,layout, i+1);
        curvesProperty.push_back( property );
        connect (property, SIGNAL(propertyChange()),
                this, SLOT(curvePropertyChanged()));
        i++;
    }
    setLayout (layout);
}

PlotProperty::~PlotProperty(){
}

void PlotProperty::curvePropertyChanged (){
    plot->update();
}

CurveProperty::CurveProperty(PlotCurve* _curve,QGridLayout* layout,const int& n){

    //--------------------------------------------------------------------------
    curve = _curve;
    //--------------------------------------------------------------------------
    nameLabel = new QLabel (curve->getName());

    layout->addWidget(nameLabel,n,0);
    //--------------------------------------------------------------------------
    visibleCheckBox = new QCheckBox ();
    visibleCheckBox->setChecked( curve->isVisible() );
    connect (visibleCheckBox, SIGNAL(stateChanged (int)),
                        this, SLOT(visibleChanged(int)));
    layout->addWidget(visibleCheckBox,n,1);

    //--------------------------------------------------------------------------
    selectColor = new QPushButton();
    selectColor->setAutoFillBackground(true);
    selectColor->setFlat(true);

    QPalette palette = selectColor->palette();
    palette.setColor( QPalette::Button, curve->getColor() );
    palette.setColor( QPalette::BrightText, curve->getColor() );
    selectColor->setPalette( palette );
    selectColor->setFixedWidth(40);

    connect (selectColor, SIGNAL(clicked()),
                        this, SLOT(colorChanged()));

    layout->addWidget(selectColor,n,2);
}


void CurveProperty::visibleChanged (int){
    curve->setVisible  (visibleCheckBox->isChecked());
    emit propertyChange();
}

void CurveProperty::colorChanged (){
    QColor newColor =QColorDialog::getColor ( curve->getColor(), nullptr, tr("Curve color"));
    if (not newColor.isValid())
        return;

    curve->setColor  (newColor);

    QPalette palette = selectColor->palette();
    palette.setColor( QPalette::Button, curve->getColor() );
    palette.setColor( QPalette::BrightText, curve->getColor() );
    selectColor->setPalette( palette );

    emit propertyChange();
}
