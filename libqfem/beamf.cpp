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
#include "beamf.h"

#include "hypothesis.h"
#include "doubleplus.h"
#include "uintplus.h"

#define NVERT 2

class BeamFPrivate{
public:
    BeamFPrivate( Hypothesis * hyp, UnitMeasure * ump ){
        nDOF = NVERT * hyp->nDOFVert();
        fVal = new double[ nDOF ];
        for( int i=0;i < nDOF; i++ ){
            fVal[i] = 0.0;
            int j = i % hyp->nDOFVert();
            if( j / hyp->spaceDim() == 0 ){
                f.append( new DoublePlus( &(fVal[i]), "f" + QString::number( j % hyp->spaceDim()), ump, UnitMeasure::loadF) );
            } else {
                f.append( new DoublePlus( &(fVal[i]), "f" + QString::number( j % hyp->spaceDim()), ump, UnitMeasure::loadM) );
            }
        }
        sysRef = new BeamFSysRef( BeamF::fLocal, "sysRef" );
    };
    ~BeamFPrivate(){
        for( int i=0;i < nDOF; i++ ){
            VarPlus::deleteVarPlus( f.at(i));
        }
        f.clear();
        delete [] fVal;
        VarPlus::deleteVarPlus( sysRef );
    };

    int nDOF;
    double * fVal;
    QList<DoublePlus *> f;
    BeamFSysRef * sysRef;

    static unsigned int nextId;
};

unsigned int  BeamFPrivate::nextId = 1;

BeamF::BeamF( Hypothesis * hyp, UnitMeasure * ump, const QString & n, QObject * parent ):
    QObject( parent ),
    VarPlusContainer( ump, "BeamF", n ),
    m_d( new BeamFPrivate( hyp, ump)){
}

int BeamF::nVert(){
    return NVERT;
}

BeamFSysRef * BeamF::sysRef(){
    return m_d->sysRef;
}

double * BeamF::fVal() const{
    return m_d->fVal;
}

DoublePlus * BeamF::f(int i){
    if( i < m_d->f.size() ){
        return m_d->f.at(i);
    }
    return NULL;
}

void BeamF::nextId( void ){
    id->setValueNormal( m_d->nextId );
    m_d->nextId++;
}

#include <QApplication>

class BeamFSysRefPrivate{
    Q_DECLARE_TR_FUNCTIONS(BeamFSysRefPrivate)

    class enumVal{
    public:
        enumVal(BeamF::fSysRef v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        BeamF::fSysRef val;
        QString normalStr;
        QString str;
    };

public:
    BeamFSysRefPrivate(BeamF::fSysRef v):
        value(v){
        enumList.append( enumVal( BeamF::fLocal, "fLocal", trUtf8("Locale")) );
        enumList.append( enumVal( BeamF::fGlobal, "fGlobal", trUtf8("Globale")) );
    };
    ~BeamFSysRefPrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    BeamF::fSysRef value;
    QList<enumVal> enumList;
};

BeamFSysRef::BeamFSysRef( BeamF::fSysRef sr,
                          const QString & nn,
                          bool ro):
    EnumPlus( nn, ro),
    m_d( new BeamFSysRefPrivate(sr )){
}

BeamFSysRef::~BeamFSysRef(){
    delete m_d;
}

BeamFSysRef & BeamFSysRef::operator=( BeamFSysRef & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & BeamFSysRef::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        BeamFSysRef * valCncrModelPlus = dynamic_cast<BeamFSysRef *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString BeamFSysRef::typeVar(){
    return QString("BeamFSysRef");
}

BeamF::fSysRef BeamFSysRef::value(){
    return m_d->value;
}

BeamF::fSysRef BeamFSysRef::valueNormal(){
    return m_d->value;
}

QString BeamFSysRef::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString BeamFSysRef::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void BeamFSysRef::setValueNormal( BeamF::fSysRef vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void BeamFSysRef::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void BeamFSysRef::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void BeamFSysRef::setValue( BeamF::fSysRef vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void BeamFSysRef::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int BeamFSysRef::enumCount() {
    return m_d->enumList.size();
}

QString BeamFSysRef::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int BeamFSysRef::enumCountStatic() {
    return 2;
}

QString BeamFSysRef::enumStrStatic(int v) {
    switch( v ){
    case 0: return trUtf8("Locale");
    case 1: return trUtf8("Globale");
    default: return QString();
    }
}

int BeamFSysRef::valueNormalInt() {
    return m_d->valueIndex();
}
