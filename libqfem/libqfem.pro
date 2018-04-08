include( ../config.pri )

TARGET = qfem

TEMPLATE = lib

QT += widgets

DEPENDPATH += \
    ../libglviewer \
    ../libqstructutility \
    ../libvarplus \
    ../libqload \
    ../libqmaterial \
    ../libqsection

INCLUDEPATH += \
    $$EIGEN_INCLUDE \
    ../libglviewer \
    ../libqstructutility \
    ../libvarplus \
    ../libqload \
    ../libqmaterial \
    ../libqsection

LIBS += \
    -L../bin \
    -lqsection \
    -lqmaterial \
    -lqload \
    -lvarplus \
    -lqstructutility \
    -lglviewer

SOURCES += \
    hypothesis.cpp \
    hypothesis2d.cpp \
    hypothesis3d.cpp \
    vertex.cpp \
    vertexmodel.cpp \
    element.cpp \
    elementmodel.cpp \
    elementvertexmodel.cpp \
    beam.cpp \
    femstructure.cpp \
    ssbeam.cpp \
    ssbeamtimber.cpp \
    beamqmodel.cpp \
    beamq.cpp \
    beamfmodel.cpp \
    beamf.cpp

HEADERS += \
    hypothesis.h \
    hypothesis2d.h \
    hypothesis3d.h \
    vertex.h \
    vertexmodel.h \
    element.h \
    elementmodel.h \
    elementvertexmodel.h \
    beam.h \
    femstructure.h \
    ssbeam.h \
    ssbeamtimber.h \
    beamqmodel.h \
    beamq.h \
    beamfmodel.h \
    beamf.h
