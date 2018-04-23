#include "intplus.h"

IntPlus::IntPlus( int vv,
                  const QString & nn,
                  bool em ):
    VarPlus(nn, 0, UnitMeasure::noDimension, em),
    m_value( vv ) ,
    m_wasMinSet( false ),
    m_wasMaxSet( false ){
}

IntPlus::IntPlus( IntPlus & vv ):
    VarPlus( vv ){
    m_value = vv.m_value;
    m_wasMinSet = vv.m_wasMinSet;
    m_wasMaxSet = vv.m_wasMaxSet;
}

IntPlus::~IntPlus(){
}

IntPlus & IntPlus::operator=( IntPlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_value );
        if( val.m_wasMaxSet ){
            setMaxNormal( val.m_max );
        }
        if( val.m_wasMinSet ){
            setMinNormal( val.m_min );
        }
    }
    return *this;
}

VarPlus & IntPlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        IntPlus *valInt = dynamic_cast<IntPlus*>(&val);
        if(valInt) {
            setValueNormal( valInt->m_value );
            if( valInt->m_wasMaxSet ){
                setMaxNormal( valInt->m_max );
            }
            if( valInt->m_wasMinSet ){
                setMinNormal( valInt->m_min );
            }
        }
    }
    return *this;
}

QString IntPlus::typeVar(){
    return QString("Int");
}

unsigned int IntPlus::valueNormal(){
    return m_value;
}

unsigned int IntPlus::value(){
    return m_value;
}

QString IntPlus::valueNormalStr() {
    return QString::number( valueNormal() );
}

QString IntPlus::valueStr() {
    return QString::number( value() );
}

void IntPlus::setValueNormal( int vv, bool emitAuto){
    if( m_wasMinSet ){
        if( vv < m_min){
            if( emitAuto  && !m_signalsSuspended ){
                emit valueChanged( valueStr() );
            }
            return;
        }
    }
    if( m_wasMaxSet ){
        if( vv > m_max){
            if( emitAuto  && !m_signalsSuspended ){
                emit valueChanged( valueStr() );
            }
            return;
        }
    }
    if( vv != m_value ){
        m_value = vv;
        emit valueChangedInternal( valueStr() );
        if( emitAuto  && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
        }
    }
}

void IntPlus::setValueNormal( const QString & vvStr, bool emitAuto){
    setValueNormal(vvStr.toInt(), emitAuto );
}

void IntPlus::setValue( int vv, bool emitAuto){
    setValueNormal( vv, emitAuto );
}

void IntPlus::setValue( const QString & vvStr, bool emitAuto){
    setValue( vvStr.toInt(), emitAuto );
}

void IntPlus::setMinNormal( int val ){
    m_min = val;
    m_wasMinSet = true;
    if( m_value < m_min ){
        setValueNormal( val );
    }
}

void IntPlus::resetMin(){
    m_wasMinSet = false;
}

void IntPlus::setMaxNormal(int val ){
    m_max = val;
    m_wasMaxSet = true;
    if( m_value > m_max ){
        setValueNormal( val );
    }
}

void IntPlus::resetMax(){
    m_wasMaxSet = false;
}
