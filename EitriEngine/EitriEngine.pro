#-------------------------------------------------
#
# Project created by QtCreator 2014-11-14T20:53:02
#
#-------------------------------------------------

QT       -= core gui

TARGET = EitriEngine
TEMPLATE = lib
CONFIG += staticlib

SUBDIRS += internal
INCLUDEPATH += internal interface

SOURCES += \
    internal/eitri.cpp \
    internal/eitriNodes.cpp \
    internal/eitriSerialization.cpp \
    internal/jsmn.cpp \
    internal/eitriHelpers.cpp

HEADERS += \
    interface/eitri.h \
    internal/stb_perlin.h \
    internal/eitriInternal.h \
    internal/jsmn.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
