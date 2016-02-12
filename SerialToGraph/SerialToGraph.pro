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
    hw/Bluetooth.cpp \
    hw/HwSink.cpp \
    hw/PortBase.cpp \
    qcustomplot/qcustomplot.cpp \
    Axis.cpp \
    AxisChooseDialog.cpp \
    AxisMenu.cpp \
    AxisSettings.cpp \
    ButtonLine.cpp \
    ClickableGroupBox.cpp \
    ClickableLabel.cpp \
    ConnectivityLabel.cpp \
    Context.cpp \
    Export.cpp \
    FormDialogBase.cpp \
    FormDialogColor.cpp \
    Channel.cpp \
    ChannelMenu.cpp \
    ChannelSettings.cpp \
    ChannelWithTime.cpp \
    Measurement.cpp \
    MeasurementMenu.cpp \
    MeasurementSettings.cpp \
    MenuDialogBase.cpp \
    Plot.cpp \
    PortListDialog.cpp \
    PortListWidget.cpp \
    widgets/PlatformDialog.cpp \
    widgets/ColorPickerDialog.cpp \
    widgets/ClickableWidget.cpp

HEADERS  += MainWindow.h \
    hw/Bluetooth.h \
    hw/HwSink.h \
    hw/PortBase.h \
    qcustomplot/qcustomplot.h \
    Axis.h \
    AxisChooseDialog.h \
    AxisMenu.h \
    AxisSettings.h \
    ButtonLine.h \
    ClickableGroupBox.h \
    ClickableLabel.h \
    ConnectivityLabel.h \
    Context.h \
    Export.h \
    FormDialogBase.h \
    FormDialogColor.h \
    Channel.h \
    ChannelMenu.h \
    ChannelSettings.h \
    ChannelWithTime.h \
    Measurement.h \
    MeasurementMenu.h \
    MeasurementSettings.h \
    MenuDialogBase.h \
    Plot.h \
    PortListDialog.h \
    PortListWidget.h \
    Serializer.h \
    widgets/PlatformDialog.h \
    widgets/ColorPickerDialog.h \
    widgets/ClickableWidget.h

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


