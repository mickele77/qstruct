#include "pillar.h"

#include <QVector>

#include "sectionarea.h"
#include "unitmeasure.h"
#include "point2d.h"

Pillar::Pillar( UnitMeasure * um ):
        m_P( new Point2D( um->scale(UnitMeasure::coord )) ),
        m_section( new SectionArea(um) ) {
}

Pillar::~Pillar() {
    delete m_P;
    delete m_section;
}

Point2D Pillar::P(){
    return *m_P;
}

void Pillar::setP( const Point2D & np ){
    *m_P = np;
}
