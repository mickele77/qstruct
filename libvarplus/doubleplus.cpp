#include "doubleplus.h"

DoublePlus::DoublePlus(double vv, const QString & nn, UnitMeasure * ump, UnitMeasure::unitMeasure um, bool rOnly ):
    VarPlus(nn, ump, um, rOnly),
    m_value( new double(vv) ),
    m_valueWasCreated( true ),
    m_wasMinSet( false ),
    m_wasMaxSet( false ){
}

DoublePlus::DoublePlus( double * vv, const QString & nn, UnitMeasure * ump, UnitMeasure::unitMeasure um, bool em ):
    VarPlus(nn, ump, um, em),
    m_value( vv ),
    m_valueWasCreated( false ),
    m_wasMinSet( false ),
    m_wasMaxSet( false ){
}

DoublePlus::DoublePlus( DoublePlus & vv ):
    VarPlus( vv ){
    m_valueWasCreated = vv.m_valueWasCreated;
    if( m_valueWasCreated ){
        m_value = new double ( *( vv.m_value ) );
    } else {
        m_value = vv.m_value;
    }
    if( vv.m_wasMaxSet ){
        setMaxNormal( vv.m_max );
    }
    if( vv.m_wasMinSet ){
        setMinNormal( vv.m_min );
    }
}

DoublePlus::~DoublePlus(){
    if( m_valueWasCreated ){
        delete m_value;
    }
}

QString DoublePlus::typeVar(){
    return QString("Double");
}

DoublePlus & DoublePlus::operator=( DoublePlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( *(val.m_value) );
        if( val.m_wasMaxSet ){
            setMaxNormal( val.m_max );
        }
        if( val.m_wasMinSet ){
            setMinNormal( val.m_min );
        }
    }
    return *this;
}

VarPlus & DoublePlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        DoublePlus *valDouble = dynamic_cast<DoublePlus*>(&val);
        if( valDouble ){
            setValueNormal( *(valDouble->m_value) );
            if( valDouble->m_wasMaxSet ){
                setMaxNormal( valDouble->m_max );
            }
            if( valDouble->m_wasMinSet ){
                setMinNormal( valDouble->m_min );
            }
        }
    }
    return *this;
}

DoublePlus & DoublePlus::operator=( double val ){
    setValueNormal( val );
    return *this;
}

DoublePlus & DoublePlus::operator+( DoublePlus & val){
    setValueNormal( *m_value + *(val.m_value) );
    return *this;
}

DoublePlus & DoublePlus::operator+( double val){
    setValueNormal( *m_value + val );
    return *this;
}

DoublePlus & DoublePlus::operator-( DoublePlus & val){
    setValueNormal( *m_value - *(val.m_value) );
    return *this;
}

DoublePlus & DoublePlus::operator-( double val){
    setValueNormal( *m_value - val );
    return *this;
}

DoublePlus & DoublePlus::operator*( double l ){
    setValueNormal( *m_value * l );
    return *this;
}

bool DoublePlus::operator==( DoublePlus & val){
    return *m_value == *(val.m_value);
}

double DoublePlus::valueNormal(){
    return *m_value;
}

double DoublePlus::value(){
    return *m_value * m_unitMeasurePointer->scale( m_unitMeasureValue );
}

QString DoublePlus::valueNormalStr() {
    return QString::number( valueNormal() );
}

QString DoublePlus::valueStr() {
    return QString::number( value() );
}

void DoublePlus::setValueNormal( double vv, bool emitAuto ){
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
    if( vv != *m_value ){
        *m_value = vv;
        emit valueChangedInternal( valueStr() );
        if( emitAuto  && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
        }
    }
}

void DoublePlus::setValue( double vv, bool emitAuto ){
    setValueNormal( vv / m_unitMeasurePointer->scale(m_unitMeasureValue ), emitAuto);
}

void DoublePlus::setValueNormal(const QString & vvStr , bool emitAuto){
    setValueNormal( vvStr.toDouble(), emitAuto );
}

void DoublePlus::setValue( const QString & vvStr, bool emitAuto ){
    setValue( vvStr.toDouble(), emitAuto );
}

void DoublePlus::setMinNormal( double val ){
    m_min = val;
    m_wasMinSet = true;
    if( *m_value < m_min ){
        setValueNormal( val );
    }
}

void DoublePlus::resetMin(){
    m_wasMinSet = false;
}

void DoublePlus::setMaxNormal( double val ){
    m_max = val;
    m_wasMaxSet = true;
    if( *m_value > m_max ){
        setValueNormal( val );
    }
}

void DoublePlus::resetMax(){
    m_wasMaxSet = false;
}
