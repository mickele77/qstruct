include( ../config.pri )

TARGET = qeasycncrgui
TEMPLATE = lib

QT       += gui widgets

DEPENDPATH += \
    ../libqeasycncr \
    ../libsimpledataplotter \
    ../libvarplusgui \
    ../libvarplus

INCLUDEPATH += \
    ../libqstructutility \
    ../libqeasycncr \
    ../libsimpledataplotter \
    ../libvarplusgui \
    ../libvarplus

LIBS += \
    -L../bin \
    -lqeasycncr \
    -lsimpledataplotter \
    -lvarplusgui \
    -lvarplus

SOURCES += \
    qeasycncrgui.cpp \
    simplesectionrcncrrectgui.cpp \
    simplesectionrcncrcompgui.cpp \
    simplesectionrcncrfrpgui.cpp

HEADERS += \
    qeasycncrgui.h \
    simplesectionrcncrrectgui.h \
    simplesectionrcncrcompgui.h \
    simplesectionrcncrfrpgui.h

FORMS += \
    qeasycncrgui.ui \
    simplesectionrcncrrectgui.ui \
    simplesectionrcncrcompgui.ui \
    simplesectionrcncrfrpgui.ui

