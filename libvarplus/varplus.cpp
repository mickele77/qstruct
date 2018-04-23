#include "varplus.h"

#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QString>
#include <QList>
#include <QXmlStreamAttribute>

#include "unitmeasure.h"
#include <QPersistentModelIndex>

VarPlus::VarPlus( const QString & nn,
                  UnitMeasure * ump,
                  UnitMeasure::unitMeasure um,
                  bool rOnly):
    QObject(),
    m_name(new QString(nn)),
    m_enabled( true ),
    m_richName(new QString(nn)),
    m_toolTip(new QString()),
    m_unitMeasurePointer(ump),
    m_unitMeasureValue(um),
    m_readOnly(rOnly),
    m_signalsSuspended(false),
    m_container(NULL),
    m_updateValueMethod(NULL){
    if( m_unitMeasurePointer ){
        connect( m_unitMeasurePointer, &UnitMeasure::richStringsChanged, this, &VarPlus::emitUnitMeasureRichStringChanged );
        connect( m_unitMeasurePointer, &UnitMeasure::stringsChanged, this, &VarPlus::emitUnitMeasureStringChanged );
    }
}

VarPlus::VarPlus( VarPlus & val ):
    QObject(),
    m_name(new QString( * (val.m_name) )),
    m_richName(new QString( *(val.m_richName) )),
    m_toolTip(new QString( *(val.m_toolTip) )),
    m_unitMeasurePointer( val.m_unitMeasurePointer ),
    m_unitMeasureValue( val.m_unitMeasureValue ),
    m_readOnly( val.m_readOnly ),
    m_signalsSuspended( val.m_signalsSuspended ),
    m_connectedTableModels(val.m_connectedTableModels),
    m_connectedVarContainers(val.m_connectedVarContainers),
    m_connectedVars(val.m_connectedVars),
    m_container(val.m_container),
    m_updateValueMethod(val.m_updateValueMethod){
}

VarPlus::~VarPlus(){
    emit aboutToBeDeleted( this );
}

void VarPlus::suspendUpdate(bool susp) {
    for( QList<VarPlus *>::iterator var = m_connectedVars.begin(); var != m_connectedVars.end(); ++var ) {
        if( susp ){
            disconnect( *var, &VarPlus::valueChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
        } else {
            connect( *var, &VarPlus::valueChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
        }
    }
}

VarPlus & VarPlus::operator=( VarPlus & val){
    if( this != &val ){
        setName( *(val.m_name) );
        setRichName(*(val.m_richName) );
        setToolTip( *m_toolTip = *(val.m_toolTip) );
        setUnitMeasure( val.m_unitMeasureValue );
        setReadOnly( val.m_readOnly );
        m_container = val.m_container;
        m_updateValueMethod = m_updateValueMethod;
        m_connectedTableModels = val.m_connectedTableModels;
        m_connectedVarContainers = val.m_connectedVarContainers;
        m_connectedVars = val.m_connectedVars;
    }
    return *this;
}

QString VarPlus::name(){
    return *m_name;
}

bool VarPlus::readOnly(){
    return m_readOnly;
}

bool VarPlus::enabled() {
    return m_enabled;
}

UnitMeasure::unitMeasure VarPlus::unitMeasureValue(){
    return m_unitMeasureValue;
}

UnitMeasure * VarPlus::unitMeasurePointer(){
    return m_unitMeasurePointer;
}

QString VarPlus::richName(){
    return *m_richName;
}

QString VarPlus::toolTip(){
    return *m_toolTip;
}

void VarPlus::setName( const QString & nn ){
    if( nn != *m_name ){
        delete m_name;
        m_name = new QString( nn );
        emit nameChanged( *m_name );
    }
}

void VarPlus::setReadOnly(bool f , bool emitAuto){
    if( m_readOnly != f ){
        m_readOnly = f;
        if( emitAuto ){
            emit readOnlyChanged( m_readOnly );
        }
    }
}

void VarPlus::setReadOnlyWithEmit(bool f) {
    setReadOnly( f, true );
}

void VarPlus::setReadOnlyInv(bool f) {
    setReadOnly( !f );
}

void VarPlus::setRichName( const QString & nn ){
    if( nn != *m_richName ){
        delete m_richName;
        m_richName = new QString( nn );
        emit richNameChanged( *m_richName );
    }
}

void VarPlus::setToolTip( const QString & nn ){
    if( nn != *m_toolTip ){
        *m_toolTip = nn;
        emit toolTipChanged( *m_toolTip );
    }
}

void VarPlus::setUnitMeasure(UnitMeasure::unitMeasure uu){
    if( uu != m_unitMeasureValue ){
        m_unitMeasureValue = uu;
        emit unitMeasureChanged( m_unitMeasureValue );
        emitUnitMeasureRichStringChanged( m_unitMeasureValue );
        emitUnitMeasureStringChanged( m_unitMeasureValue );
    }
}

void VarPlus::setValueXml(const QString &xmlVal, bool emitAuto) {
    if( xmlVal.toUpper() == "AUTO"){
        setReadOnly( true, emitAuto );
    } else {
        setReadOnly( false, emitAuto );
        setValueNormal( xmlVal, emitAuto );
    }
}

void VarPlus::setEnabled(bool v) {
    if( v != m_enabled ){
        m_enabled = v;
        emit enabledChanged(v);
    }
}

QXmlStreamAttribute VarPlus::xmlAttribute() {
    QString val;
    if( m_readOnly ){
        val = "auto";
    } else {
        val = valueNormalStr();
    }
    return QXmlStreamAttribute( name(), val );
}

void VarPlus::emitUnitMeasureStringChanged(UnitMeasure::unitMeasure u) {
    if( u == m_unitMeasureValue ){
        QString ret;
        if( m_unitMeasurePointer ){
            ret = m_unitMeasurePointer->string( m_unitMeasureValue );
        }
        emit unitMeasureStringChanged( ret );
    }
}

void VarPlus::emitUnitMeasureRichStringChanged( UnitMeasure::unitMeasure u ) {
    if( u == m_unitMeasureValue ){
        QString ret;
        if( m_unitMeasurePointer ){
            ret = m_unitMeasurePointer->richString( m_unitMeasureValue );
        }
        emit unitMeasureRichStringChanged( ret );
    }
}

void VarPlus::deleteVarPlus( VarPlus* vv ){
    delete vv;
}

void VarPlus::addConnectedTableModel(TableModelPlus *tModel) {
    if( !m_connectedTableModels.contains( tModel )) {
        connect( tModel, &TableModelPlus::modelChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
        m_connectedTableModels.append( tModel );
    }
}

void VarPlus::removeConnectedTableModel(TableModelPlus *tModel) {
    if( m_connectedTableModels.contains( tModel ) ){
        m_connectedTableModels.removeAll( tModel );
        disconnect( tModel, &TableModelPlus::modelChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
    }
}

void VarPlus::clearConnectedTableModels() {
    for( QList<TableModelPlus *>::iterator tModel = m_connectedTableModels.begin(); tModel != m_connectedTableModels.end(); ++tModel ){
        disconnect( *tModel, &TableModelPlus::modelChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
    }
    m_connectedTableModels.clear();
}

void VarPlus::addConnectedVarContainer(VarPlusContainerPlus *varCont, bool emitAuto) {
    if( !m_connectedVarContainers.contains( varCont )) {
        connect( varCont, &VarPlusContainerPlus::containerChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
        m_connectedVarContainers.append( varCont );
        updateValue( emitAuto );
    }
}

void VarPlus::removeConnectedVarContainer(VarPlusContainerPlus *varCont, bool emitAuto) {
    if( m_connectedVarContainers.contains( varCont ) ){
        m_connectedVarContainers.removeAll( varCont );
        disconnect( varCont, &VarPlusContainerPlus::containerChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
        updateValue( emitAuto );
    }
}

void VarPlus::clearConnectedVarContainers() {
    for( QList<VarPlusContainerPlus *>::iterator varCont = m_connectedVarContainers.begin(); varCont != m_connectedVarContainers.end(); ++varCont ){
        disconnect( *varCont, &VarPlusContainerPlus::containerChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
    }
    m_connectedVarContainers.clear();
}

#include <cstdarg>

void VarPlus::addConnectedVar( VarPlus * var ) {
    if( !m_connectedVars.contains( var )) {
        connect( var, &VarPlus::valueChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
        m_connectedVars.append( var );
    }
}

void VarPlus::addConnectedVars( int nVars, ...) {
    va_list vars;
    va_start(vars, nVars);
    for(int i = 1; i <= nVars; i++) {
        VarPlus * var = va_arg(vars, VarPlus *);
        connect( var, &VarPlus::valueChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
        m_connectedVars.append( var );
    }
    va_end(vars);
}

void VarPlus::removeConnectedVar(VarPlus *var) {
    if( m_connectedVars.contains( var ) ){
        m_connectedVars.removeAll( var );
        disconnect( var, &VarPlus::valueChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
    }
}

void VarPlus::clearConnectedVars() {
    for( QList<VarPlus *>::iterator var = m_connectedVars.begin(); var != m_connectedVars.end(); ++var ){
        disconnect( *var, &VarPlus::valueChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
    }
    m_connectedVars.clear();
}

QList<VarPlus *> VarPlus::connectedVars() {
    return m_connectedVars;
}

void VarPlus::updateConnectedVarsValue(QList<VarPlus *> * updatedVars) {
    if( m_readOnly ){
        if( !updatedVars->contains( this ) ){
            for( QList<VarPlus *>::iterator connVar = m_connectedVars.begin(); connVar != m_connectedVars.end(); ++connVar ){
                (*connVar)->updateConnectedVarsValue( updatedVars );
            }
            updateValue( false );
            updatedVars->append( this );
        }
    }
}

void VarPlus::setUpdateValueMethod(VarPlusContainer * cont, void (VarPlusContainer::*method)(bool emitAuto) ) {
    m_container = cont;
    m_updateValueMethod = method;
    connect( this, &VarPlus::readOnlyChanged, this, static_cast<void(VarPlus::*)(void)>(&VarPlus::updateValue) );
}

void VarPlus::updateValue(bool emitAuto) {
    if( m_readOnly ){
        if( m_container != NULL ){
            if( m_updateValueMethod != NULL ){
                (m_container->*m_updateValueMethod)( emitAuto );
            }
        }
    }
}

void VarPlus::suspendSignals(bool susp) {
    m_signalsSuspended = susp;
}

void VarPlus::storeValue() {
    m_storedValue = valueStr();
}

void VarPlus::checkStoredValue() {
    if( m_storedValue != valueStr() ){
        emit valueChanged( valueStr() );
    }
}

void VarPlus::updateValue() {
    updateValue( true );
}
