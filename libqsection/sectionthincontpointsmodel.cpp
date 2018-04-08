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
#include "sectionthincontpointsmodel.h"
#include "tablemodelplusprivate.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "point2dplus.h"
#include "doubleplus.h"

class Point2DPlus;
class DoublePlus;

class SectionThinContPointsData{
public:
    SectionThinContPointsData( UnitMeasure * ump):
        P(new Point2DPlus("P", "y", 0.0, "z", 0.0, ump, UnitMeasure::sectL )),
        r(new DoublePlus( 0.0, "r", ump, UnitMeasure::sectL )){
    }
    ~SectionThinContPointsData(){
        VarPlus::deleteVarPlus( P );
        VarPlus::deleteVarPlus( r );
    }
    SectionThinContPointsData & operator=( SectionThinContPointsData & val){
        if( this != &val ){
            *P = *(val.P);
            *r = *(val.r);
        }
        return *this;
    }

    void writeXml(QXmlStreamWriter *writer){
        writer->writeStartElement( "SectionThinContPointsData" );
        writer->writeAttribute( P->y->xmlAttribute() );
        writer->writeAttribute( P->z->xmlAttribute() );
        writer->writeAttribute( r->xmlAttribute() );
        writer->writeEndElement();
    }

    void loadFromXML(const QXmlStreamAttributes &attrs){
        for ( int i=0; i<attrs.count(); i++ ) {
            if( attrs.at(i).name().toString().toUpper() == P->y->name().toUpper() ){
                P->y->setValueNormal( attrs.at(i).value().toString() );
            } else if( attrs.at(i).name().toString().toUpper() == P->z->name().toUpper() ){
                P->z->setValueNormal( attrs.at(i).value().toString() );
            } else if( attrs.at(i).name().toString().toUpper() == "R" ){
                r->setValueNormal( attrs.at(i).value().toString() );
            }
        }
    }

    Point2DPlus * P;
    DoublePlus * r;
};

class SectionThinContPointsModelPrivate{
public:
    SectionThinContPointsModelPrivate(){
    };

    QList<SectionThinContPointsData *> dataList;
};

SectionThinContPointsModel::SectionThinContPointsModel( UnitMeasure * ump, QObject *parent ):
    TableModelPlus( "SectionThinContPointsModel", ump, parent),
    m_dd( new SectionThinContPointsModelPrivate() ){
    connect( m_d->unitMeasure, SIGNAL(stringsChanged(UnitMeasure::unitMeasure)), this, SLOT(updateHeaders()) );
    updateHeaders();
}

SectionThinContPointsModel::~SectionThinContPointsModel(){
}

SectionThinContPointsModel & SectionThinContPointsModel::operator=( SectionThinContPointsModel & val){
    if( this != &val ){
        removeRows( 0, m_dd->dataList.size() );
        insertRows( 0, val.m_dd->dataList.size() );

        QList<SectionThinContPointsData *>::iterator iter = m_dd->dataList.begin() ;
        QList<SectionThinContPointsData *>::iterator iterVal = val.m_dd->dataList.begin() ;
        while( (iter != m_dd->dataList.end()) && (iterVal != val.m_dd->dataList.end())){
            **iter = **iterVal;
            ++iter;
            ++iterVal;
        }
    }
    return *this;
}

void SectionThinContPointsModel::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement( m_d->modelName );
    for( QList<SectionThinContPointsData *>::iterator i = m_dd->dataList.begin(); i != m_dd->dataList.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void SectionThinContPointsModel::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    QString myName = m_d->modelName.toUpper();

    while( !(n == myName && t == QXmlStreamReader::StartElement) &&
           !(reader->hasError()) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }

    while( !(n == myName && t == QXmlStreamReader::EndElement)  &&
           !(reader->hasError()) &&
           (t != QXmlStreamReader::EndDocument) ){
        if( n == "SECTIONTHINCONTPOINTSDATA" && t == QXmlStreamReader::StartElement ) {
            insertRows( m_dd->dataList.size() );
            m_dd->dataList.last()->loadFromXML( reader->attributes() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void SectionThinContPointsModel::insertRows(int pos, int count) {
    if (pos < 0 || pos > m_dd->dataList.size() )
        return;
    for( int i=0; i < count; ++i){
        SectionThinContPointsData * d = new SectionThinContPointsData( m_d->unitMeasure);
        m_dd->dataList.insert( pos, d );
        insertRowsPrivate( pos );
        setVarValueRow( pos, d->P->y, d->P->z, d->r );
    }
    emit modelChanged();
}

void SectionThinContPointsModel::removeRows(int pos, int count) {
    if (pos < 0 || (pos + count) > m_dd->dataList.size())
        return;

    for (int row = pos; row < (pos+count); row++){
        // ok, procediamo a cancellare la riga
        removeRowsPrivate( pos );
        // e la sezione
        delete m_dd->dataList.at(pos);
        m_dd->dataList.removeAt( pos );
    }
    emit modelChanged();
}

int SectionThinContPointsModel::size() {
    return m_dd->dataList.size();
}

Point2DPlus *SectionThinContPointsModel::P(int i) {
    if( i < 0 || i >= m_dd->dataList.size() ){
        return NULL;
    }
    return m_dd->dataList.at(i)->P;
}

DoublePlus *SectionThinContPointsModel::r(int i) {
    if( i < 0 || i >= m_dd->dataList.size() ){
        return NULL;
    }
    return m_dd->dataList.at(i)->r;
}

void SectionThinContPointsModel::updateHeaders(){
    QList<QString> headerList;
    headerList << trUtf8("y [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL ) )
               << trUtf8("z [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL ) )
               << trUtf8("r [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL ) );
    setHeaders( headerList );
}


