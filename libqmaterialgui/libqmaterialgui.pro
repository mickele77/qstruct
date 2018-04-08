include( ../config.pri )
TEMPLATE = lib

TARGET = qmaterialgui

QT += gui widgets sql

DEPENDPATH += \
    ../libvarplus \
    ../libvarplusgui \
    ../libqcustomplot \
    ../libqmaterial \
    ../libqsection

INCLUDEPATH += \
    $$UI_DIR \
    ../libqstructutility \
    ../libvarplus \
    ../libvarplusgui \
    ../libqcustomplot \
    ../libqload \
    ../libqmaterial \
    ../libqsection

LIBS += -L../bin \
    -lqsection \
    -lqmaterial \
    -lqload \
    -lqcustomplot \
    -lvarplusgui \
    -lvarplus

# Input
SOURCES += \
    materialgui.cpp \
    materialdefaultgui.cpp \
    concretegui.cpp \
    steelgui.cpp \
    steelcncrgui.cpp \
    timbergui.cpp \
    frpgui.cpp \
    existmasonrygui.cpp \
    changerichnamedialog.cpp \
    materialmodelgui.cpp

HEADERS += \
    materialgui.h \
    materialdefaultgui.h \
    concretegui.h \
    steelcncrgui.h \
    steelgui.h \
    timbergui.h \
    frpgui.h \
    existmasonrygui.h \
    changerichnamedialog.h \
    materialmodelgui.h

FORMS += \
    materialdefaultgui.ui \
    concretegui.ui \
    steelcncrgui.ui \
    steelgui.ui \
    timbergui.ui \
    existmasonrygui.ui \
    changerichnamedialog.ui \
    materialmodelgui.ui \
    frpgui.ui

# Resources
RESOURCES +=
