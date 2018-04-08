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
#include "load.h"

#include "loadgroup.h"

Load::Load( double vv,
           const QString & nn,
           LoadType::Type lType,
           LoadPhase * lph,
           LoadGroup * lg,
           UnitMeasure * ump,
           bool ro):
    VarPlus( nn, ump, UnitMeasure::loadF, ro),
    loadType( new LoadType(lType, "loadType", false) ),
    m_value(vv),
    m_phase(lph),
    m_group(lg){

    connect( loadType, SIGNAL(valueChanged(QString)), this, SLOT(updateUnitMeasure()));
    updateUnitMeasure();
}

void Load::updateUnitMeasure() {
    switch( loadType->valueNormal() ){
    case LoadType::loadPoint :{
        setUnitMeasure( UnitMeasure::loadF );
        break;}
    case LoadType::loadLine :{
        setUnitMeasure( UnitMeasure::loadFLine );
        break;}
    case LoadType::loadArea :{
        setUnitMeasure( UnitMeasure::loadFArea );
        break;}
    case LoadType::loadVolume :{
        setUnitMeasure( UnitMeasure::loadFVolume );
        break;}
    default: {
        break; }
    }
}

QString Load::typeVar(){
    return QString("Load");
}

Load & Load::operator=( Load & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_value );
    }
    return *this;
}

VarPlus & Load::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        Load *valLoad = dynamic_cast<Load*>(&val);
        if( valLoad ){
            setValueNormal( valLoad->m_value );
        }
    }
    return *this;
}

double Load::value(){
    return m_value * m_unitMeasurePointer->scale( m_unitMeasureValue );
}

double Load::valueNormal(){
    return m_value;
}

QString Load::valueStr(){
    return QString::number( value() );
}

QString Load::valueNormalStr(){
    return QString::number( valueNormal() );
}

void Load::setValueNormal( double vv ){
    setValueNormal( vv , true);
}

void Load::setValueNormal( double vv, bool emitAuto ){
    if( vv != m_value ){
        m_value = vv;
        if( emitAuto ){
            emit valueChanged( valueStr() );
        }
    }
}

void Load::setValue( double vv ){
    setValue( vv , true);
}

void Load::setValue( double vv, bool emitAuto ){
    setValueNormal( vv / m_unitMeasurePointer->scale(m_unitMeasureValue ), emitAuto);
}

void Load::setValueNormal( const QString & vvStr ){
    setValueNormal(vvStr.toDouble());
}

void Load::setValue( const QString & vvStr ){
    setValue(vvStr.toDouble());
}
