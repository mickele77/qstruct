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

#include "varpluscontainer.h"

#include "qstringplus.h"
#include "uintplus.h"
#include "varplus.h"
#include "unitmeasure.h"

#include <QXmlAttributes>
#include <QXmlStreamAttributes>
#include <QString>
#include <QMap>
#include <QList>
#include <QObject>

VarPlusContainer::VarPlusContainer(UnitMeasure * ump, const QString & t, const QString & n):
    m_typeNameInternal( new QString(t) ),
    m_unitMeasure( ump ),
    m_varContainer( new QMap<QString, VarPlus *> ),
    m_usingContainer( new QList<VarPlusContainer *> ){

    id = new UIntPlus( 1, "id", false );
    id->setToolTip( QObject::trUtf8("Id: numero univoco associato al vertice"));
    id->setRichName( QObject::trUtf8("Id"));
    addVarToContainer( id );

    name = new QStringPlus( n, "name", false );
    name->setToolTip( QObject::trUtf8("Denominazione del contenitore"));
    name->setRichName( QObject::trUtf8("Denominazione") );
    addVarToContainer( name );
}

VarPlusContainer::~VarPlusContainer() {
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i ){
        VarPlus::deleteVarPlus( i.value() );
    }
    m_varContainer->clear();
}

VarPlusContainer & VarPlusContainer::operator=( VarPlusContainer & val){
    if( this != &val ){
        m_unitMeasure = val.unitMeasure();
        for( QMap<QString, VarPlus *>::iterator iter = val.m_varContainer->begin(); iter != val.m_varContainer->end(); ++iter ){
            if( (*iter)->name() != QString("id") ){
                if( m_varContainer->contains((*iter)->name())){
                    *(m_varContainer->value((*iter)->name())) = **iter;
                }
            }
        }
    }
    return *this;
}

UnitMeasure * VarPlusContainer::unitMeasure(){
    return m_unitMeasure;
}

void VarPlusContainer::setUnitMeasure( UnitMeasure * ump ){
    m_unitMeasure = ump;
}

void VarPlusContainer::nextId() {
    id->setValueNormal( id->valueNormal() + 1 );
}

void VarPlusContainer::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement(*m_typeNameInternal);
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i) {
        writer->writeAttribute( (*i)->xmlAttribute() );
    }
    writer->writeEndElement();
}

void VarPlusContainer::addVarToContainer( VarPlus * var ){
    m_varContainer->insert( var->name(), var );
}

void VarPlusContainer::loadFromXML( const QXmlStreamAttributes &attrs ){
    for ( int i=0; i<attrs.count(); ++i ) {
        for( QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin(); iter != m_varContainer->end(); ++iter ){
            if ( attrs.at(i).name().toString().toUpper() == (*iter)->name().toUpper() ) {
                (*iter)->setValueXml( attrs.at(i).value().toString() );
            }
        }
    }
    updateReadOnlyVars();
}

void VarPlusContainer::updateReadOnlyVars() {
    // aggiorniamo il valore di tutte le variabili readonly
    QList<VarPlus *> updatedVars;
    for( QMap<QString, VarPlus *>::iterator var = m_varContainer->begin(); var != m_varContainer->end(); ++var ){
        if( var.value()->readOnly() ){
            if( !updatedVars.contains(var.value() ) ){
                var.value()->updateConnectedVarsValue( &updatedVars );
                var.value()->updateValue( false );
                updatedVars.append( var.value() );
            }
        }
    }
}

void VarPlusContainer::setIsUsedBy(VarPlusContainer *otherContainer) {
    if( !m_usingContainer->contains( otherContainer )){
        m_usingContainer->append( otherContainer );
    }
}

void VarPlusContainer::removeIsUsedBy(VarPlusContainer *otherContainer) {
    m_usingContainer->removeAll( otherContainer );
}

bool VarPlusContainer::isUsed() {
    return m_usingContainer->size() > 0;
}

void VarPlusContainer::suspendSignals( bool susp ) {
    for( QMap<QString, VarPlus *>::iterator var = m_varContainer->begin(); var != m_varContainer->end(); ++var ){
        var.value()->suspendSignals( susp );
        if( susp ){
            var.value()->storeValue();
        } else {
            var.value()->checkStoredValue();
        }
    }
}
