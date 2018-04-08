include( ../config.pri )
TEMPLATE = lib
TARGET = qfem2dgui

QT += opengl xml

DEPENDPATH += \
    ../libglviewer \
    ../libqstructutility \
    ../libvarplus \
    ../libqfem \
    ../libqfemgui \

INCLUDEPATH += \
    $$UI_DIR \
    ../libqfemgui \
    ../libqfem \
    ../libglviewer \
    ../libqstructutility \
    ../libvarplus \
    $$EIGEN_INCLUDE


LIBS += \
    -L../bin \
    -lqfemgui \
    -lqfem \
    -lvarplus \
    -lqstructutility \
    -lglviewer

# Input
SOURCES += \
    qfem2dgui.cpp \
    femstructureviewer.cpp

HEADERS += \
    qfem2dgui.h \
    femstructureviewer.h





