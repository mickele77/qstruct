include( ../config.pri )

TEMPLATE = lib

TARGET = qsoil

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
    ../libqsection

INCLUDEPATH += \
    ../libqstructutility \
    ../libvarplus \
    ../libqsection

QT += xml

LIBS += \
    -L../bin \
    -lqsection \
    -lvarplus \
    -lqstructutility

# Input
HEADERS += \
    soil.h \
    soilmodel.h \
    retainingwall.h \
    retainingwallmodel.h \
    soilsheet.h \
    soilsheetmodel.h

SOURCES += \
    soil.cpp \
    soilmodel.cpp \
    retainingwall.cpp \
    retainingwallmodel.cpp \
    soilsheet.cpp \
    soilsheetmodel.cpp
