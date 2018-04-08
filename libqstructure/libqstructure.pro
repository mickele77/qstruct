include( ../config.pri )
TEMPLATE = lib
TARGET = qstructure
QT += widgets

contains(DEFINES, BUILD_STATIC) {
  CONFIG += staticlib
}

win32 {
    DEFINES += BUILD_LIB
}

DEPENDPATH += \
    ../libqstructutility \
    ../libvarplus \
    ../libqload \
    ../libqmaterial \
    ../libqmaterialgui \
    ../libqsection \
    ../libqsectiongui \
    ../libqsoil \
    ../libqsoilgui \
    ../libqfem \
    ../libqfemgui \
    ../libqfem2dgui \
    ../libqtimberconnection \
    ../libqtimberconnectiongui \
    ../libqeasycncr \
    ../libqeasycncrgui \
    ../libqeasy \
    ../libqeasygui

INCLUDEPATH += \
    ../libqstructutility \
    ../libvarplus \
    ../libqload \
    ../libqmaterial \
    ../libqmaterialgui \
    ../libqsection \
    ../libqsectiongui \
    ../libqsoil \
    ../libqsoilgui \
    ../libqfem \
    ../libqfemgui \
    ../libqfem2dgui \
    ../libqtimberconnection \
    ../libqtimberconnectiongui \
    ../libqeasycncr \
    ../libqeasycncrgui \
    ../libqeasy \
    ../libqeasygui \
    $$EIGEN_INCLUDE

LIBS += \
    -L../bin \
    -lqtimberconnectiongui \
    -lqtimberconnection \
    -lqfem2dgui \
    -lqfemgui \
    -lqfem \
    -lqsoil \
    -lqsoilgui \
    -lqsectiongui \
    -lqsection \
    -lqmaterialgui \
    -lqmaterial \
    -lqload \
    -lqeasycncr \
    -lqeasycncrgui \
    -lqeasy \
    -lqeasygui \
    -lvarplus \
    -lqstructutility

# Resources
RESOURCES += \
    ../icons.qrc

# Input
HEADERS += \
    structmodule.h \
    materialmodule.h \
    sectionmodule.h \
    fem2dmodule.h \
    timberconnectionmodule.h \
    soilmodule.h \
    retainingwallmodule.h \
    qstructuregui.h \
    qstructure.h \
    easymodule.h \
    easycncrmodule.h \
    steelconnectionmodule.h \
    soilsheetmodule.h

SOURCES += \
    structmodule.cpp \
    materialmodule.cpp \
    sectionmodule.cpp \
    fem2dmodule.cpp \
    timberconnectionmodule.cpp \
    soilmodule.cpp \
    retainingwallmodule.cpp \
    qstructuregui.cpp \
    qstructure.cpp \
    easymodule.cpp \
    easycncrmodule.cpp \
    steelconnectionmodule.cpp \
    soilsheetmodule.cpp
