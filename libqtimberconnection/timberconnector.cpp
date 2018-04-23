/*
            Copyright (C) 2014-2017 Michele Mocciola

            This file is part of QStruct.

            QStruct is free software: you can redistribute it and/or modify
            it under the terms of the GNU General Public License as published by
            the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.

            QStruct is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
            GNU General Public License for more details.

            You should have received a copy of the GNU General Public License
            along with QStruct.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "timberconnector.h"

#include "doubleplus.h"
#include "varplus.h"
#include "unitmeasure.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlAttributes>
#include <QString>

#include <cmath>

TimberConnector::TimberConnector( UnitMeasure * ump, double ffyk, double ffuk, double dd, QObject * parent ):
    QObject( parent ),
    VarPlusContainer(ump, "TimberConnector", ""){
    initVar( ffyk, ffuk, dd );
}

void TimberConnector::initVar( double ffyk, double ffuk, double dd ){
    connectorType = new TimberConnectorType( TimberConnector::Screw, "connectorType");
    connectorType->setRichName( trUtf8("Tipologia connettore"));
    addVarToContainer( connectorType );

    predrilledHole = new TimberPredrilledHole( TimberConnector::WithPredrilledHole, "predrilledHole");
    predrilledHole->setRichName( trUtf8("Con/senza preforo"));
    addVarToContainer( predrilledHole );

    fyk = new DoublePlus( ffyk, "fyk", m_unitMeasure, UnitMeasure::tension );
    fyk->setReadOnly( false );
    fyk->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">yk</span>"));
    fyk->setToolTip( trUtf8("Tensione caratteristica di snervamento del connettore"));
    addVarToContainer( fyk );

    fuk = new DoublePlus( ffuk, "fuk", m_unitMeasure, UnitMeasure::tension );
    fuk->setReadOnly( false );
    fuk->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">uk</span>"));
    fuk->setToolTip( trUtf8("Tensione caratteristica di rottura del connettore"));
    addVarToContainer( fuk );

    d = new DoublePlus( dd, "d", m_unitMeasure, UnitMeasure::sectL );
    d->setReadOnly( false );
    d->setRichName( trUtf8("d"));
    d->setToolTip( trUtf8("Diametro del connettore"));
    addVarToContainer( d );

    dEf = new DoublePlus( 0.0, "dEf", m_unitMeasure, UnitMeasure::sectL );
    dEf->setReadOnly( true );
    dEf->setRichName( trUtf8("d<span style=\" vertical-align:sub;\">ef</span>"));
    dEf->setToolTip( trUtf8("Diametro effettivo della vite"));
    addVarToContainer( dEf );
    connect( d, SIGNAL(valueChanged(QString)), this, SLOT(setDEf()));
    setDEf();

    dHead = new DoublePlus( dd, "dHead", m_unitMeasure, UnitMeasure::sectL );
    dHead->setReadOnly( false );
    dHead->setRichName( trUtf8("d<span style=\" vertical-align:sub;\">head</span>"));
    dHead->setToolTip( trUtf8("Per viti, diametro della rondella, per chiodi diametro della testa del chiodo"));
    addVarToContainer( dHead );

    lEf = new DoublePlus( dd, "lEf", m_unitMeasure, UnitMeasure::sectL );
    lEf->setReadOnly( false );
    lEf->setRichName( trUtf8("l<span style=\" vertical-align:sub;\">ef</span>"));
    lEf->setToolTip( trUtf8("Per viti, lunghezza della porzione filettata meno un diametro"));
    addVarToContainer( lEf );

    alpha = new DoublePlus( M_PI / 2.0, "alpha", m_unitMeasure, UnitMeasure::angle );
    alpha->setReadOnly( false );
    alpha->setRichName( trUtf8("α"));
    alpha->setToolTip( trUtf8("Inclinazione del connettore rispetto alle fibre di legno"));
    addVarToContainer( alpha );

    MyRk = new DoublePlus( 0.0, "MyRk", m_unitMeasure, UnitMeasure::loadM );
    MyRk->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">y,Rk</span>"));
    MyRk->setReadOnly( true );
    addVarToContainer( MyRk );
    connect( connectorType, &TimberConnectorType::valueChanged, this, &TimberConnector::setMyRk );
    connect( MyRk, SIGNAL(readOnlyChanged(bool)), this, SLOT(setMyRk()));
    connect( d, SIGNAL(valueChanged(QString)), this, SLOT(setMyRk()));
    connect( fuk, SIGNAL(valueChanged(QString)), this, SLOT(setMyRk()));
    setMyRk();

    connect( connectorType, SIGNAL(valueChanged(QString)), this, SLOT(setVisibleVar()));
    setVisibleVar();
}

void TimberConnector::setMyRk(){
    if( MyRk->readOnly() ){
        if( connectorType->valueNormal() == TimberConnector::SmoothSquareNail ||
                connectorType->valueNormal() == TimberConnector::OtherSquareNail ){
            MyRk->setValueNormal( 1.0e-3 * 0.45 * (fuk->valueNormal() * 1.0e-6) * pow( (d->valueNormal()*1.0e+3), 2.6 ) );
        } else {
            double diam = d->valueNormal();
            if( connectorType->valueNormal() == TimberConnector::Screw ){
                diam = dEf->valueNormal();
            }
            MyRk->setValueNormal( 1.0e-3 * 0.30 * (fuk->valueNormal() * 1.0e-6) * pow( diam * 1.0e+3, 2.6 ) );
        }
    }
}

void TimberConnector::setDEf(){
    if( dEf->readOnly() ){
        dEf->setValueNormal( d->valueNormal() * 0.90 );
    }
}

void TimberConnector::setVisibleVar(){
    if( connectorType->valueNormal() == TimberConnector::SmoothRoundNail || \
            connectorType->valueNormal() == TimberConnector::SmoothSquareNail || \
            connectorType->valueNormal() == TimberConnector::OtherRoundNail || \
            connectorType->valueNormal() == TimberConnector::OtherSquareNail ){
        dHead->setEnabled( true );
        dHead->setRichName( trUtf8("d<span style=\" vertical-align:sub;\">h</span>"));
        dHead->setToolTip( trUtf8("Diametro della testa del chiodo"));
    } else if( connectorType->valueNormal() == TimberConnector::Bolt || \
               connectorType->valueNormal() == TimberConnector::Screw ){
        dHead->setEnabled( true );
        dHead->setRichName( trUtf8("d<span style=\" vertical-align:sub;\">washer</span>"));
        dHead->setToolTip( trUtf8("Diametro della rondella"));
    } else {
        dHead->setEnabled( false );
    }
    if( connectorType->valueNormal() == TimberConnector::Screw ){
        dEf->setEnabled( true );
        lEf->setEnabled( true );
    } else {
        dEf->setEnabled( false );
        lEf->setEnabled( false );
    }
}

#include <QCoreApplication>

class TimberConnectorTypePrivate{
    Q_DECLARE_TR_FUNCTIONS(TimberConnectorTypePrivate)

    class enumVal{
    public:
        enumVal(TimberConnector::ConnectorType v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        TimberConnector::ConnectorType val;
        QString normalStr;
        QString str;
    };

public:
    TimberConnectorTypePrivate(TimberConnector::ConnectorType v):
        value(v){
        enumList.append( enumVal( TimberConnector::Bolt, "Bolt", trUtf8("Bullone/Spinotto")) );
        enumList.append( enumVal( TimberConnector::Screw, "Screw", trUtf8("Vite")) );
        enumList.append( enumVal( TimberConnector::SmoothRoundNail, "SmoothRoundNail", trUtf8("Chiodo a gambo liscio, rotondo")) );
        enumList.append( enumVal( TimberConnector::SmoothSquareNail, "SmoothSquareNail", trUtf8("Chiodo a gambo liscio, quadrato")) );
        enumList.append( enumVal( TimberConnector::OtherRoundNail, "OtherRoundNail", trUtf8("Chiodo non a gambo liscio, rotondo")) );
        enumList.append( enumVal( TimberConnector::OtherSquareNail, "OtherSquareNail", trUtf8("Chiodo non a gambo liscio, quadrato")) );
    };
    ~TimberConnectorTypePrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    TimberConnector::ConnectorType value;
    QList<enumVal> enumList;
};

TimberConnectorType::TimberConnectorType( TimberConnector::ConnectorType tt,
                                          const QString & nn,
                                          bool ro):
    EnumPlus( nn, ro),
    m_d( new TimberConnectorTypePrivate(tt) ){
}

TimberConnectorType::~TimberConnectorType(){
    delete m_d;
}

TimberConnectorType & TimberConnectorType::operator=( TimberConnectorType & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & TimberConnectorType::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        TimberConnectorType * valCncrModelPlus = dynamic_cast<TimberConnectorType *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString TimberConnectorType::typeVar(){
    return QString("TimberConnectorType");
}

TimberConnector::ConnectorType TimberConnectorType::value(){
    return m_d->value;
}

TimberConnector::ConnectorType TimberConnectorType::valueNormal(){
    return m_d->value;
}

QString TimberConnectorType::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString TimberConnectorType::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void TimberConnectorType::setValueNormal( TimberConnector::ConnectorType vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void TimberConnectorType::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void TimberConnectorType::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void TimberConnectorType::setValue( TimberConnector::ConnectorType vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void TimberConnectorType::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int TimberConnectorType::enumCount() {
    return m_d->enumList.size();
}

QString TimberConnectorType::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int TimberConnectorType::valueNormalInt() {
    return m_d->valueIndex();
}

class TimberPredrilledHolePrivate{
    Q_DECLARE_TR_FUNCTIONS(TimberPredrilledHolePrivate)

    class enumVal{
    public:
        enumVal( TimberConnector::PredrilledHole v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        TimberConnector::PredrilledHole val;
        QString normalStr;
        QString str;
    };

public:
    TimberPredrilledHolePrivate( TimberConnector::PredrilledHole v):
        value(v){
        enumList.append( enumVal( TimberConnector::WithPredrilledHole, "WithPredrilledHole", trUtf8("Con preforo")) );
        enumList.append( enumVal( TimberConnector::WithoutPredrilledHole, "WithoutPredrilledHole", trUtf8("Senza preforo")) );
    };
    ~TimberPredrilledHolePrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    TimberConnector::PredrilledHole value;
    QList<enumVal> enumList;
};

TimberPredrilledHole::TimberPredrilledHole( TimberConnector::PredrilledHole tt,
                                            const QString & nn,
                                            bool ro):
    EnumPlus( nn, ro),
    m_d( new TimberPredrilledHolePrivate(tt) ){
}

TimberPredrilledHole::~TimberPredrilledHole(){
    delete m_d;
}

TimberPredrilledHole & TimberPredrilledHole::operator=( TimberPredrilledHole & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & TimberPredrilledHole::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        TimberPredrilledHole * valCncrModelPlus = dynamic_cast<TimberPredrilledHole *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString TimberPredrilledHole::typeVar(){
    return QString("TimberPredrilledHole");
}

TimberConnector::PredrilledHole TimberPredrilledHole::value(){
    return m_d->value;
}

TimberConnector::PredrilledHole TimberPredrilledHole::valueNormal(){
    return m_d->value;
}

QString TimberPredrilledHole::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString TimberPredrilledHole::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void TimberPredrilledHole::setValueNormal(  TimberConnector::PredrilledHole vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void TimberPredrilledHole::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void TimberPredrilledHole::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void TimberPredrilledHole::setValue(  TimberConnector::PredrilledHole vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void TimberPredrilledHole::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int TimberPredrilledHole::enumCount() {
    return m_d->enumList.size();
}

QString TimberPredrilledHole::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int TimberPredrilledHole::valueNormalInt() {
    return m_d->valueIndex();
}
