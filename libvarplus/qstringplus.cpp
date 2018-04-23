#include "qstringplus.h"

#include "unitmeasure.h"

QStringPlus::QStringPlus(const QString & vv, const QString & nn,  bool rOnly ):
    VarPlus(nn, 0, UnitMeasure::noDimension, rOnly),
    m_value( vv ){
}

QStringPlus::QStringPlus( QStringPlus & vv ):
    VarPlus( vv ){
    m_value = vv.m_value;
}

QStringPlus::~QStringPlus(){

}

QString QStringPlus::typeVar(){
    return QString("QString");
}

QStringPlus & QStringPlus::operator=( QStringPlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_value );
        setReadOnly( val.m_readOnly );
    }
    return *this;
}

VarPlus & QStringPlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        QStringPlus *valQString = dynamic_cast<QStringPlus*>(&val);
        if(valQString) {
            setValueNormal( valQString->m_value );
        }
    }
    return *this;
}

QString QStringPlus::valueNormalStr() {
    return m_value;
}

QString QStringPlus::valueStr() {
    return valueNormalStr();
}

void QStringPlus::setValueNormal( const QString & vvStr, bool emitAuto ){
    if( vvStr != m_value ){
        m_value = vvStr;
        emit valueChangedInternal( m_value );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( m_value );
        }
    }
}

void QStringPlus::setValue( const QString & vvStr, bool emitAuto ){
    setValueNormal( vvStr, emitAuto );
}
