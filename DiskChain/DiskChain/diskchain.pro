#-------------------------------------------------
#
# Project created by QtCreator 2016-11-19T23:03:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = treeViews
TEMPLATE = app

CONFIG   += c++14 precompile_header j8
# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = pch.h

SOURCES += main.cpp\
    mainwindow.cpp \
    pch.cpp \
    mydebug.cpp \
    dcfileinfo.cpp \
    blockchain.cpp

HEADERS  += mainwindow.h \
    pch.h \
    mydebug.h \
    dcfileinfo.h \
    blockchain.h

FORMS    += mainwindow.ui

RESOURCES += icons.qrc
