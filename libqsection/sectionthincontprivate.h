#ifndef SECTIONTHINCONTPRIVATE_H
#define SECTIONTHINCONTPRIVATE_H

class SectionThinContPointsModel;
class UnitMeasure;

class SectionThinContPrivate{
public:
    SectionThinContPrivate( UnitMeasure * ump );
    ~SectionThinContPrivate();

    SectionThinContPointsModel * pointsModel;
};

#endif // SECTIONTHINCONTPRIVATE_H
