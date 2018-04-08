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

#include "timberconnectionmodel.h"

#include "tablemodelplusprivate.h"

#include "materialmodel.h"
#include "timberconnection.h"
#include "timber.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "doubleplus.h"

#include <QXmlStreamWriter>
#include <QList>
#include <QMap>
#include <QColor>
#include <QFont>
#include <QVariant>

typedef QList<TimberConnection *>::iterator TimberConnectionIterator;

class TimberConnectionModelPrivate {
public:
    TimberConnectionModelPrivate( MaterialModel * matModel ):
        materialModel( matModel ),
        currentTimberConnection( -1 ) {
    };
    TimberConnection * connection( int p ){
        if( p >= 0 && p < timberConnectionContainer.size() ){
            return timberConnectionContainer.at(p);
        } else {
            return NULL;
        }
    };
    TimberConnection * connectionId( unsigned int id ){
        return timberConnectionMap.value( id, NULL );
    };
    void insertConnection( TimberConnection * con, int p ){
        // l'id deve essere unico
        while( timberConnectionMap.contains(con->id->valueNormal()) ){
            con->nextId();
        }
        timberConnectionContainer.insert( p, con );
        timberConnectionMap.insert( con->id->valueNormal(), con );
    };
    void removeConnection( int p ){
        timberConnectionMap.remove( timberConnectionContainer.at(p)->id->valueNormal() );
        delete timberConnectionContainer.at(p);
        timberConnectionContainer.removeAt(p);
    };
    MaterialModel * materialModel;
    QList<TimberConnection *> timberConnectionContainer;
    QMap<unsigned int, TimberConnection *> timberConnectionMap;
    int currentTimberConnection;
};

TimberConnectionModel::TimberConnectionModel(UnitMeasure * ump, MaterialModel * matModel, QObject *parent) :
    TableModelPlus( "TimberConnectionModel", ump, parent ),
    m_dd( new TimberConnectionModelPrivate( matModel ) ){
    QList<QString> headers;
    headers <<  trUtf8("Id") << trUtf8("Nome");
    setHeaders( headers );
}

void TimberConnectionModel::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement( m_d->modelName );
    for( TimberConnectionIterator i = m_dd->timberConnectionContainer.begin(); i != m_dd->timberConnectionContainer.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void TimberConnectionModel::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    QString myName = m_d->modelName.toUpper();

    while( !((t == QXmlStreamReader::StartElement) && (n == myName))  &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }

    while( !((t == QXmlStreamReader::EndElement) && (n == myName)) &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        if( t == QXmlStreamReader::StartElement && n == "TIMBERCONNECTION"){
            TimberConnection * addedConnection = new TimberConnection( m_d->unitMeasure, reader->attributes(), m_dd->materialModel );
            insertConnection( addedConnection, m_dd->timberConnectionContainer.size() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void TimberConnectionModel::insertRows( int position, int count ){
    if (position < 0 || position > m_dd->timberConnectionContainer.size() )
        return;
    Timber * timb = dynamic_cast<Timber *>(m_dd->materialModel->firstMaterial(MaterialModel::TimberMaterial ));
    if( timb ) {
        for( int i=0; i < count; ++i){
            TimberConnection * connection = new TimberConnection( m_d->unitMeasure, trUtf8("Connessione"), timb );
            insertConnection( connection, position );
        }
    }
}

void TimberConnectionModel::insertConnection( TimberConnection * connection, int position ){
    if (position < 0 || position > m_dd->timberConnectionContainer.size() )
        position = m_dd->timberConnectionContainer.size();
    if( connection){
        m_dd->insertConnection( connection, position );

        insertRowsPrivate( position );

        setVarValueRow( position, connection->id, connection->name );

        emit modelChanged();
    }
}

void TimberConnectionModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->timberConnectionContainer.size())
        return;

    for (int row = position; row < (position+count); row++){
        // ok, procediamo a cancellare la riga
        removeRowsPrivate( position );
        // e la sezione
        m_dd->removeConnection( position );
    }
    emit modelChanged();
}

TimberConnection * TimberConnectionModel::timberConnection( int i ){
    if( i >= 0 && i < m_dd->timberConnectionContainer.size() )
        return m_dd->timberConnectionContainer.at( i );
    else
        return 0;
}

TimberConnection * TimberConnectionModel::timberConnection(const QModelIndex & index ){
    return m_dd->connection( index.row() );
}

TimberConnection * TimberConnectionModel::timberConnectionId( unsigned int i){
    return m_dd->connectionId(i);
}

int TimberConnectionModel::timberConnectionCount(){
    return m_dd->timberConnectionContainer.size();
}

TimberConnection * TimberConnectionModel::currentTimberConnection(){
    if( m_dd->currentTimberConnection >= 0 && m_dd->currentTimberConnection < m_dd->timberConnectionContainer.size() ){
        return m_dd->timberConnectionContainer.at( m_dd->currentTimberConnection );
    } else {
        return NULL;
    }
}

int TimberConnectionModel::currentTimberConnectionIndex(){
    return m_dd->currentTimberConnection;
}

void TimberConnectionModel::setCurrentTimberConnection( int i ){
    if( i >= -1 && i < m_dd->timberConnectionContainer.size() ){
        if( i != m_dd->currentTimberConnection ){
            m_dd->currentTimberConnection =  i;
            emit currentChanged( currentTimberConnection() );
        }
    }
}
