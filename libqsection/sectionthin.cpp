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
#include "sectionthin.h"

#include "sectionthinprivate.h"

#include "sectionline.h"
#include "sectionarc.h"
#include "material.h"
#include "materialmodel.h"

#include "uintplus.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "point2dplus.h"
#include "convertutility.h"

#include <QXmlStreamWriter>
#include <QList>
#include <QMap>
#include <QFont>

#include <cmath>

SectionThin::SectionThin(UnitMeasure * ump, Material * mat, const QString & nn, QObject *parent) :
    Section( ump, mat, nn, parent ),
    m_d(new SectionThinPrivate( ump, mat, 0.0 )){
    initVar();
}

SectionThin::SectionThin(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *matModel, SectionLoadPhaseModel * lmodel, QObject *parent):
    Section( ump, NULL, "", parent ),
    m_d(new SectionThinPrivate( ump, NULL, 0.0 )){
    initVar();
    loadFromXML( attrs, matModel, lmodel );
}

SectionThin::~SectionThin(){
    delete m_d;
}

SectionThin & SectionThin::operator=( SectionThin & val ){
    if( this != &val ){
        this->Section::operator=( val );
        *(m_d->sectionLineModel) = *(val.m_d->sectionLineModel);
        *(m_d->sectionArcModel) = *(val.m_d->sectionArcModel);
    }
    return *this;
}

Section & SectionThin::operator=( Section & val ){
    if( this != &val ){
        SectionThin * sectionThin =dynamic_cast<SectionThin *>(&val);
        if( sectionThin ){
            this->SectionThin::operator=( *sectionThin );
            return *this;
        }
        this->Section::operator=( val );
    }
    return *this;
}

void SectionThin::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement(*m_typeNameInternal);
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i) {
        writer->writeAttribute( (*i)->xmlAttribute() );
    }
    if( m_material ){
        writer->writeAttribute( "material", m_material->id->valueNormalStr() );
    }

    m_d->sectionLineModel->writeXml( writer );
    m_d->sectionArcModel->writeXml( writer );

    writer->writeEndElement();
}

void SectionThin::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType token = reader->readNext();
    QString name = reader->name().toString().toUpper();
    QString myName = m_typeNameInternal->toUpper();
    while( !(token == QXmlStreamReader::EndElement && name == myName) &&
           !(reader->hasError() ) &&
           (token != QXmlStreamReader::EndDocument) ){
        if( token == QXmlStreamReader::StartElement ){
            if( name == m_d->sectionLineModel->modelName().toUpper() ){
                token = reader->readNext();
                m_d->sectionLineModel->readXml(reader);
            }
            if( name == m_d->sectionArcModel->modelName().toUpper() ){
                token = reader->readNext();
                m_d->sectionArcModel->readXml(reader);
            }
        }
        token = reader->readNext();
        name = reader->name().toString().toUpper();
    }

    setTNormal();
}

void SectionThin::setMaterial( Material * mat, bool emitAuto ){
    if( mat != m_material ){
        Section::setMaterial( mat, emitAuto );
        m_d->sectionLineModel->setMaterial( m_material, emitAuto );
        m_d->sectionArcModel->setMaterial( m_material, emitAuto );
    }
}

void SectionThin::initVar(){
    *m_typeNameInternal = "SectionThin";
    typeName->setValue( trUtf8("Sottile"));

    t = new DoublePlus(0.0, "t", m_unitMeasure, UnitMeasure::sectL );
    t->setRichName( trUtf8("t"));
    t->setToolTip( trUtf8("Spessore degli elementi (considerato solo se viene spuntato il campo t del singolo elemento)"));
    addVarToContainer( t );
    connect( t, &DoublePlus::valueChanged, this, &SectionThin::setTNormal );

    A->setReadOnly( true );
    Sy->setReadOnly( true );
    Sz->setReadOnly( true );
    Iyy->setReadOnly( true );
    Izz->setReadOnly( true );
    Iyz->setReadOnly( true );
    Sy3DyDz->setReadOnly( true );
    Sz3DyDz->setReadOnly( true );
    Sy2zDyDz->setReadOnly( true );
    Syz2DyDz->setReadOnly( true );

    connect( m_d->sectionLineModel, &MultiSectionLineModel::modelChanged, this, &SectionThin::updateReadOnlyVars );
    connect( m_d->sectionLineModel, &MultiSectionLineModel::setTNormalSignal, this, &SectionThin::setTNormalModelLine );
    connect( m_d->sectionArcModel, &MultiSectionArcModel::modelChanged, this, &SectionThin::updateReadOnlyVars );
    connect( m_d->sectionArcModel, &MultiSectionArcModel::setTNormalSignal, this, &SectionThin::setTNormalModelArc );
    connect( m_d->sectionLineModel, &MultiSectionLineModel::modelChanged, this, &SectionThin::sectionChanged );
    connect( m_d->sectionArcModel, &MultiSectionArcModel::modelChanged, this, &SectionThin::sectionChanged );
}

void SectionThin::setTNormalModelLine(){
    suspendSignals( true );
    m_d->sectionLineModel->setTNormal( t->valueNormal() );
    suspendSignals( false );
    m_d->sectionLineModel->updateReadOnlyVars();
}

void SectionThin::setTNormalModelArc(){
    suspendSignals( true );
    m_d->sectionArcModel->setTNormal( t->valueNormal() );
    suspendSignals( false );
    m_d->sectionArcModel->updateReadOnlyVars();
}

void SectionThin::setTNormal(){
    setTNormalModelArc();
    setTNormalModelLine();
}

MultiSectionLineModel * SectionThin::sectionLineModel(){
    return m_d->sectionLineModel;
}

MultiSectionArcModel * SectionThin::sectionArcModel(){
    return m_d->sectionArcModel;
}

void SectionThin::translateNormal( double yTrans, double zTrans, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }
    m_d->sectionLineModel->translateNormal( yTrans, zTrans );
    m_d->sectionArcModel->translateNormal( yTrans, zTrans );
    updateReadOnlyVars();
    if( suspSignals ){
        suspendSignals( false );
    }
}

void SectionThin::rotateNormal(double rot, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }
    m_d->sectionLineModel->rotateNormal( rot );
    m_d->sectionArcModel->rotateNormal( rot );
    updateReadOnlyVars();
    if( suspSignals ){
        suspendSignals(false);
    }
}

SectionLine * SectionThin::sectionLine( int i ){
    return m_d->sectionLineModel->sectionLine( i );
}

SectionLine * SectionThin::sectionLineId( unsigned int i){
    return m_d->sectionLineModel->sectionLineId(i);
}

int SectionThin::nSectionLine(){
    return m_d->sectionLineModel->count();
}

SectionArc * SectionThin::sectionArc( int i ){
    return m_d->sectionArcModel->sectionArc(i);
}

SectionArc * SectionThin::sectionArcId( unsigned int i){
    return m_d->sectionArcModel->sectionArcId(i);
}

int SectionThin::nSectionArc(){
    return m_d->sectionArcModel->count();
}

double SectionThin::ANormal(){
    return ( m_d->sectionLineModel->ANormal() + m_d->sectionArcModel->ANormal() );
}

double SectionThin::SyNormal(){
    return ( m_d->sectionLineModel->SyNormal() + m_d->sectionArcModel->SyNormal() );
}

double SectionThin::SzNormal(){
    return ( m_d->sectionLineModel->SzNormal() + m_d->sectionArcModel->SzNormal() );
}

double SectionThin::IyyNormal(){
    return ( m_d->sectionLineModel->IyyNormal() + m_d->sectionArcModel->IyyNormal() );
}

double SectionThin::IzzNormal(){
    return ( m_d->sectionLineModel->IzzNormal() + m_d->sectionArcModel->IzzNormal() );
}

double SectionThin::IyzNormal(){
    return ( m_d->sectionLineModel->IyzNormal() + m_d->sectionArcModel->IyzNormal() );
}

double SectionThin::Sy3DyDzNormal(){
    return ( m_d->sectionLineModel->Sy3DyDzNormal() + m_d->sectionArcModel->Sy3DyDzNormal() );
}

double SectionThin::Sz3DyDzNormal(){
    return ( m_d->sectionLineModel->Sz3DyDzNormal() + m_d->sectionArcModel->Sz3DyDzNormal() );
}

double SectionThin::Sy2zDyDzNormal(){
    return ( m_d->sectionLineModel->Sy2zDyDzNormal() + m_d->sectionArcModel->Sy2zDyDzNormal() );
}

double SectionThin::Syz2DyDzNormal(){
    return ( m_d->sectionLineModel->Syz2DyDzNormal() + m_d->sectionArcModel->Syz2DyDzNormal() );
}

double SectionThin::ITNormal(){
    return ( m_d->sectionLineModel->ITNormal() + m_d->sectionArcModel->ITNormal() );
}

double SectionThin::pWNormal(){
    return ( m_d->sectionLineModel->pWNormal() + m_d->sectionArcModel->pWNormal() );
}

double SectionThin::WMyNormal(){
    double z1 = m_d->sectionLineModel->zMaxNormal();
    double z2 = m_d->sectionArcModel->zMaxNormal();
    double zMax = z1 > z2 ? z1: z2;
    if( zMax != 0.0  ){
        return IyyNormal() / zMax;
    } else {
        return 0.0;
    }
}

double SectionThin::WMzNormal(){
    double y1 = m_d->sectionLineModel->yMinNormal();
    double y2 = m_d->sectionArcModel->yMinNormal();
    double yMin = y1 < y2 ? y1: y2;
    if( yMin != 0.0  ){
        return - IzzNormal() / yMin;
    } else {
        return 0.0;
    }
}

double SectionThin::WPlMyNormal(){
    SectionThin sectPlus( m_unitMeasure, m_material, "sectPlus");
    sectionLineModel()->cut( sectPlus.sectionLineModel(), NULL, 0.0, 0.0, 1.0, 0.0 );
    sectionArcModel()->cut( sectPlus.sectionArcModel(), NULL, 0.0, 0.0, 1.0, 0.0 );
    return sectPlus.Sy->valueNormal() * 2.0;
}

double SectionThin::WPlMzNormal(){
    SectionThin sectPlus( m_unitMeasure, m_material, "sectPlus");
    sectionLineModel()->cut( sectPlus.sectionLineModel(), NULL, 0.0, 1.0, 0.0, 0.0 );
    sectionArcModel()->cut( sectPlus.sectionArcModel(), NULL, 0.0, 1.0, 0.0, 0.0 );
    return sectPlus.Sz->valueNormal() * 2.0;
}

double SectionThin::WPlNormalT( double theta ){
    // parametro da calcolare
    double t = 0.0;

    // parametro confronto calcolo aree
    double ARel = A->valueNormal();

    if( ARel ){

        // estremi del parametro t
        double tInf, tSup;

        double tMin, tMax;
        bool found = false;
        if( sectionLineModel()->proj( &tMin, &tMax, cos(theta), sin(theta)) ){
            tInf = tMax;
            tSup = tMax;
            found = true;
        }
        if( sectionArcModel()->proj( &tMin, &tMax, cos(theta), sin(theta)) ){
            if( found ){
                if( tMax > tSup ){
                    tSup = tMax;
                }
                if( tMin < tInf ){
                    tInf = tMin;
                }
            } else {
                tInf = tMax;
                tSup = tMax;
                found = true;
            }
        }

        // sezione con def positive
        SectionThin sectPlus( m_unitMeasure, m_material, "sectPlus");
        // sezione con def negative
        SectionThin sectMinus( m_unitMeasure, m_material, "sectMinus");

        // calcoliamo area di sectPlus e sectMinuis in corrispondenza di tSup
        // tSup deve corrispondere a (sectPlus.A - sectMinus.A) < 0.0
        double y0 = cos( theta ) * tSup;
        double z0 = sin( theta ) * tSup;
        double l = - y0 * cos( theta) - z0 * sin(theta);
        double my = cos(theta);
        double mz = sin(theta);
        sectionLineModel()->cut(sectPlus.sectionLineModel(), sectMinus.sectionLineModel(), l, my, mz );
        sectPlus.sectionArcModel()->clear();
        sectMinus.sectionArcModel()->clear();
        sectionArcModel()->cut(sectPlus.sectionArcModel(), sectMinus.sectionArcModel(), l, my, mz );
        if( (sectPlus.A->valueNormal() - sectMinus.A->valueNormal()) > 0.0 ){
            double tmp = tSup;
            tSup = tInf;
            tInf = tmp;
        }

        double dA = 1.0;
        while( dA > WPLPREC ){
            t = 0.50 * (tInf + tSup );

            y0 = cos( theta ) * t;
            z0 = sin( theta ) * t;
            l = - y0 * cos( theta) - z0 * sin(theta);
            my = cos(theta);
            mz = sin(theta);
            sectionLineModel()->cut(sectPlus.sectionLineModel(), sectMinus.sectionLineModel(), l, my, mz );
            sectPlus.sectionArcModel()->clear();
            sectMinus.sectionArcModel()->clear();
            sectionArcModel()->cut(sectPlus.sectionArcModel(), sectMinus.sectionArcModel(), l, my, mz );

            if( (sectPlus.A->valueNormal() - sectMinus.A->valueNormal()) > 0.0 ){
                tInf = t;
            } else {
                tSup = t;
            }

            dA = fabs(sectPlus.A->valueNormal() - sectMinus.A->valueNormal()) / ARel;

        }
    }
    return t;
}

double SectionThin::WPlNormal( double ny, double nz ){

    // dimensione trasversale massima della sezione
    // serve per definire un parametro necessario per valutare la
    // precisione del calcolo del momento statico
    double dMax = yMaxNormal() - yMinNormal();
    if( (zMaxNormal() - zMinNormal()) > dMax ){
        dMax = zMaxNormal() - zMinNormal();
    }

    // parametro confronto calcolo mom statico
    double SRel = A->valueNormal() * dMax;

    // se area e dimensione trasversale massima sono diversi da zero
    if( SRel != 0.0 ){
        // inclinazione del vettore momento agente rispetto all'asee y
        double alpha = atan2( nz, ny);

        // sezione con def positive
        SectionThin sectPlus( m_unitMeasure, m_material, "sectPlus");
        // sezione con def negative
        SectionThin sectMinus( m_unitMeasure, m_material, "sectMinus");

        double theta = alpha;
        double beta = alpha;

        // parametro per stima equilibrio trasversale a momento
        double dSn = 1.0;

        while( dSn > WPLPREC ){
            theta -= (beta - alpha);

            double t = WPlNormalT( theta );

            double y0 = cos( theta ) * t;
            double z0 = sin( theta ) * t;
            double l = - y0 * cos( theta) - z0 * sin(theta);
            double my = cos(theta);
            double mz = sin(theta);
            sectionLineModel()->cut(sectPlus.sectionLineModel(), sectMinus.sectionLineModel(), l, my, mz );
            sectPlus.sectionArcModel()->clear();
            sectMinus.sectionArcModel()->clear();
            sectionArcModel()->cut(sectPlus.sectionArcModel(), sectMinus.sectionArcModel(), l, my, mz );

            double SnPlus = cos(alpha) * sectPlus.Sy->valueNormal() + sin(alpha) * sectPlus.Sy->valueNormal();
            double SnMinus = cos(alpha) * sectMinus.Sy->valueNormal() + sin(alpha) * sectMinus.Sy->valueNormal();

            dSn = fabs( SnPlus - SnMinus ) / SRel;

            beta = atan2( sectPlus.Sz->valueNormal() - sectMinus.Sz->valueNormal(), sectPlus.Sy->valueNormal() - sectMinus.Sy->valueNormal());
        }

        double StPlus = - sin(alpha) * sectPlus.Sy->valueNormal() + cos(alpha) * sectPlus.Sy->valueNormal();
        double StMinus = - sin(alpha) * sectMinus.Sy->valueNormal() + cos(alpha) * sectMinus.Sy->valueNormal();

        return (StPlus - StMinus);
    } else {
        return 0.0;
    }
}

double SectionThin::yMaxNormal(){
    double ret = sectionLineModel()->yMaxNormal();
    if( sectionArcModel()->yMaxNormal() > ret ){
        ret = sectionArcModel()->yMaxNormal();
    }
    return ret;
}

double SectionThin::yMinNormal(){
    double ret = sectionLineModel()->yMinNormal();
    if( sectionArcModel()->yMinNormal() < ret ){
        ret = sectionArcModel()->yMinNormal();
    }
    return ret;
}

double SectionThin::zMaxNormal(){
    double ret = sectionLineModel()->zMaxNormal();
    if( sectionArcModel()->zMaxNormal() > ret ){
        ret = sectionArcModel()->zMaxNormal();
    }
    return ret;
}

double SectionThin::zMinNormal(){
    double ret = sectionLineModel()->zMinNormal();
    if( sectionArcModel()->zMinNormal() < ret ){
        ret = sectionArcModel()->zMinNormal();
    }
    return ret;
}

void SectionThin::clear( bool emitAuto ) {
    t->setValueNormal( 0.0, emitAuto );
    m_d->sectionArcModel->clear();
    m_d->sectionArcModel->setTNormal( 0.0, emitAuto );
    m_d->sectionLineModel->clear();
    m_d->sectionLineModel->setTNormal( 0.0, emitAuto );
}


QGraphicsItem * SectionThin::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsItemGroup;
        m_d->qGraphicsItem->addToGroup( m_d->sectionLineModel->qGraphicsItem());
        m_d->qGraphicsItem->addToGroup( m_d->sectionArcModel->qGraphicsItem());
    }
    return m_d->qGraphicsItem;
}
