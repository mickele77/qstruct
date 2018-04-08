#include "enumplus.h"

EnumPlus::EnumPlus( const QString & nn, bool rOnly) :
    VarPlus(nn, 0, UnitMeasure::noDimension, rOnly) {
}

EnumPlus::EnumPlus( EnumPlus & v ):
    VarPlus(v){
}

int EnumPlus::valueInt(){
    return valueNormalInt();
}
