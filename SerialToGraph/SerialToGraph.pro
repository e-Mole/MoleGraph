#-------------------------------------------------
#
# Project created by QtCreator 2015-04-27T21:14:15
#
#-------------------------------------------------

QT       += core gui

linux:android
{
INCLUDEPATH += $(ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_NDK_TOOLCHAIN_VERSION)/include
INCLUDEPATH += $(ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_NDK_TOOLCHAIN_VERSION)/libs/armeabi-v7a/include
LIBS += -L$(ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_NDK_TOOLCHAIN_VERSION)/libs/armeabi
}

!linux:android{
QT += serialport
SOURCES += hw/SerialPort.cpp
HEADERS  += hw/SerialPort.h
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ArduinoToGraph
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    qcustomplot/qcustomplot.cpp \
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
    ConnectivityLabel.cpp \
    ChannelMenu.cpp \
    ClickableLabel.cpp \
    ChannelWithTime.cpp \
    AxisChooseDialog.cpp \
    ClickableGroupBox.cpp \
    hw/HwSink.cpp \
    hw/PortBase.cpp \
    hw/Bluetooth.cpp

HEADERS  += MainWindow.h \
    qcustomplot/qcustomplot.h \
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
    ConnectivityLabel.h \
    ChannelMenu.h \
    ClickableLabel.h \
    ChannelWithTime.h \
    AxisChooseDialog.h \
    Serializer.h \
    ClickableGroupBox.h \
    hw/HwSink.h \
    hw/PortBase.h \
    hw/Bluetooth.h

TRANSLATIONS = languages/serialToGraph_cs.ts

CONFIG += c++11
