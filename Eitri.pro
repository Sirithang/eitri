#-------------------------------------------------
#
# Project created by QtCreator 2014-11-09T13:21:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Eitri
TEMPLATE = app

INCLUDEPATH += $$PWD/include $$PWD/EitriEngine

SOURCES += src/main.cpp\
        src/etrimain.cpp \
    EitriEngine/eitri.cpp \
    src/graphcanvas.cpp \
    src/opsdialog.cpp \
    src/graphitems.cpp \
    src/imagelabel.cpp \
    src/paraminspector.cpp

HEADERS  += include/etrimain.h \
    EitriEngine/eitri.h \
    include/graphcanvas.h \
    include/opsdialog.h \
    include/graphitem.h \
    include/imagelabel.h \
    include/paraminspector.h

FORMS    += etrimain.ui
