include( ../config.pri )
TARGET = qeasycncr
TEMPLATE = lib

QT       -= gui

contains(DEFINES, BUILD_STATIC) {
  CONFIG += staticlib
}

win32 {
    contains(DEFINES, BUILD_SHARED_WIN) {
      DEFINES += BUILD_EASYCNCR_LIB
    }
}

DEPENDPATH += \
    ../libqstructutility \
    ../libvarplus

INCLUDEPATH += \
    ../libqstructutility \
    ../libvarplus

LIBS += \
    -L../bin \
    -lvarplus \
    -lqstructutility

SOURCES += \
    simplesectionrcncrrect.cpp \
    simplesectionrcncrcomp.cpp \
    steelamodel.cpp \
    steelfepsmodel.cpp \
    slssteelsigmamodel.cpp \
    cncrrectmodel.cpp

HEADERS += \
    simplesectionrcncrrect.h \
    simplesectionrcncrcomp.h \
    steelamodel.h \
    steelfepsmodel.h \
    slssteelsigmamodel.h \
    cncrrectmodel.h \
    easycncr_export.h

