include( ../config.pri )

TEMPLATE = lib

TARGET = qsection

QT += widgets

contains(DEFINES, BUILD_STATIC) {
  CONFIG += staticlib
}

win32 {
    contains(DEFINES, BUILD_SHARED_WIN) {
      DEFINES += BUILD_SECTION_LIB
    }
}

DEPENDPATH += \
    ../libqstructutility \
    ../libvarplus \
    ../libqload \
    ../libqmaterial

# E' necessario inserire nella variabile INCLUDEPATH la directory "." altrimenti
# gcc da' problemi in fase di compilazione con il file sectiontimber.cpp che
# richiama il file timber.h cha a sua volta richiama il file sectiontimber.h
INCLUDEPATH += . \
    ../libqstructutility \
    ../libvarplus \
    ../libqload \
    ../libqmaterial

LIBS += \
    -L../bin \
    -lqmaterial \
    -lqload \
    -lvarplus \
    -lqstructutility

# Input
SOURCES += section.cpp \
    sectionpoints.cpp \
    sectionsteelcncr.cpp \
    sectioncncr.cpp \
    sectionrcncr.cpp \
    sectionsteel.cpp \
    sectiontimber.cpp \
    sectionsteeldatasetmodel.cpp \
    sectiontimberdatasetmodel.cpp \
    sectionmodel.cpp \
    sectionline.cpp \
    sectionarc.cpp \
    sectionthinsimple.cpp \
    sectionthin.cpp \
    sectionsteeli.cpp \
    sectionsteelcirc.cpp \
    sectionsteelomega.cpp \
    sectionsteelrect.cpp \
    sectionsteelvar.cpp \
    sectionsteelprofile.cpp \
    sectiontimbercncr.cpp \
    multisectionarcmodel.cpp \
    multisection.cpp \
    multisectioncncrmodel.cpp \
    multisectionlinemodel.cpp \
    multisectionsteelcncrmodel.cpp \
    sectionthincont.cpp \
    sectionthincontpointsmodel.cpp \
    sectionprofile.cpp \
    multisectionmodelbase.cpp \
    sectionsteell.cpp \
    multisectionsteelmodel.cpp \
    sectionthincontsteel.cpp \
    sectionthincontprivate.cpp \
    sectionfrp.cpp \
    multisectionfrpmodel.cpp \
    sectionloadphase.cpp \
    sectionloadphasemodel.cpp

HEADERS += \
    section.h \
    sectionpoints.h \
    sectionsteelcncr.h \
    sectioncncr.h \
    sectionrcncr.h \
    sectionsteel.h \
    sectiontimber.h \
    sectiontimberdatasetmodel.h  \
    sectionsteeldatasetmodel.h  \
    sectionmodel.h \
    sectionline.h \
    sectionarc.h \
    sectionthinsimple.h \
    sectionthin.h \
    sectionsteeli.h \
    sectionsteelcirc.h \
    sectionsteelomega.h \
    sectionsteelrect.h \
    sectionsteelvar.h \
    sectionsteelprofile.h \
    sectiontimbercncr.h \
    multisectionarcmodel.h \
    multisectioncncrmodel.h \
    multisectionlinemodel.h \
    multisectionsteelcncrmodel.h \
    multisection.h \
    sectionthincont.h \
    sectionthincontpointsmodel.h \
    sectionthinprivate.h \
    sectionprofile.h \
    section_export.h \
    multisectionmodelbase.h \
    section_def.h \
    sectionsteell.h \
    multisectionsteelmodel.h \
    sectionthincontsteel.h \
    sectionthincontprivate.h \
    sectionfrp.h \
    multisectionfrpmodel.h \
    sectionloadphase.h \
    sectionloadphasemodel.h
