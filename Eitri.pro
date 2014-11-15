#-------------------------------------------------
#
# Project created by QtCreator 2014-11-09T13:21:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Eitri
TEMPLATE = app

INCLUDEPATH += $$PWD/include

SOURCES += src/main.cpp\
        src/etrimain.cpp \
    src/graphcanvas.cpp \
    src/opsdialog.cpp \
    src/graphitems.cpp \
    src/imagelabel.cpp \
    src/paraminspector.cpp

HEADERS  += include/etrimain.h \
    include/graphcanvas.h \
    include/opsdialog.h \
    include/graphitem.h \
    include/imagelabel.h \
    include/paraminspector.h

FORMS    += etrimain.ui

# -- Adding Eitri lib

 win32:CONFIG(release, debug|release): LIBS += -L$$PWD/EitriEngine/lib/release/ -lEitriEngine
 else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/EitriEngine/lib/debug/ -lEitriEngine

 INCLUDEPATH += $$PWD/EitriEngine/interface
 DEPENDPATH += $$PWD/EitriEngine/lib

 win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/EitriEngine/lib/release/EitriEngine.lib
 else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/EitriEngine/lib/debug/EitriEngine.lib
