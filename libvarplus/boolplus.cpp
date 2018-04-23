#include "boolplus.h"

#include <QCheckBox>

BoolPlus::BoolPlus(bool vv, const QString & nn, bool rOnly ):
    VarPlus(nn, 0, UnitMeasure::noDimension, rOnly),
    m_value(vv){
}

BoolPlus::BoolPlus( BoolPlus & vv ):
    VarPlus( vv ){
}

BoolPlus::~BoolPlus(){
}

QString BoolPlus::typeVar(){
    return QString("Bool");
}

BoolPlus & BoolPlus::operator=( BoolPlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        m_value = val.m_value;
    }
    return *this;
}

VarPlus & BoolPlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        BoolPlus *valDouble = dynamic_cast<BoolPlus*>(&val);
        if( valDouble ) {
            m_value = valDouble->m_value;
        }
    }
    return *this;
}

bool BoolPlus::valueNormal(){
    return m_value;
}

bool BoolPlus::value(){
    return m_value;
}

QString BoolPlus::valueNormalStr() {
    if( m_value )
        return QString("yes");
    else
        return QString("no");
}

QString BoolPlus::valueStr() {
    if( m_value )
        return trUtf8("sì");
    else
        return trUtf8("no");
}

void BoolPlus::setValueNormal( bool vv, bool emitAuto ){
    if( vv != m_value ){
        m_value = vv;
        emit valueChangedInternal( valueStr() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueChangedBool( m_value );
        }
    }
}

void BoolPlus::setValue( bool vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void BoolPlus::setValueNormal( const QString & vvStr, bool emitAuto ){
    if( vvStr.toUpper() == QString("YES"))
        setValueNormal( true, emitAuto );
    if( vvStr.toUpper() == QString("NO"))
        setValueNormal( false, emitAuto );
}

void BoolPlus::setValue( const QString & vvStr, bool emitAuto ){
    if( vvStr.toUpper() == trUtf8("SÌ"))
        setValueNormal( true, emitAuto );
    if( vvStr.toUpper() == trUtf8("NO"))
        setValueNormal( false, emitAuto );
}
