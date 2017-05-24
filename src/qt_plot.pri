INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/PlotCurve.h \
    $$PWD/PlotWidget.h \
    $$PWD/PlotBehavior.h \
    $$PWD/glerrorlog.cpp \
    $$PWD/PlotQuickItem.h

SOURCES += \
    $$PWD/PlotCurve.cpp \
    $$PWD/PlotWidget.cpp \
    $$PWD/PlotBehavior.cpp \
    $$PWD/PlotQuickItem.cpp

RESOURCES += $$PWD/../qt_plot.qrc

QT += gui widgets
CONFIG += c++11
