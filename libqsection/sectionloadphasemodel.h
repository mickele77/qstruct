#ifndef SECTIONLOADPHASEMODEL_H
#define SECTIONLOADPHASEMODEL_H

class SectionRCncr;

class SectionLoadPhase;

class SectionLoadPhaseModelPrivate;

#include "section_export.h"
#include "tablemodelplus.h"

class EXPORT_SECTION_LIB_OPT SectionLoadPhaseModel: public TableModelPlus {
public:
    SectionLoadPhaseModel(UnitMeasure *ump, QObject *parent = 0);

    ~SectionLoadPhaseModel();

    void writeXml(QXmlStreamWriter * writer);
    void readXml(QXmlStreamReader * reader);

    void insertRows( int position, int count = 1);
    void removeRows( int position, int count = 1);

    int size();

    void calculate( SectionRCncr * sectRCncr );

    QList<SectionLoadPhase *> loadPhasesList();

    int loadPhaseNumber( SectionLoadPhase *lp );

    SectionLoadPhase * loadPhaseId( unsigned int idInp );

private:
    SectionLoadPhaseModelPrivate * m_dd;

};

#endif // SECTIONLOADPHASEMODEL_H
