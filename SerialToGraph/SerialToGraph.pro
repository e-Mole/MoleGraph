#-------------------------------------------------
#
# Project created by QtCreator 2015-04-27T21:14:15
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ArduinoToGraph
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    qcustomplot/qcustomplot.cpp \
    SerialPort.cpp \
    ChannelSettings.cpp \
    Channel.cpp \
    PortListDialog.cpp \
    PortListWidget.cpp \
    ButtonLine.cpp \
    ChannelSideBar.cpp \
    MyCustomPlot.cpp \
    Graph.cpp \
    CentralLayout.cpp \
    DisplayWidget.cpp

HEADERS  += MainWindow.h \
    qcustomplot/qcustomplot.h \
    SerialPort.h \
    ChannelSettings.h \
    Channel.h \
    PortListDialog.h \
    PortListWidget.h \
    ButtonLine.h \
    ChannelSideBar.h \
    MyCustomPlot.h \
    Graph.h \
    CentralLayout.h \
    DisplayWidget.h

TRANSLATIONS = languages/serialToGraph_cs.ts
