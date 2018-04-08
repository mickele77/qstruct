#include "uintplus.h"

UIntPlus::UIntPlus( unsigned int vv,
                    const QString & nn,
                    bool rOnly):
    VarPlus(nn, NULL, UnitMeasure::noDimension, rOnly),
    m_value( vv )
{
}

UIntPlus::UIntPlus( UIntPlus & vv ):
    VarPlus( vv ){
    m_value = vv.m_value;
}

UIntPlus::~UIntPlus(){
}

UIntPlus & UIntPlus::operator=( UIntPlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_value );
    }
    return *this;
}

VarPlus & UIntPlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        UIntPlus *valUInt = dynamic_cast<UIntPlus*>(&val);
        if(valUInt) {
            setValueNormal( valUInt->m_value );
        }
    }
    return *this;
}

QString UIntPlus::typeVar(){
    return QString("UInt");
}

unsigned int UIntPlus::valueNormal(){
    return m_value;
}

unsigned int UIntPlus::value(){
    return m_value;
}

QString UIntPlus::valueNormalStr() {
    return QString::number( valueNormal() );
}

QString UIntPlus::valueStr() {
    return QString::number( value() );
}

void UIntPlus::setValueNormal( unsigned int vv, bool emitAuto ){
    if( vv != m_value ){
        m_value = vv;
        emit valueChangedInternal( valueStr() );
        if( emitAuto && !m_suspendSignals ){
            emit valueChanged( valueStr() );
        }
    }
}

void UIntPlus::setValue( unsigned int vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void UIntPlus::setValueNormal( const QString & vvStr, bool emitAuto ){
    setValueNormal(vvStr.toDouble(), emitAuto );
}

void UIntPlus::setValue( const QString & vvStr, bool emitAuto ){
    setValue(vvStr.toDouble(), emitAuto );
}
