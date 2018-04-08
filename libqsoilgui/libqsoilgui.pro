include( ../config.pri )
TEMPLATE = lib

TARGET = qsoilgui

QT += widgets

DEPENDPATH += \
    ../libqstructutility \
    ../libvarplus \
    ../libvarplusgui \
    ../libqsoil

INCLUDEPATH += \
    $$UI_DIR \
    ../libqstructutility \
    ../libvarplus \
    ../libvarplusgui \
    ../libqsoil

LIBS += \
    -L../bin \
    -lqsoil \
    -lvarplusgui \
    -lvarplus \
    -lqstructutility

# Input
SOURCES += \
    soilmodelgui.cpp \
    soilgui.cpp \
    retainingwallgui.cpp \
    retainingwallmodelgui.cpp \
    retainingwallqscombinationgui.cpp \
    retainingwalleqqscombinationgui.cpp \
    retainingwallguiprivate.cpp \
    soilsheetgui.cpp \
    soilsheetmodelgui.cpp

HEADERS += \
    soilmodelgui.h \
    soilgui.h \
    retainingwallgui.h \
    retainingwallmodelgui.h \
    retainingwallqscombinationgui.h \
    retainingwalleqqscombinationgui.h \
    retainingwallqscombinationguiprivate.h \
    retainingwallguiprivate.h \
    soilsheetgui.h \
    soilsheetmodelgui.h

FORMS += \
    soilmodelgui.ui \
    soilgui.ui \
    retainingwallgui.ui \
    retainingwallmodelgui.ui \
    retainingwallqscombinationgui.ui \
    soilsheetgui.ui \
    soilsheetmodelgui.ui
