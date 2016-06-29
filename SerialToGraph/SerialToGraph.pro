#-------------------------------------------------
#
# Project created by QtCreator 2016-02-07T12:21:04
#
#-------------------------------------------------
versionTarget.target = GitVersion.h
versionTarget.depends = FORCE
versionTarget.commands = $$PWD/getGitVersion.bat $$PWD
PRE_TARGETDEPS += GitVersion.h
QMAKE_EXTRA_TARGETS += versionTarget

TARGET = MoleGraph
DEFINES += TARGET=\\\"$$TARGET\\\"
TEMPLATE = app

VERSION = 1
QMAKE_TARGET_COMPANY = e-Mole
QMAKE_TARGET_PRODUCT = $$TARGET
QMAKE_TARGET_DESCRIPTION = "School measuring system based on Arduino"
QMAKE_TARGET_COPYRIGHT = Copyright (c) 2016 e-Mole

QT       += core gui bluetooth widgets printsupport

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
    file/OwnFileDialog.cpp \
    file/FileModel.cpp \
    file/AddDirDialog.cpp \
    bases/ComboBox.cpp

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
    file/OwnFileDialog.h \
    file/FileModel.h \
    file/AddDirDialog.h \
    bases/ComboBox.h

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

win32:RC_ICONS += MoleGraph.ico

RESOURCES += \
    translations.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
