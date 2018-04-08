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

#include "materialmodel.h"

#include "tablemodelplusprivate.h"

#include "material.h"
#include "concrete.h"
#include "steelcncr.h"
#include "steel.h"
#include "timber.h"
#include "frp.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "doubleplus.h"

#include <QComboBox>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QList>
#include <QMap>
#include <QColor>
#include <QBrush>
#include <QVariant>

#include <QDebug>

typedef QList<Material *>::iterator MaterialIterator;

class MaterialModelPrivate{
public:
    MaterialModelPrivate( ):
        materialContainer(new QList<Material *>),
        materialMap(new QMap<unsigned int, Material * >){
    }
    Material * material( int p ){
        if( p >= 0 && p < materialContainer->size() ){
            return materialContainer->at(p);
        } else {
            return NULL;
        }
    }
    Material * materialId( unsigned int id ){
        if( materialMap->contains( id ) ){
            return materialMap->value( id, NULL );
        } else {
            return NULL;
        }
    }
    void insertMaterial( int pos, Material * m ){
        // l'id deve essere unico
        while( materialMap->contains(m->id->valueNormal()) ){
            m->nextId();
        }
        materialContainer->insert( pos, m );
        materialMap->insert( m->id->valueNormal(), m );
    }
    void removeMaterial( int pos ){
        materialMap->remove( materialContainer->at(pos)->id->valueNormal() );
        delete materialContainer->at(pos);
        materialContainer->removeAt(pos);
    }

    QList<Material *> * materialContainer;
    QMap<unsigned int, Material *> * materialMap;
};

MaterialModel::MaterialModel(UnitMeasure * ump, QObject *parent) :
    TableModelPlus("MaterialModel", ump, parent ),
    m_dd( new MaterialModelPrivate() ){
    QList<QString> headerList;
    headerList << trUtf8("Id")
               << trUtf8("Nome")
               << trUtf8("Tipo")
               << trUtf8("E") + " [" + m_d->unitMeasure->string(UnitMeasure::tension ) + "]"
               << trUtf8("G") + " [" + m_d->unitMeasure->string(UnitMeasure::tension ) + "]";
    m_d->setHeaders( headerList );
}

void MaterialModel::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement(m_d->modelName);
    for( MaterialIterator i = m_dd->materialContainer->begin(); i != m_dd->materialContainer->end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void MaterialModel::readXml(QXmlStreamReader *reader) {
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
        if( t == QXmlStreamReader::StartElement ){
            insertRows( n, reader->attributes() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void MaterialModel::insertRows(MaterialModel::MaterialType t, int position, int count ){
    for( int i=0; i < count; ++i){
        Material * addedMat = createMaterial( m_d->unitMeasure, t );
        if( addedMat ){
            insertMaterial( addedMat, position );
        }
    }
}

void MaterialModel::insertRows( QString matType, const QXmlStreamAttributes &attrs, int position , int count){
    for( int i=0; i < count; ++i){
        Material * addedMat = createMaterial( m_d->unitMeasure, matType, attrs );
        if( addedMat ){
            insertMaterial( addedMat, position );
        }
    }
}

void MaterialModel::insertMaterial( Material * addedMat, int position ){
    if (position < 0 || position > m_dd->materialContainer->size() )
        position = m_dd->materialContainer->size();
    if( addedMat ){
        m_dd->insertMaterial( position, addedMat );
        connect( addedMat, SIGNAL(materialChanged()), this, SIGNAL(modelChanged()) );
        insertRowsPrivate( position );
        setVarValueRow( position, addedMat->id, addedMat->name, addedMat->typeName, addedMat->E, addedMat->G );
        setVarReadOnly( position, 1, false );

        emit modelChanged();
    }
}

Material * MaterialModel::createMaterial( UnitMeasure * ump, MaterialModel::MaterialType type ){
    switch( type ){
    case ConcreteMaterial:
        return new Concrete( ump );
    case SteelCncrMaterial:
        return new SteelCncr(ump );
    case SteelMaterial:
        return new Steel( ump );
    case TimberMaterial:
        return new Timber( ump );
    case FRPMaterial:
        return new FRP( ump );
    default:
        Material * mat = new Material( ump, trUtf8("Materiale") );
        mat->E->setValueNormal( 2.1e+11);
        mat->nu->setValueNormal( 0.30 );
        mat->G->setValueNormal( mat->E->valueNormal() / (2.0 * (1.0 + mat->nu->valueNormal() )));
        return mat;
    }
}

Material *MaterialModel::createMaterial(UnitMeasure * ump, QString matType, const QXmlStreamAttributes &attrs) {
    matType = matType.toUpper();
    if( matType == "CONCRETE"){
        return new Concrete( ump, attrs );
    }
    if( matType == "STEEL"){
        return new Steel( ump, attrs );
    }
    if( matType == "STEELCNCR"){
        return new SteelCncr( ump, attrs );
    }
    if( matType == "TIMBER"){
        return new Timber( ump, attrs );
    }
    if( matType == "FRP"){
        return new FRP( ump, attrs );
    }
    if( matType == "MATERIAL"){
        return new Material( ump, attrs );
    }
    return NULL;
}

void MaterialModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->materialContainer->size())
        return;

    for (int row = position; row < (position+count); row++){
        if( m_dd->materialContainer->at(position)->isUsed()){
            QString title = trUtf8("Materiale in uso");
            QString message = trUtf8("Materiale usato da almeno una sezione.");
            qWarning() << title;
            qWarning() << message;
        } else {
            // scolleghiamo il materiale
            disconnect( m_dd->materialContainer->at(position), SIGNAL(materialChanged()), this, SIGNAL(modelChanged()) );
            // rimuoviamo la riga
            removeRowsPrivate( position );
            // ... e il materiale
            m_dd->removeMaterial( position );
        }
    }
    emit modelChanged();
}

Material * MaterialModel::material( int i ){
    return m_dd->material( i );
}

Material *MaterialModel::material(int i ) const {
    return m_dd->material( i );
}

Material * MaterialModel::materialId( unsigned int i){
    return m_dd->materialId( i );
}

Material *MaterialModel::firstMaterial(MaterialModel::MaterialType mt) {
    if( m_dd->materialContainer->size() > 0 ){
        if( mt == GenericMaterial ){
            return m_dd->materialContainer->at(0);
        } else {
            for( MaterialIterator i = m_dd->materialContainer->begin(); i != m_dd->materialContainer->end(); ++i ){
                if( (*i)->materialType() == mt ){
                    return *i;
                }
            }
        }
    }
    return NULL;
}

int MaterialModel::count(MaterialType mt){
    if( mt == GenericMaterial ){
        return m_dd->materialContainer->size();
    } else {
        int ret = 0;
        for( MaterialIterator i = m_dd->materialContainer->begin(); i != m_dd->materialContainer->end(); ++i ){
            if( (*i)->materialType() == mt ){
                ++ret;
            }
        }
        return ret;
    }
}

QList<Material *> MaterialModel::materialsList(MaterialModel::MaterialType mt) {
    QList< Material * > ret;
    for( QList<Material *>::iterator i=m_dd->materialContainer->begin(); i != m_dd->materialContainer->end(); ++i ){
        if( ((*i)->materialType() == mt) || (mt == GenericMaterial) ){
            ret.append( *i );
        }
    }
    return ret;
}
