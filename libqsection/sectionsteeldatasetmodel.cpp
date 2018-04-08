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
#include "sectionsteeldatasetmodel.h"

#include "sectionsteel.h"
#include "sectionsteelvar.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "point2dplus.h"
#include "unitmeasure.h"
#include "boolplus.h"

#include <QList>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QColor>

SectionSteelData::SectionSteelData( UnitMeasure * ump ):
    VarPlusContainer( ump, "SectionSteelData", "") {
    initVar();
}

SectionSteelData::~SectionSteelData(){
}

void SectionSteelData::initVar() {
    IdBeam = new QStringPlus( "", "IdBeam" );
    addVarToContainer( IdBeam );

    IdSection = new QStringPlus( "", "IdSection" );
    addVarToContainer( IdSection );

    N = new DoublePlus(0.0, "N", m_unitMeasure, UnitMeasure::loadF );
    addVarToContainer( N );

    Vz = new DoublePlus(0.0, "Vz", m_unitMeasure, UnitMeasure::loadF );
    addVarToContainer( Vz );

    Vy = new DoublePlus(0.0, "Vz", m_unitMeasure, UnitMeasure::loadF );
    addVarToContainer( Vy );

    cenM = new Point2DPlus("cenM", "yM", 0.0, "zM", 0.0, m_unitMeasure, UnitMeasure::sectL);
    addVarToContainer( cenM );

    My = new DoublePlus(0.0, "My", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( My );

    Mz = new DoublePlus(0.0, "Mz", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( Mz );

    Mx = new DoublePlus(0.0, "My", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( Mx );

    beamMyDistribution = new SectionSteelMomentDistribution( SectionSteel::momentLinear, "beamMyDistribution");
    addVarToContainer( beamMyDistribution );

    beamMyH1 = new DoublePlus(0.0, "beamMyH1", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( beamMyH1 );

    beamMyS = new DoublePlus(0.0, "beamMyS", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( beamMyS );

    beamMyH2 = new DoublePlus(0.0, "beamMyH2", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( beamMyH2 );

    beamMzDistribution = new SectionSteelMomentDistribution( SectionSteel::momentLinear, "beamMzDistribution");
    addVarToContainer( beamMzDistribution );

    beamMzH1 = new DoublePlus(0.0, "beamMzH1", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( beamMzH1 );

    beamMzS = new DoublePlus(0.0, "beamMzS", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( beamMzS );

    beamMzH2 = new DoublePlus(0.0, "beamMzH2", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( beamMzH2 );

    beamMLTDistribution = new SectionSteelMomentDistribution( SectionSteel::momentLinear, "beamMLTDistribution");
    addVarToContainer( beamMLTDistribution );

    beamMLTH1 = new DoublePlus(0.0, "beamMLTH1", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( beamMLTH1 );

    beamMLTS = new DoublePlus(0.0, "beamMLTS", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( beamMLTS );

    beamMLTH2 = new DoublePlus(0.0, "beamMLTH2", m_unitMeasure, UnitMeasure::loadM );
    addVarToContainer( beamMLTH2 );

    beamMCr = new DoublePlus(0.0, "beamMCr", m_unitMeasure, UnitMeasure::loadM, true );
    addVarToContainer( beamMCr );

    beamlRelLT = new DoublePlus(0.0, "beamlRelLT", m_unitMeasure, UnitMeasure::noDimension, true );
    addVarToContainer( beamlRelLT );

    beamChiLT = new DoublePlus(0.0, "beamChiLT", m_unitMeasure, UnitMeasure::noDimension, true );
    addVarToContainer( beamChiLT );

    result = new DoublePlus(1.0, "result", m_unitMeasure, UnitMeasure::noDimension, true );
    addVarToContainer( result );

    verified = new BoolPlus( true, "verified", true );
    addVarToContainer( verified );
}

#include "tablemodelplusprivate.h"

SectionSteelDataSetModel::SectionSteelDataSetModel(UnitMeasure * ump,
                                                   DoublePlus * lRelY,
                                                   DoublePlus * chiY,
                                                   DoublePlus * lRelZ,
                                                   DoublePlus * chiZ,
                                                   SectionSteelTorsionalDeformability * torsDef,
                                                   DoublePlus * L0LT,
                                                   SectionSteel * sect ) :
    TableModelPlus( "SectionSteelDataSetModel", ump, sect ),
    m_section(sect),
    m_dataContainer( new QList<SectionSteelData *>),
    m_lRelY(lRelY ),
    m_chiY(chiY),
    m_lRelZ(lRelZ ),
    m_chiZ(chiZ),
    m_torsDef(torsDef),
    m_L0LT(L0LT){
    updateHeaders();
    connect( ump, SIGNAL(stringsChanged(UnitMeasure::unitMeasure)), this, SLOT(updateHeaders()) );
}

void SectionSteelDataSetModel::writeXml( QXmlStreamWriter * writer ){
    writer->setAutoFormatting(true);
    writer->setCodec("UTF-8");

    writer->writeStartDocument();
    writer->writeStartElement( "SectionSteelDataSet" );
    for( QList<SectionSteelData *>::iterator iter = m_dataContainer->begin(); iter != m_dataContainer->end(); ++iter){
        (*iter)->writeXml( writer);
    }
    writer->writeEndElement();
    writer->writeEndDocument();
}

void SectionSteelDataSetModel::readXml(QXmlStreamReader * reader){
    // pulisce il modello
    removeRows( 0, m_dataContainer->size() );

    // scansione il file XML fino a trovare il primo elemento SectionSteelDataSet
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    while( !((t == QXmlStreamReader::StartElement) && (n == "SECTIONSTEELDATASET"))  &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }

    // Continua a scansionare il file XML fino alla fine dell'elemento SectionSteelDataSet o alla fine del file
    while( !(t == QXmlStreamReader::EndElement && (n == "SECTIONSTEELDATASET")) &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        if( t == QXmlStreamReader::StartElement && (n == "SECTIONSTEELDATA") ){
            appendRows();
            m_dataContainer->last()->loadFromXML( reader->attributes() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void SectionSteelDataSetModel::loadFromFile( QFile & file ){
    // removeRows()

    // if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    //    return;

    QTextStream in( &file );

    // legge prima riga con intestazioni
    QStringList headersList;
    if( !in.atEnd()){
        QString line = in.readLine();
        QStringList list = line.split( '\t', QString::SkipEmptyParts );
        for( int i=0; i < list.size(); i++ ){
            headersList.append( list.at(i).toUpper() );
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

    int nColHUM = qMin( UMList.size(), headersList.size() );

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList dataList = line.split( '\t', QString::SkipEmptyParts );
        if( dataList.size() > 0 ){
            appendRows();
            SectionSteelData * data = m_dataContainer->last();
            int nCol = qMin( dataList.size(), nColHUM );
            for( int i=0; i < nCol; i++ ){
                if( headersList.at(i).toUpper() == "IDBEAM" ){
                    data->IdBeam->setValueNormal( dataList.at(i) );
                } else if( headersList.at(i).toUpper() == "IDSECTION" ){
                    data->IdSection->setValueNormal( dataList.at(i) );
                } else if( headersList.at(i).toUpper() == "N" ){
                    data->N->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "VY" ){
                    data->Vy->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "VZ" ){
                    data->Vz->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MY" ){
                    data->My->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MZ" ){
                    data->Mz->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MX" ){
                    data->Mx->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MYDIST" ){
                    data->beamMyDistribution->setValueInt( dataList.at(i).toInt()  );
                } else if( headersList.at(i).toUpper() == "MYH1" ){
                    data->beamMyH1->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MYS" ){
                    data->beamMyS->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MYH2" ){
                    data->beamMzH2->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MZDIST" ){
                    data->beamMzDistribution->setValueInt( dataList.at(i).toInt() );
                } else if( headersList.at(i).toUpper() == "MZH1" ){
                    data->beamMzH1->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MZS" ){
                    data->beamMzS->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MZH2" ){
                    data->beamMzH2->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MLTDIST" ){
                    data->beamMLTDistribution->setValueInt( dataList.at(i).toInt() );
                } else if( headersList.at(i).toUpper() == "MLTH1" ){
                    data->beamMLTH1->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MLTS" ){
                    data->beamMLTS->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MLTH2" ){
                    data->beamMLTH2->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MCR" ){
                    data->beamMCr->setReadOnly( false );
                    data->beamMCr->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "LLT" ){
                    data->beamlRelLT->setReadOnly( false );
                    data->beamlRelLT->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "CHILT" ){
                    data->beamChiLT->setReadOnly( false );
                    data->beamChiLT->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                }
            }
        }
    }
}

void SectionSteelDataSetModel::verifyULS( int row ){
    if( m_section ){
        if( m_dataContainer->at(row)->beamMCr->readOnly() ){
            m_section->beamMCrLT( m_dataContainer->at(row)->beamMCr, m_L0LT, m_dataContainer->at(row)->beamMLTH1, m_dataContainer->at(row)->beamMLTH2);
        }
        if( m_dataContainer->at(row)->beamlRelLT ){
            m_section->beamlRelBucklLT( m_dataContainer->at(row)->beamlRelLT, m_dataContainer->at(row)->beamMCr);
        }
        if( m_dataContainer->at(row)->beamChiLT ){
            m_section->beamChiBucklLT(m_dataContainer->at(row)->beamChiLT, m_dataContainer->at(row)->beamMLTH1, m_dataContainer->at(row)->beamMLTH2, m_dataContainer->at(row)->beamMLTDistribution, m_dataContainer->at(row)->beamlRelLT );
        }
        m_dataContainer->at(row)->verified->setValueNormal( m_section->verifyULS( m_lRelY,
                                                                                  m_chiY,
                                                                                  m_lRelZ,
                                                                                  m_chiZ,
                                                                                  m_dataContainer->at(row)->N,
                                                                                  m_torsDef,
                                                                                  m_dataContainer->at(row)->beamChiLT,
                                                                                  m_dataContainer->at(row)->beamMyDistribution,
                                                                                  m_dataContainer->at(row)->beamMyH1,
                                                                                  m_dataContainer->at(row)->beamMyS,
                                                                                  m_dataContainer->at(row)->beamMyH2,
                                                                                  m_dataContainer->at(row)->beamMzDistribution,
                                                                                  m_dataContainer->at(row)->beamMzH1,
                                                                                  m_dataContainer->at(row)->beamMzS,
                                                                                  m_dataContainer->at(row)->beamMzH2,
                                                                                  m_dataContainer->at(row)->beamMLTDistribution,
                                                                                  m_dataContainer->at(row)->beamMLTH1,
                                                                                  m_dataContainer->at(row)->beamMLTS,
                                                                                  m_dataContainer->at(row)->beamMLTH2,
                                                                                  m_dataContainer->at(row)->cenM,
                                                                                  m_dataContainer->at(row)->My,
                                                                                  m_dataContainer->at(row)->Mz,
                                                                                  m_dataContainer->at(row)->Vy,
                                                                                  m_dataContainer->at(row)->Vz,
                                                                                  m_dataContainer->at(row)->result) );
    }
}

void SectionSteelDataSetModel::verifyULSAll(){
    for( int i=0; i < m_dataContainer->size(); i++ ){
        verifyULS( i );
    }
}

void SectionSteelDataSetModel::setSection( SectionSteel * sect ){
    m_section = sect;
    verifyULSAll();
}

void SectionSteelDataSetModel::updateHeaders(){
    QList<QString> headers;
    headers << trUtf8("Trave");
    headers << trUtf8("Sezione");
    headers << trUtf8("N") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadF) + QString("]");
    headers << trUtf8("Tz") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadF) + QString("]");
    headers << trUtf8("Ty") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadF) + QString("]");
    headers << trUtf8("yM") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::sectL) + QString("]");
    headers << trUtf8("zM") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::sectL) + QString("]");
    headers << trUtf8("My")  + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("Mz")  + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("Mx") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MyDist");
    headers << trUtf8("MyH1") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MyS") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MyH2") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MzDist");
    headers << trUtf8("MzH1") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MzS") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MzH2") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MLTDist");
    headers << trUtf8("MLTH1") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MLTS") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MLTH2") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("MCr") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::loadM) + QString("]");
    headers << trUtf8("lLT") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::noDimension) + QString("]");
    headers << trUtf8("ChiLT") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::noDimension) + QString("]");
    headers << trUtf8("Risultato") + QString(" [") + m_d->unitMeasure->string( UnitMeasure::noDimension) + QString("]");
    setHeaders( headers );
}

void SectionSteelDataSetModel::insertRows( int position, int count){
    if (position < 0 || position > m_dataContainer->size() )
        position = m_dataContainer->size();

    for( int i=position; i < (position+count); i++){
        insertData( new SectionSteelData( m_d->unitMeasure ), i );
    }
}

void SectionSteelDataSetModel::insertData( SectionSteelData * data, int position ){
    if( data ){
        m_dataContainer->insert( position, data );

        insertRowsPrivate( position );
        setVarValueRow( position,
                        m_dataContainer->at( position )->IdBeam,
                        m_dataContainer->at( position )->IdSection,
                        m_dataContainer->at( position )->N,
                        m_dataContainer->at( position )->Vz,
                        m_dataContainer->at( position )->Vy,
                        m_dataContainer->at( position )->cenM->y,
                        m_dataContainer->at( position )->cenM->z,
                        m_dataContainer->at( position )->My,
                        m_dataContainer->at( position )->Mz,
                        m_dataContainer->at( position )->Mx,
                        m_dataContainer->at( position )->beamMyDistribution,
                        m_dataContainer->at( position )->beamMyH1,
                        m_dataContainer->at( position )->beamMyS,
                        m_dataContainer->at( position )->beamMyH2,
                        m_dataContainer->at( position )->beamMzDistribution,
                        m_dataContainer->at( position )->beamMzH1,
                        m_dataContainer->at( position )->beamMzS,
                        m_dataContainer->at( position )->beamMzH2,
                        m_dataContainer->at( position )->beamMLTDistribution,
                        m_dataContainer->at( position )->beamMLTH1,
                        m_dataContainer->at( position )->beamMLTS,
                        m_dataContainer->at( position )->beamMLTH2,
                        m_dataContainer->at( position )->beamMCr,
                        m_dataContainer->at( position )->beamlRelLT,
                        m_dataContainer->at( position )->beamChiLT,
                        m_dataContainer->at( position )->result );
        emit modelChanged();
    }
}

void  SectionSteelDataSetModel::appendRows( int count ){
    insertRows( m_dataContainer->size(), count );
}

void SectionSteelDataSetModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dataContainer->size())
        return;

    for (int row = position; row < (position+count); row++){
        removeRowsPrivate( position );
        delete m_dataContainer->at( position );
        m_dataContainer->removeAt( position );
    }
    emit modelChanged();
}

int SectionSteelDataSetModel::dataCount() {
    return m_dataContainer->size();
}

SectionSteelData * SectionSteelDataSetModel::data( int i ){
    if( i >= 0 && i < m_dataContainer->size() ){
        return m_dataContainer->at(i);
    }
    return NULL;
}
