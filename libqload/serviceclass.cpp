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
#include "serviceclass.h"

#include <QList>
#include <QCoreApplication>

class ServiceClassEnumVal{
public:
    ServiceClassEnumVal(ServiceClass::Class sc, const QString & ns, const QString & s):
        val(sc),
        normalStr(ns),
        str(s){
    };
    ServiceClass::Class val;
    QString normalStr;
    QString str;
};

class ServiceClassPrivate {
    Q_DECLARE_TR_FUNCTIONS(ServiceClassPrivate)
public:
    ServiceClassPrivate(ServiceClass::Class tt):
        value(tt){
        enumList.append( ServiceClassEnumVal(ServiceClass::SC1, "SC1", trUtf8("CS1")) );
        enumList.append( ServiceClassEnumVal(ServiceClass::SC2, "SC2", trUtf8("CS2")) );
        enumList.append( ServiceClassEnumVal(ServiceClass::SC3, "SC3", trUtf8("CS3")) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    };
    ServiceClass::Class value;
    QList<ServiceClassEnumVal> enumList;
};

ServiceClass::ServiceClass( ServiceClass::Class tt,
                            const QString & nn,
                            bool ro):
    EnumPlus( nn, ro),
    m_d( new ServiceClassPrivate(tt) ) {
}

ServiceClass::~ServiceClass(){
    delete m_d;
}

QString ServiceClass::typeVar(){
    return QString("ServiceClass");
}

ServiceClass & ServiceClass::operator=( ServiceClass & val ){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & ServiceClass::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        ServiceClass * valServiceClass = dynamic_cast<ServiceClass *>(&val);
        if(valServiceClass) {
            setValueNormal( valServiceClass->m_d->value );
        }
    }
    return *this;
}

ServiceClass::Class ServiceClass::value(){
    return m_d->value;
}

ServiceClass::Class ServiceClass::valueNormal(){
    return m_d->value;
}

QString ServiceClass::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString ServiceClass::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

int ServiceClass::enumCount() {
    return m_d->enumList.size();
}

QString ServiceClass::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int ServiceClass::valueNormalInt() {
    return m_d->valueIndex();
}

void ServiceClass::setValueNormal( ServiceClass::Class vv, bool emitAuto ){
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

void ServiceClass::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void ServiceClass::setValue( ServiceClass::Class vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void ServiceClass::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void ServiceClass::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

