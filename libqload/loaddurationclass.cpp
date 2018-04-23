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
#include "loaddurationclass.h"

#include <QList>
#include <QCoreApplication>

class LoadDurationClassPrivate {
    Q_DECLARE_TR_FUNCTIONS(LoadDurationClassPrivate)
public:
    class LoadDurationClassEnumVal{
    public:
        LoadDurationClassEnumVal(LoadDurationClass::Class sc, const QString & ns, const QString & s):
            val(sc),
            normalStr(ns),
            str(s){
        };
        LoadDurationClass::Class val;
        QString normalStr;
        QString str;
    };

    LoadDurationClassPrivate(LoadDurationClass::Class tt):
        value(tt){
        enumList.append( LoadDurationClassEnumVal(LoadDurationClass::permanent, "permanent", trUtf8("permanente")) );
        enumList.append( LoadDurationClassEnumVal(LoadDurationClass::longTerm, "longTerm", trUtf8("lunga durata")) );
        enumList.append( LoadDurationClassEnumVal(LoadDurationClass::mediumTerm, "mediumTerm", trUtf8("media durata")) );
        enumList.append( LoadDurationClassEnumVal(LoadDurationClass::shortTerm, "shortTerm", trUtf8("breve durata")) );
        enumList.append( LoadDurationClassEnumVal(LoadDurationClass::instantaneous, "instantaneous", trUtf8("istantanea")) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    };
    LoadDurationClass::Class value;
    QList<LoadDurationClassEnumVal> enumList;
};

LoadDurationClass::LoadDurationClass( LoadDurationClass::Class tt,
                                      const QString & nn,
                                      bool ro):
    EnumPlus( nn, ro),
    m_d( new LoadDurationClassPrivate(tt) ){
}

LoadDurationClass::~LoadDurationClass(){
    delete m_d;
}

int LoadDurationClass::nVal(){
    return 5;
}

QString  LoadDurationClass::valStr( int i ){
    switch( i ){
    case 0:
        return trUtf8("permanente");
    case 1:
        return trUtf8("lunga durata") ;
    case 2:
        return trUtf8("media durata");
    case 3:
        return trUtf8("breve durata");
    case 4:
        return trUtf8("istantanea");
    default:
        return QString();
    }
}

LoadDurationClass::Class LoadDurationClass::val( int i ){
    switch( i ){
    case 0:
        return permanent;
    case 1:
        return longTerm;
    case 2:
        return mediumTerm;
    case 3:
        return shortTerm;
    case 4:
        return instantaneous;
    default:
        return permanent;
    }
}

QString LoadDurationClass::typeVar(){
    return QString("LoadDurationClass");
}

LoadDurationClass & LoadDurationClass::operator=( LoadDurationClass & val ){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & LoadDurationClass::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        LoadDurationClass * valLoadDurationClass = dynamic_cast<LoadDurationClass *>(&val);
        if(valLoadDurationClass) {
            setValueNormal( valLoadDurationClass->m_d->value );
        }
    }
    return *this;
}

LoadDurationClass::Class LoadDurationClass::value(){
    return m_d->value;
}

LoadDurationClass::Class LoadDurationClass::valueNormal(){
    return m_d->value;
}

QString LoadDurationClass::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString LoadDurationClass::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

int LoadDurationClass::enumCount() {
    return m_d->enumList.size();
}

QString LoadDurationClass::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int LoadDurationClass::valueNormalInt() {
    return m_d->valueIndex();
}

void LoadDurationClass::setValueNormal( LoadDurationClass::Class vv, bool emitAuto ){
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

void LoadDurationClass::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void LoadDurationClass::setValue( LoadDurationClass::Class vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void LoadDurationClass::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void LoadDurationClass::setValueInt(int i , bool emitAuto) {
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

