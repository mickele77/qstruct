#include "slab.h"

#include <QVector>

#include "pillar.h"

Slab::Slab( UnitMeasure * um, QObject * parent):
        SectionArea( um, parent ),
        m_pillars( new QVector <Pillar *> ){
}
