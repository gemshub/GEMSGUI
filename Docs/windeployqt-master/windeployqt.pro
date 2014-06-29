#-------------------------------------------------
#
# Project created by QtCreator 2013-07-09T11:46:50
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = windeployqt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

win32: LIBS += -lShlwapi

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

SOURCES += main.cpp \
    utils.cpp

HEADERS += \
    utils.h
