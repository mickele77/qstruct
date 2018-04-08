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
#include "lstype.h"

#include <QList>
#include <QCoreApplication>

class LSTypeEnumVal{
public:
    LSTypeEnumVal(LSType::Type sc, const QString & ns, const QString & s):
        val(sc),
        normalStr(ns),
        str(s){
    };
    LSType::Type val;
    QString normalStr;
    QString str;
};

class LSTypePrivate {
    Q_DECLARE_TR_FUNCTIONS(LSTypePrivate)
public:
    LSTypePrivate(LSType::Type tt):
        value(tt){
        enumList.append( LSTypeEnumVal(LSType::ULSFundamental, "ULSFundamental", trUtf8("SLU - comb. fondamentali")) );
        enumList.append( LSTypeEnumVal(LSType::ULSExceptional, "ULSExceptional", trUtf8("SLU - comb. eccezionali")) );
        enumList.append( LSTypeEnumVal(LSType::ELS, "ELS", trUtf8("SLE")) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    };
    LSType::Type value;
    QList<LSTypeEnumVal> enumList;
};

LSType::LSType( LSType::Type tt,
                const QString & nn,
                bool ro):
    EnumPlus( nn, ro),
    m_d( new LSTypePrivate(tt) ){
}

LSType::~LSType(){
    delete m_d;
}

QString LSType::typeVar(){
    return QString("LSType");
}

LSType & LSType::operator=( LSType & val ){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & LSType::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        LSType * valLSType = dynamic_cast<LSType *>(&val);
        if(valLSType) {
            setValueNormal( valLSType->m_d->value );
        }
    }
    return *this;
}

LSType::Type LSType::value(){
    return m_d->value;
}

LSType::Type LSType::valueNormal(){
    return m_d->value;
}

QString LSType::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString LSType::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

int LSType::enumCount() {
    return m_d->enumList.size();
}

QString LSType::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int LSType::valueNormalInt() {
    return m_d->valueIndex();
}

void LSType::setValueNormal( LSType::Type vv, bool emitAuto ){
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

void LSType::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void LSType::setValue( LSType::Type vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void LSType::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void LSType::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}
