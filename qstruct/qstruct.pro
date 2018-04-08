include( ../config.pri )

TEMPLATE = app

TARGET = QStruct

QT += xml gui widgets

DEPENDPATH += \
    ../libglviewer \
    ../libqstructutility \
    ../libvarplus \
    ../libvarplusgui \
    ../libsimpledataplotter \
    ../libqload \
    ../libqmaterial \
    ../libqmaterialgui \
    ../libqsection \
    ../libqsectiongui \
    ../libqsoil \
    ../libqsoilgui \
    ../libqfem \
    ../libqfemgui \
    ../libqfem2dgui \
    ../libqtimberconnection \
    ../libqtimberconnectiongui \
    ../libqeasy \
    ../libqeasygui \
    ../libqeasycncr \
    ../libqeasycncrgui \
    ../libqstructure

INCLUDEPATH += \
    ../$UI_DIR \
    ../libglviewer \
    ../libqstructutility \
    ../libvarplus \
    ../libvarplusgui \
    ../libsimpledataplotter \
    ../libqload \
    ../libqmaterial \
    ../libqmaterialgui \
    ../libqsection \
    ../libqsectiongui \
    ../libqsoil \
    ../libqsoilgui \
    ../libqfem \
    ../libqfemgui \
    ../libqfem2dgui \
    ../libqtimberconnection \
    ../libqtimberconnectiongui \
    ../libqeasy \
    ../libqeasygui \
    ../libqeasycncr \
    ../libqeasycncrgui \
    ../libqstructure

LIBS += \
    -L../bin \
    -lqstructure \
    -lqtimberconnectiongui \
    -lqtimberconnection \
    -lqfem2dgui \
    -lqfemgui \
    -lqfem \
    -lqsoilgui \
    -lqsoil \
    -lqsectiongui \
    -lqsection \
    -lqmaterialgui \
    -lqmaterial \
    -lqcustomplot \
    -lqeasycncrgui \
    -lqeasycncr \
    -lqeasygui \
    -lqeasy \
    -lqload \
    -lsimpledataplotter \
    -lvarplusgui \
    -lvarplus \
    -lqstructutility \
    -lglviewer

# DEFINES += \

# Input
SOURCES += \
    main.cpp

HEADERS +=
