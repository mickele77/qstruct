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
#include "sectiontimberdatasetmodel.h"

#include "tablemodelplusprivate.h"

#include "sectiontimber.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "boolplus.h"
#include "point2dplus.h"
#include "unitmeasure.h"

#include <QList>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QColor>

SectionTimberData::SectionTimberData( UnitMeasure * ump ):
    VarPlusContainer( ump, "SectionTimberData", "") {
    initVar( ump );
}

SectionTimberData::~SectionTimberData(){
}

void SectionTimberData::initVar( UnitMeasure *  ump ) {
    IdBeam = new QStringPlus( "", "IdBeam" );
    addVarToContainer( IdBeam );

    IdSection = new QStringPlus( "", "IdSection" );
    addVarToContainer( IdSection );

    N = new DoublePlus(0.0, "N", ump, UnitMeasure::loadF );
    addVarToContainer( N );

    Vz = new DoublePlus(0.0, "Vz", ump, UnitMeasure::loadF );
    addVarToContainer( Vz );

    Vy = new DoublePlus(0.0, "Vy", ump, UnitMeasure::loadF );
    addVarToContainer( Vy );

    cenM = new Point2DPlus("cenM", "yM", 0.0, "zM", 0.0, ump, UnitMeasure::sectL);
    addVarToContainer( cenM );

    My = new DoublePlus(0.0, "My", ump, UnitMeasure::loadM );
    addVarToContainer( My );

    Mz = new DoublePlus(0.0, "Mz", ump, UnitMeasure::loadM );
    addVarToContainer( Mz );

    Mx = new DoublePlus(0.0, "My", ump, UnitMeasure::loadM );
    addVarToContainer( Mx );

    result = new DoublePlus(1.0, "result", ump, UnitMeasure::noDimension );
    addVarToContainer( result );

    verified = new BoolPlus(true, "verified", true );
    addVarToContainer( verified );
}

SectionTimberDataSetModel::SectionTimberDataSetModel(UnitMeasure * ump,
                                                     LSType * lsType,
                                                     ServiceClass * sClass,
                                                     LoadDurationClass * ldClass,
                                                     DoublePlus * kCritNy,
                                                     DoublePlus * kCritNz,
                                                     DoublePlus * kCritMy,
                                                     DoublePlus * kCritMz,
                                                     SectionTimber * sect ) :
    TableModelPlus( "SectionTimberDataSetModel", ump, sect ),
    m_section(sect),
    m_dataContainer( new QList<SectionTimberData *>),
    isCentMMOrigin(true),
    m_lsType(lsType),
    m_sClass( sClass),
    m_ldClass(ldClass),
    m_kCritNy( kCritNy),
    m_kCritNz( kCritNz),
    m_kCritMy( kCritMy),
    m_kCritMz( kCritMz){
    updateHeaders();
    connect(ump, SIGNAL(stringsChanged(UnitMeasure::unitMeasure)), this, SLOT(updateHeaders()) );
}

void SectionTimberDataSetModel::writeXml(QXmlStreamWriter *writer){
    Q_UNUSED(writer);
    // TODO
}

void SectionTimberDataSetModel::readXml(QXmlStreamReader *reader) {
    Q_UNUSED( reader );
    // TODO
}

void SectionTimberDataSetModel::loadFromFile( QFile & file ){
    // removeRows()

    // if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    //    return;

    QTextStream in( &file );

    // legge prima riga con intestazioni
    QStringList colList;
    if( !in.atEnd()){
        QString line = in.readLine();
        QStringList list = line.split( '\t', QString::SkipEmptyParts );
        for( int i=0; i < list.size(); i++ ){
            colList.append( list.at(i).toUpper() );
        }
    }

    // legge seconda riga con conversioni unità  di misura
    QList<double> UMList;
    if( !in.atEnd()){
        QString line = in.readLine();
        QStringList list = line.split( '\t', QString::SkipEmptyParts );
        for( int i=0; i < list.size(); i++ ){
            UMList.append( list.at(i).toDouble() );
        }
    }

    if( colList.size() <= UMList.size() ){
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList list = line.split( '\t', QString::SkipEmptyParts );
            appendRows();
            SectionTimberData * data = m_dataContainer->last();
            for( int i=0; i < list.size(); i++ ){
                if( colList.at(i).toUpper() == "IDBEAM" ){
                    data->IdBeam->setValueNormal( list.at(i) );
                } else if( colList.at(i).toUpper() == "IDSECTION" ){
                    data->IdSection->setValueNormal( list.at(i) );
                } else if( colList.at(i).toUpper() == "N" ){
                    data->N->setValueNormal( list.at(i).toDouble() * UMList.at(i));
                } else if( colList.at(i).toUpper() == "VY" ){
                    data->Vy->setValueNormal( list.at(i).toDouble() * UMList.at(i));
                } else if( colList.at(i).toUpper() == "VZ" ){
                    data->Vz->setValueNormal( list.at(i).toDouble() * UMList.at(i));
                } else if( colList.at(i).toUpper() == "MY" ){
                    data->My->setValueNormal( list.at(i).toDouble() * UMList.at(i));
                } else if( colList.at(i).toUpper() == "MZ" ){
                    data->Mz->setValueNormal( list.at(i).toDouble() * UMList.at(i));
                } else if( colList.at(i).toUpper() == "MX" ){
                    data->Mx->setValueNormal( list.at(i).toDouble() * UMList.at(i));
                }
            }
        }
    }
}

void SectionTimberDataSetModel::verifyULS( int row ){
    if( m_section ){
        m_dataContainer->at(row)->verified->setValueNormal( m_section->verifyULS( m_lsType,
                                                                                  m_sClass,
                                                                                  m_ldClass,
                                                                                  m_kCritNy,
                                                                                  m_kCritNz,
                                                                                  m_dataContainer->at(row)->N,
                                                                                  m_dataContainer->at(row)->cenM,
                                                                                  m_dataContainer->at(row)->Mx,
                                                                                  m_kCritMy,
                                                                                  m_dataContainer->at(row)->My,
                                                                                  m_kCritMz,
                                                                                  m_dataContainer->at(row)->Mz,
                                                                                  m_dataContainer->at(row)->Vy,
                                                                                  m_dataContainer->at(row)->Vz,
                                                                                  m_dataContainer->at(row)->result) );
    }
}

void SectionTimberDataSetModel::verifyULSAll(){
    for( int i=0; i < m_dataContainer->size(); i++ ){
        verifyULS( i );
    }
}

void SectionTimberDataSetModel::setSection( SectionTimber * sect ){
    m_section = sect;
    verifyULSAll();
}

void SectionTimberDataSetModel::updateHeaders(){
    QList<QString> headers;
    headers << trUtf8("Trave")
            << trUtf8("Sezione")
            << trUtf8("N") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadF) + QString("]")
            << trUtf8("Tz") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadF) + QString("]")
            << trUtf8("Ty") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadF) + QString("]")
            << trUtf8("yM") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::sectL) + QString("]")
            << trUtf8("zM") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::sectL) + QString("]")
            << trUtf8("My")  + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]")
            << trUtf8("Mz")  + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]")
            << trUtf8("Mx") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]")
            << trUtf8("Risultato") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::noDimension) + QString("]");
    setHeaders( headers );
}

void SectionTimberDataSetModel::insertRows( int position, int count){
    if (position < 0 || position > m_dataContainer->size() )
        position = m_dataContainer->size();

    for( int i=position; i < (position+count); i++){
        insertData( new SectionTimberData( m_d->unitMeasure ), i );
    }
}

void SectionTimberDataSetModel::insertData( SectionTimberData * data, int position ){
    if( data ){
        m_dataContainer->insert( position, new SectionTimberData( m_d->unitMeasure ) );

        insertRowsPrivate( position );
        setVarValueRow( position,
                        data->IdBeam,
                        data->IdSection,
                        data->N,
                        data->Vz,
                        data->Vy,
                        data->cenM->y,
                        data->cenM->z,
                        data->My,
                        data->Mz,
                        data->Mx,
                        data->result );
        emit modelChanged();
    }
}

void SectionTimberDataSetModel::appendRows( int count ){
    insertRows( m_dataContainer->size(), count );
}

void SectionTimberDataSetModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dataContainer->size())
        return;

    for (int row = position; row < (position+count); row++){
        removeRowsPrivate( position );
        delete m_dataContainer->at( position );
        m_dataContainer->removeAt( position );
    }
    emit modelChanged();
}

int SectionTimberDataSetModel::dataCount() {
    return m_dataContainer->size();
}

SectionTimberData * SectionTimberDataSetModel::data( int i ){
    if( i >= 0 && i < m_dataContainer->size() ){
        return m_dataContainer->at(i);
    }
    return NULL;
}
