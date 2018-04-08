include( ../config.pri )

TARGET = qfemgui

TEMPLATE = lib

QT += gui widgets

DEPENDPATH += \
    ../libqfem \
    ../libqsection \
    ../libqmaterial \
    ../libqcustomplot \
    ../libvarplusgui \
    ../libvarplus \
    ../libqstructutility

INCLUDEPATH += \
    ../libqfem \
    ../libqsection \
    ../libqmaterial \
    ../libqcustomplot \
    ../libvarplusgui \
    ../libvarplus \
    ../libqstructutility \
    $$EIGEN_INCLUDE

LIBS += \
    -L../bin \
    -lqfem \
    -lqsection \
    -lqmaterial \
    -lqcustomplot \
    -lvarplusgui \
    -lvarplus \
    -lqstructutility

SOURCES += \
    vertexmodelgui.cpp \
    elementmodelgui.cpp \
    beamgui.cpp \
    elementgui.cpp \
    elementvertexdelegate.cpp \
    beamqdelegate.cpp \
    beamfdelegate.cpp \
    calcresultsgui.cpp \
    beamstressgui.cpp \
    beamdeformationgui.cpp

HEADERS += \
    vertexmodelgui.h \
    elementmodelgui.h \
    beamgui.h \
    elementgui.h \
    elementvertexdelegate.h \
    beamqdelegate.h \
    beamfdelegate.h \
    calcresultsgui.h \
    beamstressgui.h \
    beamdeformationgui.h \

FORMS += \
    vertexmodelgui.ui \
    elementmodelgui.ui \
    calcresultsgui.ui
