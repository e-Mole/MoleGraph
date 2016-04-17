#-------------------------------------------------
#
# Project created by QtCreator 2016-02-07T12:21:04
#
#-------------------------------------------------

QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ArduinoToGraph
TEMPLATE = app

GIT_VERSION = $$system(git describe --always --tags)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

SOURCES += main.cpp\
    MainWindow.cpp \
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
    Context.cpp \
    ChannelMenu.cpp \
    ChannelSettings.cpp \
    Measurement.cpp \
    MeasurementMenu.cpp \
    MeasurementSettings.cpp \
    Plot.cpp \
    PortListDialog.cpp \
    hw/PortInfo.cpp \
    GlobalSettings.cpp \
    GlobalSettingsDialog.cpp \
    MyMessageBox.cpp \
    ChannelWidget.cpp \
    Console.cpp \
    ChannelBase.cpp \
    SampleChannel.cpp \
    HwChannel.cpp \
    ColorCheckBox.cpp \
    AboutDialog.cpp \
    file/Export.cpp \
    file/FileDialog.cpp \
    file/NativeFileDialog.cpp \
    file/OwnFileDialog.cpp

HEADERS  += MainWindow.h \
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
    Context.h \
    ChannelMenu.h \
    ChannelSettings.h \
    Measurement.h \
    MeasurementMenu.h \
    MeasurementSettings.h \
    Plot.h \
    PortListDialog.h \
    Serializer.h \
    hw/PortInfo.h \
    GlobalSettings.h \
    GlobalSettingsDialog.h \
    MyMessageBox.h \
    ChannelWidget.h \
    Console.h \
    ChannelBase.h \
    SampleChannel.h \
    HwChannel.h \
    ColorCheckBox.h \
    AboutDialog.h \
    file/Export.h \
    file/FileDialog.h \
    file/NativeFileDialog.h \
    file/OwnFileDialog.h

CONFIG += mobility
CONFIG += c++11
MOBILITY = 

TRANSLATIONS = languages/serialToGraph_cs.ts

!android{
QT += serialport
SOURCES += hw/SerialPort.cpp
HEADERS += hw/SerialPort.h
}

android{
QT += androidextras
}

RESOURCES += \
    translations.qrc


