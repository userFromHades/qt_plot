QT += core gui opengl widgets
QT += quick quickcontrols2


OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc

TARGET = qml_signal
TEMPLATE = app

CONFIG += c++11


include ($$PWD/../../src/qt_plot.pri)

SOURCES += main.cpp


LIBS +=  -lopengl32 -lglu32
win32{
    LIBS += -lwsock32 -lws2_32
}

RESOURCES += res.qrc

OTHER_FILES += \qml\ui.qml

