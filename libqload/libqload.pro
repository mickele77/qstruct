include( ../config.pri )
TEMPLATE = lib

TARGET = qload

DEPENDPATH += \
    ../libvarplus

INCLUDEPATH += \
    ../libvarplus

QT += xml widgets

LIBS += \
    -L../bin \
    -lvarplus

# Input
HEADERS += \
    load.h \
    loadtablemodel.h \
    loaddurationclass.h \
    loadcase.h \
    loadgroup.h \
    loadcompl.h \
    loadcompltablemodel.h \
    serviceclass.h \
    lstype.h \
    loadtype.h \
    loadcompltabledelegate.h \
    load_export.h

SOURCES += \
    load.cpp \
    loadtablemodel.cpp \
    loadcase.cpp \
    loaddurationclass.cpp \
    loadgroup.cpp \
    loadcompl.cpp \
    loadcompltablemodel.cpp \
    serviceclass.cpp \
    lstype.cpp \
    loadtype.cpp \
    loadcompltabledelegate.cpp
