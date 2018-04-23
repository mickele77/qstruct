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
#include "loadtype.h"

#include <QCoreApplication>

class LoadTypePrivate{
    Q_DECLARE_TR_FUNCTIONS(LoadTypePrivate)

    class enumVal{
    public:
        enumVal(LoadType::Type v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        LoadType::Type val;
        QString normalStr;
        QString str;
    };

public:
    LoadTypePrivate(LoadType::Type v):
        value(v){
        enumList.append( enumVal( LoadType::loadPoint, "loadPoint", trUtf8("Puntuale") ) );
        enumList.append( enumVal( LoadType::loadLine, "SectionBiLinear", trUtf8("Distribuito lineare")) );
        enumList.append( enumVal( LoadType::loadArea, "loadArea", trUtf8("Distribuito superficiale")) );
        enumList.append( enumVal( LoadType::loadVolume, "loadVolume", trUtf8("Volume")) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    LoadType::Type value;
    QList<enumVal> enumList;
};

LoadType::LoadType( LoadType::Type tt,
                    const QString & nn,
                    bool ro):
    EnumPlus( nn, ro),
    m_d( new LoadTypePrivate(tt) ){
}

LoadType::~LoadType(){
    delete m_d;
}

LoadType & LoadType::operator=( LoadType & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & LoadType::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        LoadType * valModelPlus = dynamic_cast<LoadType *>(&val);
        if(valModelPlus) {
            setValueNormal( valModelPlus->m_d->value );
        }
    }
    return *this;
}

QString LoadType::typeVar(){
    return QString("LoadType");
}

LoadType::Type LoadType::value(){
    return m_d->value;
}

LoadType::Type LoadType::valueNormal(){
    return m_d->value;
}

QString LoadType::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString LoadType::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void LoadType::setValueNormal( LoadType::Type vv, bool emitAuto ){
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

void LoadType::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void LoadType::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void LoadType::setValue( LoadType::Type vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void LoadType::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int LoadType::enumCount() {
    return m_d->enumList.size();
}

QString LoadType::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int LoadType::enumCountStatic(){
    return 4;
}

QString LoadType::enumStrStatic( int v ){
    switch( v ){
    case 0:
        return trUtf8("Puntuale");
        break;
    case 1:
        return trUtf8("Distribuito lineare");
        break;
    case 2:
        return trUtf8("Distribuito superficiale");
        break;
    case 3:
        return trUtf8("Volume");
        break;
    default:
        return QString();
        break;
    }
}

int LoadType::valueNormalInt() {
    return m_d->valueIndex();
}
