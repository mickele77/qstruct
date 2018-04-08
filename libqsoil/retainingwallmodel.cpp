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

#include "retainingwallmodel.h"

#include "tablemodelplusprivate.h"

#include "soilmodel.h"
#include "retainingwall.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "doubleplus.h"

#include <QXmlStreamWriter>
#include <QList>
#include <QMap>
#include <QColor>
#include <QFont>
#include <QVariant>

typedef QList<RetainingWall *>::iterator RetainingWallIterator;

class RetainingWallModelPrivate{
public:
    RetainingWallModelPrivate( SoilModel * sModel ):
        soilModel(sModel),
        defaultRetainingWall(-1),
        currentRetainingWall(-1){
    }
    RetainingWall * retainingWall( int p ){
        if( p >= 0 && p < retainingWallContainer.size() ){
            return retainingWallContainer.at(p);
        } else {
            return NULL;
        }
    }
    RetainingWall * retainingWallId( unsigned int id ){
        return retainingWallMap.value( id, NULL );
    }
    void insertRetainingWall( RetainingWall * rw, int p ){
        // l'id deve essere unico
        while( retainingWallMap.contains(rw->id->valueNormal()) ){
            rw->nextId();
        }
        retainingWallContainer.insert( p, rw );
        retainingWallMap.insert( rw->id->valueNormal(), rw );
    };
    void removeRetainingWall( int p ){
        retainingWallMap.remove( retainingWallContainer.at(p)->id->valueNormal() );
        delete retainingWallContainer.at(p);
        retainingWallContainer.removeAt(p);
    };
    QList<RetainingWall *> retainingWallContainer;
    QMap<unsigned int, RetainingWall * > retainingWallMap;
    SoilModel * soilModel;
    int defaultRetainingWall;
    int currentRetainingWall;
};

RetainingWallModel::RetainingWallModel(UnitMeasure * ump, SoilModel * sModel, QObject *parent) :
    TableModelPlus( "RetainingWallModel", ump, parent ),
    m_dd( new RetainingWallModelPrivate( sModel ) ){
    QList<QString> headers;
    headers << trUtf8("Id") << trUtf8("Nome");
    setHeaders( headers );
}

void RetainingWallModel::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement( m_d->modelName );
    for( RetainingWallIterator i = m_dd->retainingWallContainer.begin(); i != m_dd->retainingWallContainer.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void RetainingWallModel::readXml(QXmlStreamReader *reader) {
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
            RetainingWall * addedWall = new RetainingWall( m_d->unitMeasure, reader->attributes(), m_dd->soilModel );
            insertRetainingWall( addedWall, m_dd->retainingWallContainer.size() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void RetainingWallModel::insertRows( int position, int count ){
    if (position < 0 || position > m_dd->retainingWallContainer.size() )
        return;

    if( m_dd->soilModel->defaultSoil() != NULL ){
        for (int row = position; row < (position+count); ++row) {
            RetainingWall * addedWall = new RetainingWall( m_d->unitMeasure, "Muro", m_dd->soilModel->defaultSoil(), m_dd->soilModel->defaultSoil() );
            insertRetainingWall( addedWall, position );
        }
    }
}

void RetainingWallModel::insertRetainingWall( RetainingWall * addedRW, int position ){
    if (position < 0 || position > m_dd->retainingWallContainer.size() )
        position = m_dd->retainingWallContainer.size();

    if( addedRW ){
        m_dd->insertRetainingWall( addedRW, position );
        insertRowsPrivate( position );
        setVarValueRow( position, addedRW->id, addedRW->name );
        emit modelChanged();
    }
}

void RetainingWallModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->retainingWallContainer.size())
        return;

    for (int row = position; row < (position+count); row++){
        // ok, procediamo a cancellare la riga
        removeRowsPrivate( position );
        // e la sezione
        m_dd->removeRetainingWall( position );
    }
    emit modelChanged();
}

RetainingWall *RetainingWallModel::defaultRetainingWall() {
    if( m_dd->defaultRetainingWall >= 0 && m_dd->defaultRetainingWall < m_dd->retainingWallContainer.size() ){
        return m_dd->retainingWallContainer.at( m_dd->defaultRetainingWall );
    } else {
        return NULL;
    }
}

int RetainingWallModel::defaultRetainingWallIndex() {
    return m_dd->defaultRetainingWall;
}

void RetainingWallModel::setDefaultRetainingWall( int i ){
    if( i >= 0 && i < m_dd->retainingWallContainer.size() ){
        if( i != m_dd->defaultRetainingWall ){
            m_dd->defaultRetainingWall =  i;
            emit defaultChanged( m_dd->retainingWallContainer.at( m_dd->defaultRetainingWall ) );
        }
    }
}

RetainingWall * RetainingWallModel::retainingWall( int i ){
    if( i >= 0 && i < m_dd->retainingWallContainer.size() )
        return m_dd->retainingWallContainer.at( i );
    else
        return 0;
}

RetainingWall * RetainingWallModel::retainingWallId( unsigned int i){
    return m_dd->retainingWallId( i );
}

int RetainingWallModel::retainingWallCount(){
    return m_dd->retainingWallContainer.size();
}

RetainingWall * RetainingWallModel::currentRetainingWall(){
    if( m_dd->currentRetainingWall >= 0 && m_dd->currentRetainingWall < m_dd->retainingWallContainer.size() ){
        return m_dd->retainingWallContainer.at( m_dd->currentRetainingWall );
    } else {
        return NULL;
    }
}

int RetainingWallModel::currentRetainingWallIndex(){
    return m_dd->currentRetainingWall;
}

void RetainingWallModel::setCurrentRetainingWall( int i ){
    if( i >= 0 && i < m_dd->retainingWallContainer.size() ){
        if( i != m_dd->currentRetainingWall ){
            m_dd->currentRetainingWall =  i;
            emit currentChanged( currentRetainingWall() );
        }
    }
}
