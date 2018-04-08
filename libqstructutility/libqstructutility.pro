include( ../config.pri )

TEMPLATE = lib

TARGET = qstructutility

QT += widgets

contains(DEFINES, BUILD_STATIC) {
  CONFIG += staticlib
}

win32 {
    contains(DEFINES, BUILD_SHARED_WIN) {
      DEFINES +=  BUILD_QSTRUCTUTILITY_LIB
    }
}

DEPENDPATH += \

INCLUDEPATH += \
    $$EIGEN_INCLUDE

# Input
HEADERS += \
    qstructutility_export.h \
    structmath.h \
    convertutility.h \
    eigen-core.h \
    qgraphicspointitem.h \
    qgraphicsitemviewer.h \
    functionzeros.h \
    point2dmodel.h

SOURCES += \
    structmath.cpp \
    convertutility.cpp \
    qgraphicspointitem.cpp \
    qgraphicsitemviewer.cpp \
    functionzeros.cpp \
    point2dmodel.cpp
