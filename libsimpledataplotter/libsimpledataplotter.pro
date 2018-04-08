include( ../config.pri )

TARGET = simpledataplotter

TEMPLATE = lib

QT       += core gui widgets printsupport

SOURCES += \
    simpledataplotter.cpp \
    simpledataplotterdialog.cpp

HEADERS  += \
    simpledataplotter.h \
    simpledataplotterdialog.h

FORMS += \
    simpledataplotterdialog.ui

RESOURCES += \
    simpledataplottericons.qrc
