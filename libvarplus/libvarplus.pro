include( ../config.pri )

TEMPLATE = lib

TARGET = varplus

QT += xml widgets

contains(DEFINES, BUILD_STATIC) {
  CONFIG += staticlib
}

win32 {
    contains(DEFINES, BUILD_SHARED_WIN) {
        DEFINES += BUILD_VARPLUS_LIB
    }
}

DEPENDPATH += \
    ../libqstructutility

INCLUDEPATH += \
    ../libqstructutility

LIBS += \
    -L../bin \
    -lqstructutility

# Input
HEADERS += \
    unitmeasure.h \
    unitmeasuregui.h \
    varplus.h \
    doubleplus.h \
    point2dplus.h \
    line2dplus.h \
    point3dplus.h \
    qstringplus.h \
    intplus.h \
    uintplus.h \
    boolplus.h \
    point2dplusmodel.h \
    varpluscontainer.h \
    varplusmath.h \
    enumplus.h \
    tablemodelplus.h \
    tablemodelplusprivate.h \
    varplus_export.h \
    varpluscontainerplus.h

SOURCES += \
    unitmeasure.cpp \
    unitmeasuregui.cpp \
    varplus.cpp \
    doubleplus.cpp \
    point2dplus.cpp \
    line2dplus.cpp \
    point3dplus.cpp \
    qstringplus.cpp \
    intplus.cpp \
    uintplus.cpp \
    boolplus.cpp \
    point2dplusmodel.cpp \
    varpluscontainer.cpp \
    varplusmath.cpp \
    enumplus.cpp \
    tablemodelplus.cpp \
    varpluscontainerplus.cpp
