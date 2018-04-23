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
#include "beamq.h"

#include "hypothesis.h"
#include "doubleplus.h"
#include "uintplus.h"

#define NVERT 2

class BeamQPrivate{
public:
    BeamQPrivate( Hypothesis * hyp, UnitMeasure * ump ){
        nDOF = NVERT * hyp->nDOFVert();
        qVal = new double[ nDOF ];
        for( int i=0;i < nDOF; i++ ){
            qVal[i] = 0.0;
            int j = i % hyp->nDOFVert();
            if( j / hyp->spaceDim() == 0 ){
                q.append( new DoublePlus( &(qVal[i]), "q" + QString::number( j % hyp->spaceDim()), ump, UnitMeasure::loadFLine) );
            } else {
                q.append( new DoublePlus( &(qVal[i]), "q" + QString::number( j % hyp->spaceDim()), ump, UnitMeasure::loadMLine) );
            }
        }
        sysRef = new BeamQSysRef( BeamQ::qLocalSR, "sysRef" );
        sysRefGlobal =  new BeamQSysRefGlobal( 0, hyp, "sysRefGlobal" );
    };
    ~BeamQPrivate(){
        for( int i=0;i < nDOF; i++ ){
            VarPlus::deleteVarPlus( q.at(i));
        }
        q.clear();
        delete [] qVal;
        VarPlus::deleteVarPlus( sysRef );
        VarPlus::deleteVarPlus( sysRefGlobal );
    };

    int nDOF;
    double * qVal;
    QList<DoublePlus *> q;
    BeamQSysRef * sysRef;
    BeamQSysRefGlobal * sysRefGlobal;

    static unsigned int nextId;
};

unsigned int  BeamQPrivate::nextId = 1;

BeamQ::BeamQ( Hypothesis * hyp, UnitMeasure * ump, const QString & n, QObject * parent ):
    QObject( parent ),
    VarPlusContainer( ump, "BeamQ", n ),
    m_d( new BeamQPrivate( hyp, ump)){
}

int BeamQ::nVert(){
    return NVERT;
}

BeamQSysRef * BeamQ::sysRef(){
    return m_d->sysRef;
}

BeamQSysRefGlobal *BeamQ::sysRefGlobal(){
    return m_d->sysRefGlobal;
}

double * BeamQ::qVal() const{
    return m_d->qVal;
}

DoublePlus * BeamQ::q(int i){
    if( i < m_d->q.size() ){
        return m_d->q.at(i);
    }
    return NULL;
}

void BeamQ::nextId( void ){
    id->setValueNormal( m_d->nextId );
    m_d->nextId++;
}

#include <QApplication>

class BeamQSysRefPrivate{
    Q_DECLARE_TR_FUNCTIONS(BeamQSysRefPrivate)

    class enumVal{
    public:
        enumVal(BeamQ::qSysRef v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        BeamQ::qSysRef val;
        QString normalStr;
        QString str;
    };

public:
    BeamQSysRefPrivate(BeamQ::qSysRef v):
        value(v){
        enumList.append( enumVal( BeamQ::qLocalSR, "qLocalSR", trUtf8("Locale")) );
        enumList.append( enumVal( BeamQ::qGlobalLocalSR, "qGlobalLocalSR", trUtf8("Globale/Locale")) );
        enumList.append( enumVal( BeamQ::qGlobalSR, "qGlobalSR", trUtf8("Globale")) );
    };
    ~BeamQSysRefPrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    BeamQ::qSysRef value;
    QList<enumVal> enumList;
};

BeamQSysRef::BeamQSysRef( BeamQ::qSysRef sr,
                          const QString & nn,
                          bool ro):
    EnumPlus( nn, ro),
    m_d( new BeamQSysRefPrivate(sr )){
}

BeamQSysRef::~BeamQSysRef(){
    delete m_d;
}

BeamQSysRef & BeamQSysRef::operator=( BeamQSysRef & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & BeamQSysRef::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        BeamQSysRef * valCncrModelPlus = dynamic_cast<BeamQSysRef *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString BeamQSysRef::typeVar(){
    return QString("BeamQSysRef");
}

BeamQ::qSysRef BeamQSysRef::value(){
    return m_d->value;
}

BeamQ::qSysRef BeamQSysRef::valueNormal(){
    return m_d->value;
}

QString BeamQSysRef::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString BeamQSysRef::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void BeamQSysRef::setValueNormal( BeamQ::qSysRef vv, bool emitAuto ){
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

void BeamQSysRef::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void BeamQSysRef::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void BeamQSysRef::setValue( BeamQ::qSysRef vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void BeamQSysRef::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int BeamQSysRef::enumCount() {
    return m_d->enumList.size();
}

QString BeamQSysRef::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int BeamQSysRef::enumCountStatic() {
    return 3;
}

QString BeamQSysRef::enumStrStatic(int v) {
    switch( v ){
    case 0: return trUtf8("Locale");
    case 1: return trUtf8("Globale/Locale");
    case 2: return trUtf8("Globale");
    default: return QString();
    }
}

int BeamQSysRef::valueNormalInt() {
    return m_d->valueIndex();
}

class BeamQSysRefGlobalPrivate{
public:
    BeamQSysRefGlobalPrivate( int v, Hypothesis * hyp ):
        value(v){
        for( int i=0; i < hyp->spaceDim(); ++i ){
            QString cap;
            for( int j=0; j < hyp->spaceDim(); ++j){
                if( i != j ){
                    cap += hyp->dimCaption( j );
                }
            }
            enumList.append( cap );
        }
    };
    ~BeamQSysRefGlobalPrivate(){
    };

    bool setValue( int v ){
        if( v < enumList.size() && v >= 0 ){
            value = v;
            return true;
        } else {
            value = 0;
            return false;
        }
    }

    int value;
    QList<QString> enumList;
};

BeamQSysRefGlobal::BeamQSysRefGlobal(int v,
                                     Hypothesis * hyp,
                                     const QString &n,
                                     bool ro):
    EnumPlus( n, ro),
    m_d( new BeamQSysRefGlobalPrivate( v, hyp )){
}

BeamQSysRefGlobal::~BeamQSysRefGlobal(){
    delete m_d;
}

QString BeamQSysRefGlobal::typeVar(){
    return QString("BeamQSysRefGlobal");
}

BeamQSysRefGlobal & BeamQSysRefGlobal::operator=( BeamQSysRefGlobal & val ){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        m_d->enumList = val.m_d->enumList;
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & BeamQSysRefGlobal::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        BeamQSysRefGlobal * valSysRefPlus = dynamic_cast<BeamQSysRefGlobal *>(&val);
        if(valSysRefPlus) {
            m_d->enumList = valSysRefPlus->m_d->enumList;
            setValueNormal( valSysRefPlus->m_d->value );
        }
    }
    return *this;
}

int BeamQSysRefGlobal::value(){
    return m_d->value;
}

int BeamQSysRefGlobal::valueNormal(){
    return m_d->value;
}

QString BeamQSysRefGlobal::valueNormalStr(){
    return m_d->enumList.at( m_d->value );
}

QString BeamQSysRefGlobal::valueStr(){
    return m_d->enumList.at( m_d->value ) ;
}

void BeamQSysRefGlobal::setValueNormal( int vv, bool emitAuto ){
    if( vv != m_d->value ){
        if( m_d->setValue( vv ) ){
            emit valueChangedInternal( valueStr() );
            emit valueIntChangedWidget( m_d->value );
            if( emitAuto && !m_signalsSuspended ){
                emit valueChanged( valueStr() );
                emit valueIntChanged( m_d->value );
            }
        }
    }
}

void BeamQSysRefGlobal::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).toUpper() == v ){
            setValueNormal( i, emitAuto );
            return;
        }
    }
}

void BeamQSysRefGlobal::setValueInt( int i, bool emitAuto ){
    setValueNormal( i, emitAuto );
}

void BeamQSysRefGlobal::setValue( int vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void BeamQSysRefGlobal::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).toUpper() == v ){
            setValueNormal( i, emitAuto );
            return;
        }
    }
}

int BeamQSysRefGlobal::enumCount() {
    return m_d->enumList.size();
}

QString BeamQSysRefGlobal::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v);
    }
    return QString();
}

int BeamQSysRefGlobal::valueNormalInt() {
    return m_d->value;
}
