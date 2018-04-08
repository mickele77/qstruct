include( ../config.pri )

TARGET = qcustomplot
TEMPLATE = lib

QT       += printsupport

contains(DEFINES, BUILD_STATIC) {
  CONFIG += staticlib
}

win32 {
    contains(DEFINES, BUILD_SHARED_WIN) {
      DEFINES += BUILD_QCUSTOMPLOT_LIB
    }
}

SOURCES += \
    qcustomplot.cpp

HEADERS += \
    qcustomplot.h \
    qcustomplot_export.h

