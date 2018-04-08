#include "sectionthincontprivate.h"

#include "sectionthincontpointsmodel.h"

SectionThinContPrivate::SectionThinContPrivate( UnitMeasure * ump ):
    pointsModel( new SectionThinContPointsModel( ump) ){
}

SectionThinContPrivate::~SectionThinContPrivate(){
    delete pointsModel;
}
