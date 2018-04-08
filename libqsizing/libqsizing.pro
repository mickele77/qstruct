include( ../config.pri )
TEMPLATE = lib
TARGET = qsizing
DESTDIR = ../bin
DEPENDPATH += ../libstructmath \
    ../libqsection
INCLUDEPATH += . \
    ../libstructmath \
    ../libqsection
LIBS += -L../bin \
    -lstructmath \
    -lqsection

# Input
SOURCES += slab.cpp \
    pillar.cpp \
    rectpillar.cpp
HEADERS += slab.h \
    pillar.h \
    rectpillar.h
