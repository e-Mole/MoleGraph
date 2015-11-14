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
    MyCustomPlot.cpp \
    Graph.cpp \
    DisplayWidget.cpp \
    CentralWidget.cpp \
    AddAxisDialog.cpp \
    RemoveAxisDialog.cpp \
    FormDialogBase.cpp \
    Axis.cpp

HEADERS  += MainWindow.h \
    qcustomplot/qcustomplot.h \
    SerialPort.h \
    ChannelSettings.h \
    Channel.h \
    PortListDialog.h \
    PortListWidget.h \
    ButtonLine.h \
    MyCustomPlot.h \
    Graph.h \
    DisplayWidget.h \
    CentralWidget.h \
    AddAxisDialog.h \
    RemoveAxisDialog.h \
    FormDialogBase.h \
    Axis.h

TRANSLATIONS = languages/serialToGraph_cs.ts
