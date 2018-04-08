include( ../config.pri )

TEMPLATE = lib

TARGET = qmaterial

QT += xml widgets

contains(DEFINES, BUILD_STATIC) {
  CONFIG += staticlib
}

win32 {
    contains(DEFINES, BUILD_SHARED_WIN) {
      DEFINES += BUILD_MATERIAL_LIB
    }
}

DEPENDPATH += \
    ../libvarplus \
    ../libqload

INCLUDEPATH += \
    ../libqstructutility \
    ../libvarplus \
    ../libqload \
    ../libqsection

LIBS += \
    -L../bin \
    -lqload \
    -lvarplus

# Input
HEADERS += \
    material.h \
    concrete.h \
    steel.h \
    steelcncr.h \
    timber.h \
    frp.h \
    materialmodel.h \
    material_export.h

SOURCES += \
    material.cpp \
    concrete.cpp \
    steel.cpp \
    steelcncr.cpp \
    timber.cpp \
    frp.cpp \
    materialmodel.cpp




