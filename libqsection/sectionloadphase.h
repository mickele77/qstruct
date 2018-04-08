#ifndef SECTIONLOADPHASE_H
#define SECTIONLOADPHASE_H

class Point2DPlus;
class DoublePlus;
class SectionLoadPhaseModel;

class SectionLoadPhasePrivate;

#include "section_export.h"
#include "varpluscontainerplus.h"

class EXPORT_SECTION_LIB_OPT SectionLoadPhase: public VarPlusContainerPlus {
public:
    SectionLoadPhase(UnitMeasure *ump, SectionLoadPhaseModel * model, const QString &n, QObject *parent = 0);

    DoublePlus * NEd;
    DoublePlus * MEdY;
    DoublePlus * MEdZ;
    Point2DPlus * NCen;

    DoublePlus * l;
    DoublePlus * my;
    DoublePlus * mz;

    int phaseNum();

private:
    SectionLoadPhasePrivate * m_d;
    void initVar();
};

#endif // SECTIONLOADPHASE_H
