#-------------------------------------------------
#
# Project created by QtCreator 2018-01-10T17:18:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UartComHelper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    win_qextserialport.cpp \
    qextserialbase.cpp \
    qextserialport.cpp \
    PortSetting/portsettingdialog.cpp

HEADERS  += mainwindow.h \
    win_qextserialport.h \
    qextserialbase.h \
    qextserialport.h \
    PortSetting/portsettingdialog.h

FORMS    += mainwindow.ui \
    PortSetting/portsettingdialog.ui
