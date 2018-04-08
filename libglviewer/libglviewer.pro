include( ../config.pri )

QT += gui

TARGET = glviewer

TEMPLATE = lib

SOURCES += \
    glwindow.cpp \
    glitem.cpp \
    glgroup.cpp \
    glline.cpp \
    glpoint.cpp \
    gltriangle.cpp \
    gltext2d.cpp

HEADERS += \
    glwindow.h \
    glitem.h \
    glgroup.h \
    glline.h \
    glpoint.h \
    gltriangle.h \
    gltext2d.h

RESOURCES += \
    shaders.qrc \
    glviewer-icons.qrc

OTHER_FILES += \
    vertex.glsl \
    fragment.glsl
