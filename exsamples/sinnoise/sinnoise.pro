
SUBDIRS += src

include (../../src/qt_plot.pri)

HEADERS += \
    MainWindow.h

SOURCES += \
    main.cpp \
    MainWindow.cpp


TARGET = qt_plot

CONFIG += c++11
