#ifndef RECTPILLAR_H
#define RECTPILLAR_H

#include "pillar.h"

class RectPillar : public Pillar
{
private:
    double m_L1;
    double m_L2;

public:
    RectPillar( UnitMeasure * );

    void setL1( double );
    void setL2( double );
};

#endif // RECTPILLAR_H
