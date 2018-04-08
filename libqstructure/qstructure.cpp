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
#include "qstructure.h"

#include "soilsheetmodel.h"
#include "retainingwallmodel.h"
#include "soilmodel.h"
#include "timberconnectionmodel.h"
#include "sectionmodel.h"
#include "materialmodel.h"
#include "unitmeasure.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QList>

class QStructurePrivate{
public:
    QStructurePrivate( QStructure * parent ){
        unitMeasure = new UnitMeasure( parent );
        materialModel = new MaterialModel( unitMeasure, parent );
        addModel( materialModel, parent );
        sectionModel = new SectionModel( unitMeasure, materialModel, parent );
        addModel( sectionModel, parent );
        timberConnectionModel = new TimberConnectionModel( unitMeasure, materialModel, parent );
        addModel( timberConnectionModel, parent );
        soilModel = new SoilModel( unitMeasure, parent );
        addModel( soilModel, parent );
        retainingWallModel = new RetainingWallModel( unitMeasure, soilModel, parent );
        addModel( retainingWallModel, parent );
        soilSheetModel = new SoilSheetModel( unitMeasure, soilModel, parent );
        addModel( soilSheetModel, parent );
    }
    void addModel( TableModelPlus * m, QStructure * str ){
        models.append( m );
        QObject::connect( m, SIGNAL(modelChanged()), str, SIGNAL(structureChanged()) );
    }
    TableModelPlus * model( QString mName  ){
        for( QList<TableModelPlus *>::iterator i=models.begin(); i != models.end(); ++i){
            if( (*i)->modelName().toUpper() == mName.toUpper() ){
                return *i;
            }
        }
        return NULL;
    }

    // Modello
    /** Puntatore alla classe UnitMeasure, contenente le unità di misura */
    UnitMeasure * unitMeasure;
    /** Elenco dei modelli contenuti nella struttura */
    QList<TableModelPlus *> models;

    /** Puntatore alla classe MaterialModel, contenente i materiali */
    MaterialModel * materialModel;
    /** Puntatore alla classe SectionModel, contenente le sezioni */
    SectionModel * sectionModel;
    /** Puntatore alla classe TimberConnectionModel, contenente le connessioni in legno */
    TimberConnectionModel * timberConnectionModel;
    /** Puntatore alla classe SoilModel, contenente i terreni */
    SoilModel * soilModel;
    /** Puntatore alla classe RetainingWallModel, contenente i muri di sostegno */
    RetainingWallModel * retainingWallModel;
    /** Puntatore alla classe SoilSheetModel, contenente le paratie */
    SoilSheetModel * soilSheetModel;
};

QStructure::QStructure(QObject *parent) :
    QObject(parent),
    m_d( new QStructurePrivate(this)) {
}

QString QStructure::name() {
    return QString("QStructure");
}

UnitMeasure *QStructure::unitMeasure() {
    return m_d->unitMeasure;
}

MaterialModel *QStructure::materialModel() {
    return m_d->materialModel;
}

SectionModel *QStructure::sectionModel() {
    return m_d->sectionModel;
}

TimberConnectionModel *QStructure::timberConnectionModel() {
    return m_d->timberConnectionModel;
}

SoilModel *QStructure::soilModel() {
    return m_d->soilModel;
}

RetainingWallModel *QStructure::retainingWallModel() {
    return m_d->retainingWallModel;
}

SoilSheetModel *QStructure::soilSheetModel() {
    return m_d->soilSheetModel;
}

void QStructure::clear() {
    QList<TableModelPlus *>::iterator i = m_d->models.end();
    while( i != m_d->models.begin() ){
        --i;
        (*i)->clear();
    }
}

void QStructure::writeXml(QXmlStreamWriter * writer) {
    writer->setAutoFormatting(true);
    writer->setCodec("UTF-8");

    writer->writeStartDocument();
    writer->writeStartElement(name());
    writer->writeAttribute("version", "0.1");
    m_d->unitMeasure->writeXml( writer );
    for( QList<TableModelPlus *>::iterator i = m_d->models.begin(); i != m_d->models.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
    writer->writeEndDocument();
}

void QStructure::readXml(QXmlStreamReader *reader) {
    while( (reader->name().toString() != name() ) &
           (!reader->atEnd()) &&
           (!reader->hasError())){
        reader->readNext();
    }
    if( reader->attributes().hasAttribute("version") ){
        double v = reader->attributes().value( "version").toString().toDouble();
        if( v == 0.1 ){
            while( (!reader->atEnd()) &&
                   (!reader->hasError())){
                reader->readNext();
                TableModelPlus * m = m_d->model( reader->name().toString() );
                if( m ){
                    m->readXml( reader );
                }
            }
        }
    }
}
