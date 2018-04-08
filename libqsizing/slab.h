#ifndef SLAB_H
#define SLAB_H

#include "sectionarea.h"

class Pillar;
template <typename T> class QVector;

class Slab : public SectionArea
{
    Q_OBJECT
private:
    QVector<Pillar *> * m_pillars;
public:
    Slab( UnitMeasure *, QObject * = 0 );
};

#endif // SLAB_H
