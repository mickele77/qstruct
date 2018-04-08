include( ../config.pri )

TARGET = qeasy
TEMPLATE = lib

QT       -= gui

contains(DEFINES, BUILD_STATIC) {
  CONFIG += staticlib
}

win32 {
    contains(DEFINES, BUILD_SHARED_WIN) {
      DEFINES += BUILD_EASY_LIB
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
    loadresultant.cpp \
    bondedanchor.cpp

HEADERS += \
    easy_export.h \
    loadresultant.h \
    bondedanchor.h
