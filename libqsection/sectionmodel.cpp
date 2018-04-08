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
#include "sectionmodel.h"

#include "tablemodelplusprivate.h"

#include "sectionrcncr.h"
#include "sectioncncr.h"
#include "sectionsteelcncr.h"
#include "sectionsteel.h"
#include "sectiontimber.h"
#include "sectionpoints.h"
#include "sectionthincontsteel.h"
#include "sectionthincont.h"
#include "sectionthin.h"
#include "sectionline.h"
#include "sectionarc.h"
#include "sectionprofile.h"
#include "sectionfrp.h"
#include "section.h"

#include "materialmodel.h"
#include "concrete.h"
#include "steel.h"
#include "steelcncr.h"
#include "timber.h"
#include "material.h"

#include "point2dplusmodel.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "doubleplus.h"

#include <QXmlStreamReader>
#include <QList>
#include <QMap>

#include <QDebug>

typedef QList<Section *>::iterator SectionIterator;

class SectionModelPrivate{
public:
    SectionModelPrivate( MaterialModel * matModel ):
        materialColumn(3),
        materialModel(matModel) {
    };
    Section * section( int p ){
        if( p >= 0 && p < sectionContainer.size() ){
            return sectionContainer.at(p);
        } else {
            return NULL;
        }
    }
    Section * sectionId( unsigned int id ){
        if( sectionMap.contains( id ) ){
            return sectionMap.value( id, NULL );
        } else {
            return NULL;
        }
    }
    void insertSection( Section * s, int p ){
        // l'id deve essere unico
        while( sectionMap.contains(s->id->valueNormal()) ){
            s->nextId();
        }
        sectionContainer.insert( p, s );
        sectionMap.insert( s->id->valueNormal(), s );
    }
    void removeSectionRow( int p ){
        sectionMap.remove( sectionContainer.at(p)->id->valueNormal() );
        delete sectionContainer.takeAt(p);
    }
    int materialColumn;
    QList<Section *> sectionContainer;
    QMap<unsigned int, Section *> sectionMap;
    MaterialModel * materialModel;
};

SectionModel::SectionModel(UnitMeasure * ump, MaterialModel * matModel, QObject *parent) :
    TableModelPlus( "SectionModel", ump, parent ),
    m_dd( new SectionModelPrivate( matModel )) {
    connect( m_d->unitMeasure, SIGNAL(stringsChanged(UnitMeasure::unitMeasure)), this, SLOT(updateHeadersUM(UnitMeasure::unitMeasure)) );
    updateHeaders();
}

int SectionModel::materialColumn() {
    return 3;
}

void SectionModel::updateHeaders(){
    QList<QString> headerList;
    headerList << trUtf8("Nome")
               << trUtf8("Tipo")
               << trUtf8("A [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL2 ) )
               << trUtf8("Materiale");
    setHeaders( headerList );
}

void SectionModel::updateHeadersUM( UnitMeasure::unitMeasure u ){
    if( u == UnitMeasure::sectL2 ){
        QList<QString> headerList;
        headerList << trUtf8("Nome")
                   << trUtf8("Tipo")
                   << trUtf8("A") + " [" + m_d->unitMeasure->string(UnitMeasure::sectL2) + "]"
                   << trUtf8("Materiale");
        setHeaders( headerList );
    }
}

Qt::ItemFlags SectionModel::flags(const QModelIndex &index) const {
    if(index.isValid()){
        if(index.column() == m_dd->materialColumn ){
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }
    }
    return TableModelPlus::flags(index);
}


QVariant SectionModel::data(const QModelIndex &index, int role) const {
    if( (index.isValid()) &&  (index.column() == m_dd->materialColumn) && (role == Qt::DisplayRole || role == Qt::EditRole) ){
        if( index.row() >= 0 && index.row() < m_dd->sectionContainer.size()  ){
            Section * sect = m_dd->sectionContainer.at(index.row());
            return qVariantFromValue( (void *) sect->material() );
        }
    }
    return TableModelPlus::data(index, role);
}

bool SectionModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if( (index.isValid()) &&  (index.column() == m_dd->materialColumn) && role == Qt::EditRole){
        if( index.row() >= 0 && index.row() < m_dd->sectionContainer.size()  ){
            Section * sect = m_dd->sectionContainer.at(index.row());
            Material * mat =(Material *) value.value<void *>();
            sect->setMaterial( mat );
            return true;
        }
        return false;
    }
    return TableModelPlus::setData(index, value, role);
}

void SectionModel::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement( m_d->modelName );
    for( SectionIterator i = m_dd->sectionContainer.begin(); i != m_dd->sectionContainer.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void SectionModel::readXml(QXmlStreamReader *reader) {
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
            Section * addedSect = createSection( m_d->unitMeasure, m_dd->materialModel, n, reader->attributes() );
            if( addedSect ){
                insertSection( addedSect, m_dd->sectionContainer.size() );
                addedSect->readXml(reader);
            }
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void SectionModel::insertRows( SectionSpace::SectionType typeSect, int position, int count ){
    if (position < 0 || position > m_dd->sectionContainer.size() )
        return;
    for( int i=0; i < count; ++i){
        Section * addedSect = createSection( typeSect );
        if( addedSect ){
            insertSection( addedSect, position );
        }
    }
}

void SectionModel::insertSection( Section * addedSect, int position ){
    if (position < 0 || position > m_dd->sectionContainer.size() )
        position = m_dd->sectionContainer.size();
    if( addedSect ){
        m_dd->insertSection( addedSect, position );

        insertRowsPrivate( position );

        setVarValueRow( position, addedSect->name, addedSect->typeName, addedSect->A, NULL );
        connect( addedSect, SIGNAL(sectionChanged()), this, SIGNAL(modelChanged()) );
        connect( addedSect, SIGNAL(materialChanged(Material*,Material*)), this, SLOT(updateMaterial()) );

        emit modelChanged();
    }
}

void SectionModel::updateMaterial(){
    Section * sectionCh = dynamic_cast<Section *>(QObject::sender());
    if( sectionCh != NULL  ){
        int r = m_dd->sectionContainer.indexOf( sectionCh );
        QModelIndex index = createIndex(r, m_dd->materialColumn );
        emit dataChanged( index, index );
    }
}

Section * SectionModel::createSection( SectionSpace::SectionType type ){
    switch( type ){
    // aggiungi qui le nuove sezioni
    case SectionSpace::CncrSection:{
        Material * cncr = m_dd->materialModel->firstMaterial( MaterialModel::ConcreteMaterial );
        if( cncr != NULL ){
            if( dynamic_cast<Concrete *>(cncr) != NULL){
                return new SectionCncr( m_d->unitMeasure, dynamic_cast<Concrete *>(cncr), trUtf8("Sezione in Cls") );
            }
        }
        break;
    }
    case SectionSpace::SteelCncrSection:{
        Material * steelCncr = m_dd->materialModel->firstMaterial( MaterialModel::SteelCncrMaterial );
        if( steelCncr != NULL ){
            if( dynamic_cast<SteelCncr *>(steelCncr) != NULL){
                return new SectionSteelCncr( m_d->unitMeasure, dynamic_cast<SteelCncr *>(steelCncr), trUtf8("Armatura C.A:") );
            }
        }
        break;
    }
    case SectionSpace::RCncrSection:{
        return new SectionRCncr( m_d->unitMeasure, m_dd->materialModel, trUtf8("Sezione in C.A.") );
        break;
    }
    case SectionSpace::SteelSection:{
        Material * steel = m_dd->materialModel->firstMaterial( MaterialModel::SteelMaterial );
        if( steel != NULL ){
            if( dynamic_cast<Steel *>(steel) != NULL){
                return new SectionSteel( m_d->unitMeasure, dynamic_cast<Steel *>(steel), SectionSteel::sectionRolled, SectionSteel::sectionI, trUtf8("Sezione in Acciaio") );
            }
        }
        break;
    }
    case SectionSpace::TimberSection:{
        Material * timber = m_dd->materialModel->firstMaterial( MaterialModel::TimberMaterial );
        if( timber != NULL ){
            if( dynamic_cast<Timber *>(timber) != NULL){
                return new SectionTimber( m_d->unitMeasure, dynamic_cast<Timber *>(timber), trUtf8("Sezione in Legno") );
            }
        }
        break;
    }
    case SectionSpace::ArcSection:{
        Material * mat = m_dd->materialModel->firstMaterial();
        if( mat != NULL ){
            return new SectionArc( m_d->unitMeasure, mat, trUtf8("Sez. sottile ad arco") );
        }
        break;
    }
    case SectionSpace::LineSection:{
        Material * mat = m_dd->materialModel->firstMaterial();
        if( mat != NULL ){
            return new SectionLine( m_d->unitMeasure, mat, trUtf8("Sez. sottile rettilinea") );
        }
        break;
    }
    case SectionSpace::ThinSection:{
        Material * mat = m_dd->materialModel->firstMaterial();
        if( mat != NULL ){
            return new SectionThin( m_d->unitMeasure, mat, trUtf8("Sezione sottile") );
        }
        break;
    }
    case SectionSpace::ThinSectionCont:{
        Material * mat = m_dd->materialModel->firstMaterial();
        if( mat != NULL ){
            return new SectionThinCont( m_d->unitMeasure, mat, trUtf8("Sezione sottile continua") );
        }
        break;
    }
    case SectionSpace::ThinSectionContSteel:{
        Material * steel = m_dd->materialModel->firstMaterial(MaterialModel::SteelMaterial);
        if( steel != NULL ){
            return new SectionThinContSteel( m_d->unitMeasure, dynamic_cast<Steel *>(steel), trUtf8("Sezione piegata a freddo") );
        }
        break;
    }
    case SectionSpace::ProfileSection:{
        Material * mat = m_dd->materialModel->firstMaterial();
        if( mat != NULL ){
            return new SectionProfile( m_d->unitMeasure, mat, trUtf8("Sezione Profilo") );
        }
        break;
    }
    case SectionSpace::PointsSection:{
        Material * mat = m_dd->materialModel->firstMaterial();
        if( mat != NULL ){
            return new SectionPoints( m_d->unitMeasure, mat, trUtf8("Sezione Punti") );
        }
        break;
    }
    default:
        Material * mat = m_dd->materialModel->firstMaterial();
        if( mat != NULL ){
            return new Section( m_d->unitMeasure, mat, trUtf8("Sezione") );
        }
        break;
    }
    return NULL;
}

Section *SectionModel::createSection(UnitMeasure * ump, MaterialModel * matModel, const QString & sectType, const QXmlStreamAttributes &attrs) {
    // aggiungi qui le nuove sezioni
    QString sectTypeUp = sectType.toUpper();
    if( sectTypeUp == "SECTIONCNCR"){
        return new SectionCncr( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONSTEELCNCR"){
        return new SectionSteelCncr( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONRCNCR"){
        return new SectionRCncr( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONSTEEL"){
        return new SectionSteel( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONTIMBER"){
        return new SectionTimber( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONARC"){
        return new SectionArc( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONLINE"){
        return new SectionLine( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONTHIN"){
        return new SectionThin( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONTHINCONT"){
        return new SectionThinCont( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONTHINCONTSTEEL"){
        return new SectionThinContSteel( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONPOINTS"){
        return new SectionPoints( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONPROFILE"){
        return new SectionProfile( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTIONFRP"){
        return new SectionFRP( ump, attrs, matModel, NULL );
    }
    if( sectTypeUp == "SECTION"){
        return new Section( ump, attrs, matModel, NULL );
    }
    return NULL;
}

void SectionModel::removeRows(int position, int count) {
    if (position < 0 || (position + count) > m_dd->sectionContainer.size())
        return;

    for (int row = position; row < (position+count); row++){
        if( m_dd->sectionContainer.at(position)->isUsed()){
            QString title = trUtf8("Sezione già in uso");
            QString message = trUtf8("Sezioned usata da almeno un elemento.");
            qWarning() << title;
            qWarning() << message;
        } else {
            // scolleghiamo la sezione
            disconnect( m_dd->sectionContainer.at(position), SIGNAL(materialChanged(Material*,Material*)), this, SLOT(updateMaterial()) );
            disconnect( m_dd->sectionContainer.at(position), SIGNAL(sectionChanged()), this, SIGNAL(modelChanged()) );

            // ok, procediamo a cancellare la riga
            removeRowsPrivate( position );
            // e la sezione
            m_dd->removeSectionRow( position );
        }
    }
    emit modelChanged();
}

Section * SectionModel::section( int i ){
    return m_dd->section( i );
}

Section * SectionModel::section( int i ) const{
    return m_dd->section( i );
}

Section * SectionModel::sectionId( unsigned int i){
    return m_dd->sectionId( i );
}

Section *SectionModel::firstSection(SectionSpace::SectionType t) {
    if( m_dd->sectionContainer.size() > 0 ){
        if( t == SectionSpace::GenericSection ){
            return m_dd->sectionContainer.first();
        } else {
            for( SectionIterator i = m_dd->sectionContainer.begin(); i != m_dd->sectionContainer.end(); ++i ){
                if( (*i)->sectionType() == t ){
                    return *i;
                }
            }
        }
    }
    return NULL;
}

Section *SectionModel::lastSection(SectionSpace::SectionType st) {
    if( m_dd->sectionContainer.size() > 0 ){
        if( st == SectionSpace::GenericSection ){
            return m_dd->sectionContainer.last();
        } else {
            SectionIterator i = m_dd->sectionContainer.end();
            while( i != m_dd->sectionContainer.begin() ){
                --i;
                if( (*i)->sectionType() == st ){
                    return *i;
                }
            }
        }
    }
    return NULL;
}

int SectionModel::count(){
    return m_dd->sectionContainer.size();
}

QList< Section *> SectionModel::sectionsList(SectionSpace::SectionType st){
    QList< Section * > ret;
    for( QList<Section *>::iterator i=m_dd->sectionContainer.begin(); i != m_dd->sectionContainer.end(); ++i ){
        if( ((*i)->sectionType() == st) || (st == SectionSpace::GenericSection) ){
            ret.append( *i );
        }
    }
    return ret;
}
