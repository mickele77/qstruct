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
#include "loadcompltablemodel.h"

#include "tablemodelplusprivate.h"

#include "loaddurationclass.h"
#include "unitmeasure.h"
#include "boolplus.h"
#include "qstringplus.h"
#include "doubleplus.h"

#include <QXmlStreamWriter>
#include <QList>

typedef QList<LoadCompl *>::iterator LoadIterator;


class LoadComplTableModelPrivate{
public:
    LoadComplTableModelPrivate(){
        indexName = 0;
        indexLType = indexName + 1; // 1
        indexVar = indexLType + 1; // 2
        indexPerm = indexVar + 1; // 3
        indexDuration = indexPerm + 1; // 4
        indexGamma = indexDuration + 1; // 5
        indexPsi0 = indexGamma + 1; // 6-8
        indexP0 = indexPsi0 + 3; // 9-19
        indexLast = indexP0 + LOADCOMPL_NP - 1; //
    };
    void insertLoad( int position, LoadCompl * addedLoad ){
        loadContainer.insert( position, addedLoad );
    };
    void removeLoad( int position ){
        delete loadContainer.at(position);
        loadContainer.removeAt( position );
    };


    QList<LoadCompl *> loadContainer;
    int indexName;
    int indexLType;
    int indexVar;
    int indexPerm;
    int indexDuration;
    int indexGamma;
    int indexPsi0;
    int indexP0;
    int indexLast;
};

LoadComplTableModel::LoadComplTableModel(UnitMeasure * ump, QObject *parent) :
    TableModelPlus("LoadComplTableModel", ump, parent),
    m_dd( new  LoadComplTableModelPrivate() ) {
    QList<QString> headerList;
    headerList << trUtf8("Nome")
               << trUtf8("Tipo")
               << trUtf8("Variabile")
               << trUtf8("Permuta")
               << trUtf8("Durata")
               << trUtf8("γ");
    for( int i=0; i < 3; ++i){
        headerList << trUtf8("ψ%1").arg(i);
    }
    for( int i=0; i < LOADCOMPL_NP; ++i){
        headerList << trUtf8("P%1").arg(i);
        headerList << QString();
    }
    m_d->setHeaders( headerList );
}

LoadComplTableModel::~LoadComplTableModel() {
}

void LoadComplTableModel::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement( m_d->modelName );
    for( LoadIterator i = m_dd->loadContainer.begin(); i != m_dd->loadContainer.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void LoadComplTableModel::readXml(QXmlStreamReader *reader) {
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
        if( t == QXmlStreamReader::StartElement ){
            LoadCompl * addedLoad = new LoadCompl( m_d->unitMeasure, reader->attributes() );
            m_dd->insertLoad( m_dd->loadContainer.size(), addedLoad );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

LoadCompl * LoadComplTableModel::load(int i ){
    if( i >= 0 && i < m_dd->loadContainer.size() )
        return m_dd->loadContainer.at( i );
    else
        return NULL;
}

int LoadComplTableModel::count(){
    return m_dd->loadContainer.size();
}

void LoadComplTableModel::insertRows(LoadType::Type lType, int position, int count ){
    if (position < 0 || position > m_dd->loadContainer.size() )
        return;
    for (int row = position; row < (position+count); ++row) {
        LoadCompl * addedLoad = new LoadCompl( m_d->unitMeasure, "", 1.0,  lType, LoadDurationClass::shortTerm );
        m_dd->insertLoad( position, addedLoad );
        insertRowsPrivate( position, count );
        setVarValue( row, m_dd->indexName, addedLoad->name);
        setVarValue( row, m_dd->indexLType, addedLoad->loadType);
        setVarValue( row, m_dd->indexVar, addedLoad->variable);
        setVarValue( row, m_dd->indexPerm, addedLoad->permute);
        setVarValue( row, m_dd->indexDuration, addedLoad->durationClass);
        setVarValue( row, m_dd->indexGamma, addedLoad->gamma);
        for( int i=0; i < 3; ++i ){
            setVarValue( row, m_dd->indexPsi0+i, addedLoad->psi(i) );
        }
        for( int i=m_dd->indexP0; i <= m_dd->indexLast; ++i ){
            if( (i-m_dd->indexP0) %2 == 0  )
                setVarNameUnitMeasure( row, m_dd->indexP0+i, addedLoad->P(i) );
            else
                setVarValue( row, m_dd->indexP0+i, addedLoad->P(i) );
        }
    }
}

void LoadComplTableModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->loadContainer.size())
        return;

    removeRowsPrivate( position, count );

    for( int i=0; i < count; ++i ){
        m_dd->removeLoad(position);
    }

}
