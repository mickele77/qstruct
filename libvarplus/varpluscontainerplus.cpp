#include "varpluscontainerplus.h"

#include "varplus.h"

VarPlusContainerPlus::VarPlusContainerPlus(UnitMeasure * ump, const QString &t, const QString &n , QObject *parent):
    QObject( parent ),
    VarPlusContainer( ump, t, n ){
}

VarPlusContainerPlus::~VarPlusContainerPlus(){
    emit aboutToBeDeleted();
}

void VarPlusContainerPlus::addVarToContainer( VarPlus * var ){
    VarPlusContainer::addVarToContainer( var );
    connect( var, &VarPlus::valueChanged, this, &VarPlusContainerPlus::containerChanged );
    connect( var, &VarPlus::readOnlyChanged, this, &VarPlusContainerPlus::containerChanged );
}

void VarPlusContainerPlus::updateReadOnlyVars() {
    VarPlusContainer::updateReadOnlyVars();
}
