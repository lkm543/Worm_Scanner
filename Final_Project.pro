#-------------------------------------------------
#
# Project created by QtCreator 2014-12-30T00:34:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Final_Project
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += "E:\\OpenCV\\opencv\\build\\include"
        LIBS += -L"E:\\OpenCV\\opencv\build\\x86\\vc12\\lib" \
            -lopencv_core2410d \
            -lopencv_highgui2410d \
            -lopencv_imgproc2410d \
            -lopencv_features2d2410d \
            -lopencv_calib3d2410d
