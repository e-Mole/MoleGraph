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
    FormDialogBase.cpp \
    Axis.cpp \
    Context.cpp \
    Plot.cpp \
    Export.cpp \
    FormDialogColor.cpp \
    AxisSettings.cpp \
    Measurement.cpp \
    MeasurementMenu.cpp \
    AxisMenu.cpp \
    MeasurementSettings.cpp \
    MenuDialogBase.cpp \
    ConnectivityLabel.cpp

HEADERS  += MainWindow.h \
    qcustomplot/qcustomplot.h \
    SerialPort.h \
    ChannelSettings.h \
    Channel.h \
    PortListDialog.h \
    PortListWidget.h \
    ButtonLine.h \
    FormDialogBase.h \
    Axis.h \
    Context.h \
    Plot.h \
    Export.h \
    FormDialogColor.h \
    AxisSettings.h \
    Measurement.h \
    MeasurementMenu.h \
    AxisMenu.h \
    MeasurementSettings.h \
    MenuDialogBase.h \
    ConnectivityLabel.h

TRANSLATIONS = languages/serialToGraph_cs.ts
