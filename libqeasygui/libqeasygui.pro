include( ../config.pri )

TARGET = qeasygui
TEMPLATE = lib

QT       += gui widgets

DEPENDPATH += \
    ../libqeasy \
    ../libsimpledataplotter \
    ../libvarplusgui \
    ../libvarplus

INCLUDEPATH += \
    ../libqstructutility \
    ../libqeasy \
    ../libsimpledataplotter \
    ../libvarplusgui \
    ../libvarplus

LIBS += \
    -L../bin \
    -lqeasy \
    -lsimpledataplotter \
    -lvarplusgui \
    -lvarplus

SOURCES += \
    loadresultantgui.cpp \
    qeasygui.cpp \
    bondedanchorgui.cpp

HEADERS += \
    loadresultantgui.h \
    qeasygui.h \
    bondedanchorgui.h

FORMS += \
    loadresultantgui.ui \
    qeasygui.ui \
    bondedanchorgui.ui

