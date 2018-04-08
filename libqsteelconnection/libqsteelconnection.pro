include( ../config.pri )

TEMPLATE = lib

TARGET = qsteelbolt

contains(DEFINES, BUILD_STATIC) {
  CONFIG += staticlib
}

win32 {
    DEFINES += BUILD_LIB
}

DEPENDPATH += \
    ../libqstructutility \
    ../libvarplus \
    ../libqmaterial

INCLUDEPATH += \
    ../libqstructutility \
    ../libvarplus \
    ../libqmaterial

QT += xml

LIBS += \
    -L../bin \
    -lqmaterial \
    -lvarplus \
    -lqstructutility

# Input
HEADERS += \
    steelbolt.h \
    steelboltconnection.h

SOURCES += \
    steelbolt.cpp \
    steelboltconnection.cpp
