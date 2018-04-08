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
#include "multisectionmodelbase.h"

#include "tablemodelplusprivate.h"

#include "sectionrcncr.h"
#include "sectionrcncr.h"
#include "sectioncncr.h"
#include "sectionsteel.h"
#include "sectionsteelcncr.h"
#include "sectiontimber.h"
#include "section.h"
#include "material.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "intplus.h"
#include "uintplus.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QGraphicsItemGroup>

MultiSectionModelBase::MultiSectionModelBase( const QString &modName, UnitMeasure * ump, MaterialModel * matModel, QObject * parent):
    TableModelPlus( modName, ump, parent ),
    m_materialColumn(1),
    m_loadPhaseColumn(2),
    m_sectionContainer( new QList<Section *> ),
    m_sectionMap ( new QMap<unsigned int, Section *> ),
    m_materialModel( matModel ),
    m_qGraphicsItem( NULL ){
}

MultiSectionModelBase::~MultiSectionModelBase(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        delete (*iter);
        iter++;
    }
    m_sectionContainer->clear();
    if( m_qGraphicsItem ){
        delete m_qGraphicsItem;
    }
}

MultiSectionModelBase & MultiSectionModelBase::operator=(MultiSectionModelBase & val) {
    if( this != &val ){
        m_materialModel = val.m_materialModel;

        removeRows( 0, count() );
        insertRows( 0, val.count() );

        QList<Section *>::iterator iter = m_sectionContainer->begin() ;
        QList<Section *>::iterator iterVal = val.m_sectionContainer->begin() ;
        while( (iter != m_sectionContainer->end()) && (iterVal != val.m_sectionContainer->end())){
            **iter = **iterVal;
            iter++;
            iterVal++;
        }
    }
    return *this;
}

Qt::ItemFlags MultiSectionModelBase::flags(const QModelIndex &index) const {
    if(index.isValid()){
        if( (index.column() == m_materialColumn) || (index.column() == m_loadPhaseColumn) ){
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }
    }
    return TableModelPlus::flags(index);
}

QVariant MultiSectionModelBase::data(const QModelIndex &index, int role) const {
    if( (index.isValid()) &&  (index.column() == m_materialColumn) && (role == Qt::DisplayRole || role == Qt::EditRole) ){
        if( index.row() >= 0 && index.row() < m_sectionContainer->size()  ){
            Section * sect = m_sectionContainer->at(index.row());
            return qVariantFromValue( (void *) sect->material() );
        }
    }
    if( (index.isValid()) &&  (index.column() == m_loadPhaseColumn) && (role == Qt::DisplayRole || role == Qt::EditRole) ){
        if( index.row() >= 0 && index.row() < m_sectionContainer->size()  ){
            Section * sect = m_sectionContainer->at(index.row());
            return qVariantFromValue( (void *) sect->loadPhase() );
        }
    }
    return TableModelPlus::data(index, role);
}

bool MultiSectionModelBase::setData(const QModelIndex &index, const QVariant &value, int role) {
    if( (index.isValid()) &&  (index.column() == m_materialColumn) && role == Qt::EditRole){
        if( index.row() >= 0 && index.row() < m_sectionContainer->size()  ){
            Section * sect = m_sectionContainer->at(index.row());
            Material * mat =(Material *) value.value<void *>();
            sect->setMaterial( mat );
            return true;
        }
        return false;
    }
    if( (index.isValid()) &&  (index.column() == m_loadPhaseColumn) && role == Qt::EditRole){
        if( index.row() >= 0 && index.row() < m_sectionContainer->size()  ){
            Section * sect = m_sectionContainer->at(index.row());
            SectionLoadPhase * lp =(SectionLoadPhase *) value.value<void *>();
            sect->setLoadPhase( lp );
            return true;
        }
        return false;
    }
    return TableModelPlus::setData(index, value, role);
}

void MultiSectionModelBase::writeXml(QXmlStreamWriter *writer){
    writer->writeStartElement( m_d->modelName );
    for( QList<Section *>::iterator i = m_sectionContainer->begin(); i != m_sectionContainer->end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void MultiSectionModelBase::readXml(QXmlStreamReader *reader){
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    QString myName = m_d->modelName.toUpper();

    /*    // Ci mettiamo sul tag iniziale
    while( !(n == myName && t == QXmlStreamReader::StartElement) &&
           !(reader->hasError()) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }*/

    // Andiamo sul tag finale. Nel frattempo leggiamo le sezioni.
    while( !(t == QXmlStreamReader::EndElement && n == myName ) &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        if( t == QXmlStreamReader::StartElement ){
            Section * addedSect = SectionModel::createSection( m_d->unitMeasure, m_materialModel, n, reader->attributes() );
            if( addedSect ){
                insertSection( addedSect, m_sectionContainer->size() );
                addedSect->readXml( reader );
            }
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}


Section * MultiSectionModelBase::createSection( const QString & sectType, const QXmlStreamAttributes & attrs ) {
    return SectionModel::createSection( m_d->unitMeasure, m_materialModel, sectType, attrs );
}

bool MultiSectionModelBase::insertRows(int first, int count , const QModelIndex &parent){
    Q_UNUSED(parent)
    for( int row =first; row < (first+count); ++row ){
        Section * addedSect = createSection();
        if( addedSect ){
            insertSection( addedSect, row );
        }
    }
    return true;
}

bool MultiSectionModelBase::appendRows(int count) {
    return insertRows( m_sectionContainer->size(), count );
}

void MultiSectionModelBase::removeRows(int position, int count) {
    if(position < 0 || (position + count) > m_sectionContainer->size() || count == 0 )
        return;

    for (int row = position; row < (position+count); row++){
        Section * sect = m_sectionContainer->at(position);
        disconnect( sect, &Section::sectionChanged, this, &MultiSectionModelBase::modelChanged );
        disconnect( sect, &Section::materialChanged, this, &MultiSectionModelBase::updateMaterial );

        removeRowsPrivate( position );

        removeSectionPrivate( position );
    }
    emit modelChanged();
}

void MultiSectionModelBase::insertSection( Section * addedSect, int position ){
    if (position < 0 || position > m_sectionContainer->size() )
        position = m_sectionContainer->size();

    if( addedSect ){
        insertSectionPrivate( addedSect, position );

        insertRowsPrivate( position );

        setVar( DisplayValue, position, 0, addedSect->name );
        connect( addedSect, &Section::sectionChanged, this, &MultiSectionModelBase::modelChanged );
        connect( addedSect, &Section::materialChanged, this, &MultiSectionModelBase::updateMaterial );

        if( m_qGraphicsItem != NULL ){
            m_qGraphicsItem->addToGroup( addedSect->qGraphicsItem() );
        }

        emit modelChanged();
    }
}

void MultiSectionModelBase::updateMaterial(){
    if( m_materialColumn > -1 ){
        Section * sectionCh = dynamic_cast<Section *>(QObject::sender());
        if( sectionCh != NULL  ){
            int r = m_sectionContainer->indexOf( sectionCh );
            QModelIndex i = createIndex( r, m_materialColumn );
            emit dataChanged( i, i );
        }
    }
}

void MultiSectionModelBase::translateNormal( double yTrans, double zTrans, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }
    QList<Section *>::iterator sect = m_sectionContainer->begin();
    while( sect != m_sectionContainer->end() ){
        (*sect)->translateNormal(yTrans, zTrans, suspSignals );
        sect++;
    }
    if( suspSignals ){
        updateReadOnlyVars();
        suspendSignals( false );
    }
}

void MultiSectionModelBase::rotateNormal( double rot, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    while( iter != m_sectionContainer->end() ){
        (*iter)->rotateNormal( rot, suspSignals );
        iter++;
    }
    if( suspSignals ){
        updateReadOnlyVars();
        suspendSignals( false );
    }
}

int MultiSectionModelBase::count()    {
    return m_sectionContainer->size();
}

Section * MultiSectionModelBase::section( int i ){
    if( i >= 0 && i < m_sectionContainer->size() )
        return m_sectionContainer->at( i );
    else
        return NULL;
}

Section * MultiSectionModelBase::lastSection(){
    if( m_sectionContainer->size() > 0 ){
        return m_sectionContainer->last();
    } else {
        return NULL;
    }
}

double MultiSectionModelBase::EANormal() {
    double ret = 0.0;
    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); ++iter ){
        if( (*iter)->material() != NULL ){
            ret += (*iter)->material()->E->valueNormal() * (*iter)->A->valueNormal();
        }
    }
    return ret;
}

double MultiSectionModelBase::EIyyNormal( double yCen, double zCen ) {
    double ret = 0.0;
    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); ++iter ){
        if( (*iter)->material() != NULL ){
            ret += (*iter)->material()->E->valueNormal() * (*iter)->IyyTransRotNormal( yCen, zCen, 0.0 );
        }
    }
    return ret;
}

double MultiSectionModelBase::EIzzNormal( double yCen, double zCen ) {
    double ret = 0.0;
    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); ++iter ){
        if( (*iter)->material() != NULL ){
            ret += (*iter)->material()->E->valueNormal() * (*iter)->IzzTransRotNormal( yCen, zCen, 0.0 );
        }
    }
    return ret;
}

Section * MultiSectionModelBase::firststSection(){
    if( m_sectionContainer->size() > 0 ){
        return m_sectionContainer->at(0);
    } else {
        return NULL;
    }
}

Section * MultiSectionModelBase::sectionId( unsigned int i){
    if( m_sectionMap->find(i) == m_sectionMap->end() )
        return NULL;
    else
        return m_sectionMap->value( i );
}

double MultiSectionModelBase::ANormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->A->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::SyNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->Sy->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::SzNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->Sz->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::IyyNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->Iyy->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::IzzNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->Izz->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::IyzNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->Iyz->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::Sy3DyDzNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->Sy3DyDz->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::Sz3DyDzNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->Sz3DyDz->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::Syz2DyDzNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->Syz2DyDz->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::Sy2zDyDzNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->Sy2zDyDz->valueNormal();
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::pWNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->pW->valueNormal();
        iter++;
    }
    return ret;
}

void MultiSectionModelBase::NMSLSNormal( double *NRet, double *MyRet, double *MzRet,
                                         double l, double my, double mz, QList<Point2DModel *> *sects) {
    *NRet = 0.0;
    *MyRet = 0.0;
    *MzRet = 0.0;
    double NTmp = 0.0, MyTmp = 0.0, MzTmp = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    while( iter != m_sectionContainer->end() ){
        (*iter)->NMSLSNormal( &NTmp, &MyTmp, &MzTmp, l, my, mz, sects );
        *NRet += NTmp;
        *MyRet += MyTmp;
        *MzRet += MzTmp;
        iter++;
    }
}

double MultiSectionModelBase::NULSNormal( double l, double my, double mz, QList<Point2DModel *> * sects ){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    while( iter != m_sectionContainer->end() ){
        ret += (*iter)->NULSNormal( l, my, mz, sects );
        iter++;
    }
    return ret;
}

double MultiSectionModelBase::NULSNormal( int phase,
                                          QList<double> * l, QList<double> * my, QList<double> * mz,
                                          QList<Point2DModel *> * sects ){
    double ret = 0.0;
    if( l->size() > int(phase) && my->size() > int(phase) && mz->size() > int(phase) ){
        QList<Section *>::iterator iter = m_sectionContainer->begin();
        while( iter != m_sectionContainer->end() ){
            if( (*iter)->internalLoadPhase() <= phase ){
                double lImp = 0.0, myImp = 0.0, mzImp = 0.0;
                if( (*iter)->internalLoadPhase() > 0 ){
                    lImp = l->at((*iter)->internalLoadPhase()-1);
                    myImp = my->at((*iter)->internalLoadPhase()-1);
                    mzImp = mz->at((*iter)->internalLoadPhase()-1);
                }
                ret += (*iter)->NULSNormal( l->at(phase)-lImp, my->at(phase)-myImp, mz->at(phase)-mzImp, sects );
            }
            iter++;
        }
    }
    return ret;
}

void MultiSectionModelBase::MULSNormal( double *yRet, double * zRet,
                                        double l, double my, double mz,
                                        QList<Point2DModel *> * sects ){
    *yRet = 0.0;
    *zRet = 0.0;
    double yTmp = 0.0, zTmp = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    while( iter != m_sectionContainer->end() ){
        (*iter)->MULSNormal( &yTmp, &zTmp, l, my, mz, sects );
        *yRet += yTmp;
        *zRet += zTmp;
        iter++;
    }
}

void MultiSectionModelBase::MULSNormal( double *yRet, double * zRet,
                                        int phase,
                                        QList<double> * l, QList<double> * my, QList<double> * mz,
                                        QList<Point2DModel *> * sects ){
    *yRet = 0.0;
    *zRet = 0.0;
    if( (l->size() > phase) && (my->size() > phase) && (mz->size() > phase) ){
        double yTmp = 0.0, zTmp = 0.0;
        QList<Section *>::iterator iter = m_sectionContainer->begin();
        while( iter != m_sectionContainer->end() ){
            if( (*iter)->internalLoadPhase() <= phase ){
                double lImp = 0.0, myImp = 0.0, mzImp = 0.0;
                if( (*iter)->internalLoadPhase() > 0 ){
                    lImp = l->at((*iter)->internalLoadPhase()-1);
                    myImp = my->at((*iter)->internalLoadPhase()-1);
                    mzImp = mz->at((*iter)->internalLoadPhase()-1);
                }
                (*iter)->MULSNormal( &yTmp, &zTmp,
                                     l->at(phase)-lImp, my->at(phase)-myImp, mz->at(phase)-mzImp,
                                     sects );
                *yRet += yTmp;
                *zRet += zTmp;
            }
            iter++;
        }
    }
}

QGraphicsItem * MultiSectionModelBase::qGraphicsItem(){
    if( m_qGraphicsItem == NULL ){
        m_qGraphicsItem = new QGraphicsItemGroup;
        for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){
            m_qGraphicsItem->addToGroup( (*iter)->qGraphicsItem()  );
        }
    }
    return m_qGraphicsItem;
}

void MultiSectionModelBase::suspendSignals(bool susp) {
    for( QList<Section *>::iterator sect = m_sectionContainer->begin(); sect != m_sectionContainer->end(); ++sect ){
        (*sect)->suspendSignals( susp );
    }
}

void MultiSectionModelBase::updateReadOnlyVars() {
    for( QList<Section *>::iterator sect = m_sectionContainer->begin(); sect != m_sectionContainer->end(); ++sect ){
        (*sect)->updateReadOnlyVars();
    }
}

void MultiSectionModelBase::insertSectionPrivate(Section * s, int p) {
    // l'id deve essere unico
    while( m_sectionMap->contains(s->id->valueNormal()) ){
        s->nextId();
    }
    m_sectionContainer->insert( p, s );
    m_sectionMap->insert( s->id->valueNormal(), s );
}

void MultiSectionModelBase::removeSectionPrivate(int p) {
    m_sectionMap->remove( m_sectionContainer->at(p)->id->valueNormal() );
    delete m_sectionContainer->at(p);
    m_sectionContainer->removeAt(p);
}

Section *MultiSectionModelBase::createSection() {
    return NULL;
}
