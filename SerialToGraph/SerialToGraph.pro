#-------------------------------------------------
#
# Project created by QtCreator 2016-02-07T12:21:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ArduinoToGraph
TEMPLATE = app


SOURCES += main.cpp\
    MainWindow.cpp \
    bases/ClickableGroupBox.cpp \
    bases/ClickableLabel.cpp \
    bases/ClickableWidget.cpp \
    bases/FormDialogBase.cpp \
    bases/FormDialogColor.cpp \
    bases/MenuDialogBase.cpp \
    bases/PlatformDialog.cpp \
    hw/Bluetooth.cpp \
    hw/HwSink.cpp \
    hw/PortBase.cpp \    
    qcustomplot/qcustomplot.cpp \
    Axis.cpp \
    AxisChooseDialog.cpp \
    AxisMenu.cpp \
    AxisSettings.cpp \
    ButtonLine.cpp \
    ColorPickerDialog.cpp \
    ConnectivityLabel.cpp \
    Context.cpp \
    Export.cpp \
    Channel.cpp \
    ChannelMenu.cpp \
    ChannelSettings.cpp \
    ChannelWithTime.cpp \
    Measurement.cpp \
    MeasurementMenu.cpp \
    MeasurementSettings.cpp \
    Plot.cpp \
    PortListDialog.cpp \
    PortListWidget.cpp \
    ChannelWidget.cpp

HEADERS  += MainWindow.h \
    bases/ClickableGroupBox.h \
    bases/ClickableLabel.h \
    bases/ClickableWidget.h \
    bases/FormDialogBase.h \
    bases/FormDialogColor.h \
    bases/MenuDialogBase.h \
    bases/PlatformDialog.h \
    hw/Bluetooth.h \
    hw/HwSink.h \
    hw/PortBase.h \
    qcustomplot/qcustomplot.h \
    Axis.h \
    AxisChooseDialog.h \
    AxisMenu.h \
    AxisSettings.h \
    ButtonLine.h \
    ColorPickerDialog.h \
    ConnectivityLabel.h \
    Context.h \
    Export.h \
    Channel.h \
    ChannelMenu.h \
    ChannelSettings.h \
    ChannelWithTime.h \
    Measurement.h \
    MeasurementMenu.h \
    MeasurementSettings.h \
    Plot.h \
    PortListDialog.h \
    PortListWidget.h \
    Serializer.h \
    ChannelWidget.h

CONFIG += mobility
CONFIG += c++11
MOBILITY = 

TRANSLATIONS = languages/serialToGraph_cs.ts

!android{
QT += serialport
SOURCES += hw/SerialPort.cpp
HEADERS += hw/SerialPort.h
}

RESOURCES += \
    translations.qrc


