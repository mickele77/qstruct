include( ../config.pri )

TARGET = qtimberconnection

TEMPLATE = lib

QT += xml

win32 {
    contains(DEFINES, BUILD_SHARED_WIN) {
      DEFINES += BUILD_TIMBERCONNECTION_LIB
    }
}

DEPENDPATH += \
    ../libqsection \
    ../libqmaterial \
    ../libqload \
    ../libvarplus \
    ../libqstructutility

INCLUDEPATH += \
    ../libqsection \
    ../libqmaterial \
    ../libqload \
    ../libvarplus \
    ../libqstructutility

LIBS += \
    -L../bin \
    -lqsection \
    -lqmaterial \
    -lqload \
    -lvarplus \
    -lqstructutility

SOURCES += \
    timberconnection.cpp \
    timberconnector.cpp \
    timberconnectionmodel.cpp

HEADERS += \
    timberconnection.h \
    timberconnector.h \
    timberconnectionmodel.h \
    timberconnection_export.h
