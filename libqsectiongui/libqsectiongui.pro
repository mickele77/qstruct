include( ../config.pri )
TEMPLATE = lib
TARGET = qsectiongui

QT += sql gui widgets

DEPENDPATH += \
    ../libsimpledataplotter \
    ../libqstructutility \
    ../libvarplus \
    ../libvarplusgui \
    ../libqcustomplot \
    ../libqload \
    ../libqmaterial \
    ../libqmaterialgui \
    ../libqsection \
    ../libqfem

INCLUDEPATH += \
    $$UI_DIR \
    ../libsimpledataplotter \
    ../libqstructutility \
    ../libvarplus \
    ../libvarplusgui \
    ../libqcustomplot \
    ../libqload \
    ../libqmaterial \
    ../libqmaterialgui \
    ../libqsection \
    ../libqfem

LIBS += \
    -L../bin \
    -lqfem \
    -lqsection \
    -lqmaterialgui \
    -lqmaterial \
    -lqload \
    -lqcustomplot \
    -lvarplusgui \
    -lvarplus \
    -lqstructutility \
    -lsimpledataplotter

# Input
SOURCES += \
    sectiongeometricdatapanel.cpp \
    sectiongui.cpp \
    sectiondefaultgui.cpp \
    sectionrcncrgui.cpp \
    sectionsteelgui.cpp \
    sectionclassdiagramgui.cpp \
    sectiontimbergui.cpp \
    sectionthingui.cpp \
    sectionthincontgui.cpp \
    sectionthincontsteelgui.cpp \
    sectiontransformpanel.cpp \
    sectionmodelgui.cpp \
    sectionmaterialdelegate.cpp \
    sectionprofilegui.cpp \
    sectionprofiledatapanel.cpp \
    sectionpointsgui.cpp \
    multisectiongui.cpp

    #sectionrcncrrectpanel.cpp
    #sectionfrpgui.cpp \
    #sectionwoodpanel.cpp \

HEADERS += \
    sectiongeometricdatapanel.h \
    sectiongui.h \
    sectiondefaultgui.h \
    sectionrcncrgui.h \
    sectionsteelgui.h \
    sectionclassdiagramgui.h \
    sectiontimbergui.h \
    sectionthingui.h \
    sectionthincontgui.h \
    sectionthincontsteelgui.h \
    sectiontransformpanel.h \
    sectionmodelgui.h \
    sectionmaterialdelegate.h \
    sectionprofilegui.h \
    sectionprofiledatapanel.h \
    sectionpointsgui.h \
    multisectiongui.h

    #sectionrcncrrectpanel.h
    #sectionfrpgui.h \
    #sectionwoodpanel.h \

FORMS += \
    sectiongeometricdatapanel.ui \
    sectiondefaultgui.ui \
    sectionrcncrgui.ui \
    sectionsteelgui.ui \
    sectionclassdiagramgui.ui \
    sectiontimbergui.ui \
    sectionthingui.ui \
    sectionthincontgui.ui \
    sectionthincontsteelgui.ui \
    sectiontransformpanel.ui \
    sectionmodelgui.ui \
    sectionprofilegui.ui \
    sectionprofiledatapanel.ui \
    sectionpointsgui.ui \
    multisectiongui.ui

    #sectionrcncrrectpanel.ui
    #sectionfrpgui.ui \
    #sectionwoodpanel.ui \
