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

#include "soilmodel.h"

#include "tablemodelplusprivate.h"

#include "soil.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "doubleplus.h"

#include <QXmlStreamWriter>
#include <QList>
#include <QMap>
#include <QColor>
#include <QFont>
#include <QVariant>

typedef QList<Soil *>::iterator SoilIterator;

class SoilModelPrivate{
public:
    SoilModelPrivate():
        defaultSoil(-1),
        currentSoil(-1) {

    }
    Soil * soil( int p ){
        if( p >= 0 && p < soilContainer.size() ){
            return soilContainer.at(p);
        } else {
            return NULL;
        }
    }
    Soil * soilId( unsigned int id ){
        return soilMap.value( id, NULL );
    }
    void insertSoil( Soil * s, int p ){
        // l'id deve essere unico
        while( soilMap.contains(s->id->valueNormal()) ){
            s->nextId();
        }
        soilContainer.insert( p, s );
        soilMap.insert( s->id->valueNormal(), s );
    };
    void removeSoil( int p ){
        soilMap.remove( soilContainer.at(p)->id->valueNormal() );
        delete soilContainer.at(p);
        soilContainer.removeAt(p);
    };
    QList<Soil *> soilContainer;
    QMap<unsigned int, Soil *> soilMap;
    int defaultSoil;
    int currentSoil;
};

SoilModel::SoilModel(UnitMeasure * ump, QObject *parent) :
    TableModelPlus( "SoilModel", ump, parent ),
    m_dd( new SoilModelPrivate() ) {
    QList<QString> headers;
    headers << trUtf8("Id") << trUtf8("Nome") << trUtf8("Tipo");
    setHeaders( headers );
}

void SoilModel::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement( m_d->modelName );
    for( SoilIterator i = m_dd->soilContainer.begin(); i != m_dd->soilContainer.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void SoilModel::readXml(QXmlStreamReader *reader) {
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
        if( n == "SOIL" && t == QXmlStreamReader::StartElement ){
            Soil * addedSoil = new Soil( m_d->unitMeasure, reader->attributes() );
            insertSoil( addedSoil, m_dd->soilContainer.size() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void SoilModel::insertRows( SoilModel::SoilType typeSoil, int position, int count ){
    if (position < 0 || position > m_dd->soilContainer.size() )
        return;
    for( int i=0; i < count; ++i){
        Soil * addedSoil = createSoil( m_d->unitMeasure, typeSoil );
        if( addedSoil ){
            insertSoil( addedSoil, position );
        }
    }
}

void SoilModel::insertSoil( Soil * addedSoil, int position ){
    if (position < 0 || position > m_dd->soilContainer.size() )
        position = m_dd->soilContainer.size();
    if( addedSoil ){
        m_dd->insertSoil( addedSoil, position );

        insertRowsPrivate( position );

        setVarValueRow( position, addedSoil->id, addedSoil->name, addedSoil->soilType );

        emit modelChanged();
    }
}

void SoilModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->soilContainer.size())
        return;

    for (int row = position; row < (position+count); row++){
        // ok, procediamo a cancellare la riga
        removeRowsPrivate( position );
        // e la sezione
        m_dd->removeSoil( position );
    }
    emit modelChanged();
}

Soil *SoilModel::defaultSoil() {
    if( m_dd->defaultSoil >= 0 && m_dd->defaultSoil < m_dd->soilContainer.size() ){
        return m_dd->soilContainer.at( m_dd->defaultSoil );
    }
    if( m_dd->soilContainer.size() > 0 ){
        return m_dd->soilContainer.at(0);
    }
    return NULL;
}

int SoilModel::defaultSoilIndex() {
    return m_dd->defaultSoil;
}

void SoilModel::setDefaultSoil( int i ){
    if( i >= 0 && i < m_dd->soilContainer.size() ){
        if( i != m_dd->defaultSoil ){
            m_dd->defaultSoil =  i;
            emit dataChanged( createIndex( m_dd->defaultSoil, 0 ), createIndex(m_dd->defaultSoil, columnCount()) );
            emit defaultChanged( m_dd->soilContainer.at( m_dd->defaultSoil ) );
        }
    }
}

Soil * SoilModel::soil( int i ){
    if( i >= 0 && i < m_dd->soilContainer.size() )
        return m_dd->soilContainer.at( i );
    else
        return 0;
}

Soil * SoilModel::soilId( unsigned int i){
    return m_dd->soilId( i );
}

int SoilModel::soilCount(){
    return m_dd->soilContainer.size();
}

int SoilModel::soilCount( SoilType matType){
    SoilIterator iter;
    int ret = 0;
    for( iter = m_dd->soilContainer.begin(); iter != m_dd->soilContainer.end(); iter++){
        switch(matType ){
        case clay: {
            if( (*iter)->soilType->valueNormalStr().toUpper() == "CLAY" ){
                ret++;
            }
            break;
        }
        case sand: {
            if( (*iter)->soilType->valueNormalStr().toUpper() == "SAND" ){
                ret++;
            }
            break;
        }
        }
    }
    return ret;
}

Soil * SoilModel::currentSoil(){
    if( m_dd->currentSoil >= 0 && m_dd->currentSoil < m_dd->soilContainer.size() ){
        return m_dd->soilContainer.at( m_dd->currentSoil );
    } else {
        return NULL;
    }
}

int SoilModel::currentSoilIndex(){
    return m_dd->currentSoil;
}

void SoilModel::setCurrentSoil( int i ){
    if( i >= 0 && i < m_dd->soilContainer.size() ){
        if( i != m_dd->currentSoil ){
            m_dd->currentSoil =  i;
            emit currentChanged( currentSoil() );
        }
    }
}

Soil * SoilModel::createSoil( UnitMeasure * um, SoilModel::SoilType st ){
    switch( st ){
    case clay:
        return new Soil( um, trUtf8("Argilla"), Soil::clay );
    default:
        return new Soil( um, trUtf8("Sabbia"), Soil::sand );
    }
}
