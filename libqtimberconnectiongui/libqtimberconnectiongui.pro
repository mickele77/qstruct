include( ../config.pri )

TARGET = qtimberconnectiongui

TEMPLATE = lib

QT += widgets

DEPENDPATH += \
    ../libqtimberconnection \
    ../libqsection \
    ../libqmaterial \
    ../libqload \
    ../libvarplusgui \
    ../libvarplus \
    ../libqstructutility

INCLUDEPATH += \
    ../libqtimberconnection \
    ../libqsection \
    ../libqmaterial \
    ../libqload \
    ../libvarplusgui \
    ../libvarplus \
    ../libqstructutility

LIBS += \
    -L../bin \
    -lqtimberconnection \
    -lqsection \
    -lqmaterial \
    -lqload \
    -lvarplusgui \
    -lvarplus \
    -lqstructutility

SOURCES += \
    timberconnectionmodelgui.cpp \
    timberconnectiongui.cpp

HEADERS += \
    timberconnectionmodelgui.h \
    timberconnectiongui.h

FORMS += \
    timberconnectionmodelgui.ui \
    timberconnectiongui.ui












