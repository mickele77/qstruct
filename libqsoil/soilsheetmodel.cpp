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

#include "soilsheetmodel.h"

#include "tablemodelplusprivate.h"

#include "soilmodel.h"
#include "soilsheet.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "doubleplus.h"

#include <QXmlStreamWriter>
#include <QList>
#include <QMap>
#include <QColor>
#include <QFont>
#include <QVariant>

typedef QList<SoilSheet *>::iterator SoilSheetIterator;

class SoilSheetModelPrivate{
public:
    SoilSheetModelPrivate( SoilModel * sModel ):
        soilModel(sModel),
        defaultSoilSheet(-1),
        currentSoilSheet(-1){
    }
    SoilSheet * soilSheet( int p ){
        if( p >= 0 && p < soilSheetContainer.size() ){
            return soilSheetContainer.at(p);
        } else {
            return NULL;
        }
    }
    SoilSheet * soilSheetId( unsigned int id ){
        return soilSheetMap.value( id, NULL );
    }
    void insertSoilSheet( SoilSheet * rw, int p ){
        // l'id deve essere unico
        while( soilSheetMap.contains(rw->id->valueNormal()) ){
            rw->nextId();
        }
        soilSheetContainer.insert( p, rw );
        soilSheetMap.insert( rw->id->valueNormal(), rw );
    }
    void removeSoilSheet( int p ){
        soilSheetMap.remove( soilSheetContainer.at(p)->id->valueNormal() );
        delete soilSheetContainer.at(p);
        soilSheetContainer.removeAt(p);
    }
    QList<SoilSheet *> soilSheetContainer;
    QMap<unsigned int, SoilSheet * > soilSheetMap;
    SoilModel * soilModel;
    int defaultSoilSheet;
    int currentSoilSheet;
};

SoilSheetModel::SoilSheetModel(UnitMeasure * ump, SoilModel * sModel, QObject *parent) :
    TableModelPlus( "SoilSheetModel", ump, parent ),
    m_dd( new SoilSheetModelPrivate( sModel ) ){
    QList<QString> headers;
    headers << trUtf8("Id") << trUtf8("Nome");
    setHeaders( headers );
}

void SoilSheetModel::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement( m_d->modelName );
    for( SoilSheetIterator i = m_dd->soilSheetContainer.begin(); i != m_dd->soilSheetContainer.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void SoilSheetModel::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    QString myName = m_d->modelName.toUpper();

    while( !((t == QXmlStreamReader::StartElement) && (n == myName))  &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }

    while( !(t == QXmlStreamReader::EndElement && (n == myName)) &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        if( n == "RETAININGWALL" && t == QXmlStreamReader::StartElement ){
            SoilSheet * addedWall = new SoilSheet( m_d->unitMeasure, reader->attributes(), m_dd->soilModel );
            insertSoilSheet( addedWall, m_dd->soilSheetContainer.size() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void SoilSheetModel::insertRows( int position, int count ){
    if (position < 0 || position > m_dd->soilSheetContainer.size() )
        return;

    if( m_dd->soilModel->defaultSoil() != NULL ){
        for (int row = position; row < (position+count); ++row) {
            SoilSheet * addedWall = new SoilSheet( m_d->unitMeasure, "Muro", m_dd->soilModel->defaultSoil(), m_dd->soilModel->defaultSoil() );
            insertSoilSheet( addedWall, position );
        }
    }
}

void SoilSheetModel::insertSoilSheet( SoilSheet * addedRW, int position ){
    if (position < 0 || position > m_dd->soilSheetContainer.size() )
        position = m_dd->soilSheetContainer.size();

    if( addedRW ){
        m_dd->insertSoilSheet( addedRW, position );
        insertRowsPrivate( position );
        setVarValueRow( position, addedRW->id, addedRW->name );
        emit modelChanged();
    }
}

void SoilSheetModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->soilSheetContainer.size())
        return;

    for (int row = position; row < (position+count); row++){
        // ok, procediamo a cancellare la riga
        removeRowsPrivate( position );
        // e la sezione
        m_dd->removeSoilSheet( position );
    }
    emit modelChanged();
}

SoilSheet *SoilSheetModel::defaultSoilSheet() {
    if( m_dd->defaultSoilSheet >= 0 && m_dd->defaultSoilSheet < m_dd->soilSheetContainer.size() ){
        return m_dd->soilSheetContainer.at( m_dd->defaultSoilSheet );
    } else {
        return NULL;
    }
}

int SoilSheetModel::defaultSoilSheetIndex() {
    return m_dd->defaultSoilSheet;
}

void SoilSheetModel::setDefaultSoilSheet( int i ){
    if( i >= 0 && i < m_dd->soilSheetContainer.size() ){
        if( i != m_dd->defaultSoilSheet ){
            m_dd->defaultSoilSheet =  i;
            emit defaultChanged( m_dd->soilSheetContainer.at( m_dd->defaultSoilSheet ) );
        }
    }
}

SoilSheet * SoilSheetModel::soilSheet( int i ){
    if( i >= 0 && i < m_dd->soilSheetContainer.size() )
        return m_dd->soilSheetContainer.at( i );
    else
        return 0;
}

SoilSheet * SoilSheetModel::soilSheetId( unsigned int i){
    return m_dd->soilSheetId( i );
}

int SoilSheetModel::soilSheetCount(){
    return m_dd->soilSheetContainer.size();
}

SoilSheet * SoilSheetModel::currentSoilSheet(){
    if( m_dd->currentSoilSheet >= 0 && m_dd->currentSoilSheet < m_dd->soilSheetContainer.size() ){
        return m_dd->soilSheetContainer.at( m_dd->currentSoilSheet );
    } else {
        return NULL;
    }
}

int SoilSheetModel::currentSoilSheetIndex(){
    return m_dd->currentSoilSheet;
}

void SoilSheetModel::setCurrentSoilSheet( int i ){
    if( i >= 0 && i < m_dd->soilSheetContainer.size() ){
        if( i != m_dd->currentSoilSheet ){
            m_dd->currentSoilSheet =  i;
            emit currentChanged( currentSoilSheet() );
        }
    }
}
