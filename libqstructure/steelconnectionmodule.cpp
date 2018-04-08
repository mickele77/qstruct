#include "steelconnectionmodule.h"

#include <QIcon>

SteelConnectionModule::SteelConnectionModule(UnitMeasure *um, QWidget *parent):
StructModule(um, parent){
}


QString SteelConnectionModule::name(){
    return QString("SteelConnection");
}

QString SteelConnectionModule::richName(){
    return trUtf8("CollegaAcciaio");
}

QIcon SteelConnectionModule::icon(){
    return QIcon(":/images/steelconnection.svg");
}
