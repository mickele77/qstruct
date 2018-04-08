#include "rectpillar.h"

RectPillar::RectPillar( UnitMeasure * um):
        Pillar(um){
}

void RectPillar::setL1( double v ){
    m_L1 = v;
}

void RectPillar::setL2( double v ){
    m_L2 = v;
}
