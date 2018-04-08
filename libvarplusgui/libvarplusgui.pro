include( ../config.pri )

TARGET = varplusgui
TEMPLATE = lib

QT       += gui widgets

DEPENDPATH += \
    ../libvarplus

INCLUDEPATH += \
    ../libvarplus \
    ../libqstructutility

LIBS += \
    -L../bin \
    -lvarplus \
    -lqstructutility

SOURCES += \
    qwidgetplus.cpp \
    qlabelunitmeasure.cpp \
    qlabelrichname.cpp \
    qlineeditvalue.cpp \
    qcheckboxreadonly.cpp \
    qcheckboxbool.cpp \
    varplusguiutility.cpp \
    qwidgetenumplus.cpp \
    qcomboboxenum.cpp \
    qradiobuttonenum.cpp

HEADERS += \
    qwidgetplus.h \
    qlabelunitmeasure.h \
    qlabelrichname.h \
    qlineeditvalue.h \
    qcheckboxreadonly.h \
    qcheckboxbool.h \
    varplusguiutility.h \
    qwidgetenumplus.h \
    qcomboboxenum.h \
    qradiobuttonenum.h

