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
#include "sectiontimbercncr.h"

#include <cmath>

#include "sectioncncr.h"
#include "sectiontimber.h"

#include "materialmodel.h"
#include "concrete.h"
#include "timber.h"

#include "doubleplus.h"
#include "point2dplus.h"
#include "point2dplusmodel.h"
#include "qstringplus.h"

#include "structmath.h"
#include "unitmeasure.h"

#include <QString>
#include <QGraphicsItemGroup>
#include <QPolygonF>
#include <QPointF>

class SectionTimberCncrPrivate{
public:
    SectionTimberCncrPrivate( UnitMeasure * ump, MaterialModel * matModel, const QString & nn, QObject * parent ):
        materialModel( matModel ),
        qGraphicsItem(NULL){
        Concrete * cncr = dynamic_cast<Concrete *>(matModel->firstMaterial( MaterialModel::ConcreteMaterial));
        sectionCncr = new SectionCncr(ump, cncr, nn + QString(":sectionCncr") , parent );
        Timber * tmbr = dynamic_cast<Timber *>(matModel->firstMaterial( MaterialModel::TimberMaterial));
        sectionTimber = new SectionTimber(ump, tmbr, nn + QString(":sectionTimber"), parent );
    };

    MaterialModel * materialModel;
    SectionCncr * sectionCncr;
    SectionTimber * sectionTimber;
    QGraphicsItemGroup * qGraphicsItem;
};

SectionTimberCncr::SectionTimberCncr( UnitMeasure * ump,
                                      MaterialModel * materialMod,
                                      const QString & nn,
                                      QObject * parent):
    Section( ump, NULL, nn, parent ),
    m_d( new SectionTimberCncrPrivate( ump, materialMod, nn, this ) ){

    *m_typeNameInternal = "SectionTimberCncr";
    typeName->setValue( trUtf8("Sezione Legno-Cls"));

    A->setReadOnly( true );
    Sy->setReadOnly( true );
    Sz->setReadOnly( true );
    Iyy->setReadOnly( true );
    Izz->setReadOnly( true );
    Iyz->setReadOnly( true );
    Sy3DyDz->setReadOnly( true );
    Sz3DyDz->setReadOnly( true );
    Syz2DyDz->setReadOnly( true );
    Sy2zDyDz->setReadOnly( true );

    connect( m_d->sectionCncr, &SectionCncr::sectionChanged, this, &SectionTimberCncr::updateReadOnlyVars );
    connect( m_d->sectionTimber, &SectionTimber::sectionChanged, this, &SectionTimberCncr::updateReadOnlyVars );
    connect( m_d->sectionCncr, &SectionCncr::sectionChanged, this, &SectionTimberCncr::sectionChanged );
    connect( m_d->sectionTimber, &SectionTimber::sectionChanged, this, &SectionTimberCncr::sectionChanged );
}

SectionTimberCncr::~SectionTimberCncr(){
}

SectionTimberCncr & SectionTimberCncr::operator=( SectionTimberCncr & val){
    if( this != &val ){
        this->Section::operator=( val );

        *(m_d->sectionCncr) = *(val.m_d->sectionCncr);
        *(m_d->sectionTimber) = *(val.m_d->sectionTimber);
    }
    return *this;
}

Section & SectionTimberCncr::operator=( Section & val){
    if( this != &val ){
        SectionTimberCncr * sectTR = dynamic_cast<SectionTimberCncr *>(&val);
        if( sectTR ){
            *this = *sectTR;
        }
        else {
            this->Section::operator=( val );
        }
    }
    return *this;
}

void SectionTimberCncr::translateNormal( double yTrans, double zTrans, bool suspSignals){
    if( suspSignals ){
        suspendSignals(true);
    }
    m_d->sectionCncr->translateNormal( yTrans, zTrans );
    m_d->sectionTimber->translateNormal( yTrans, zTrans );
    if( suspSignals ){
        updateReadOnlyVars();
        suspendSignals(false);
    }
}

void SectionTimberCncr::rotateNormal( double rot, bool suspSignals ){
    if( suspSignals ){
        suspendSignals(true);
    }
    m_d->sectionCncr->rotateNormal( rot );
    m_d->sectionTimber->rotateNormal( rot );
    if( suspSignals ){
        updateReadOnlyVars();
        suspendSignals(false);
    }
}

QString SectionTimberCncr::typeSectionNormal(){
    return QString("SectionTimberCncr");
}

QString SectionTimberCncr::typeSection(){
    return trUtf8("Sez. Legno-Cemento");
}

SectionCncr * SectionTimberCncr::sectionCncr(){
    return m_d->sectionCncr;
}

SectionTimber * SectionTimberCncr::sectionTimber(){
    return m_d->sectionTimber;
}

bool SectionTimberCncr::lmP1P2( double *l, double *m, double x1, double y1, double x2, double y2){
    if( x1 != x2 ){
        (*m) = (y2-y1) / (x2-x1);
        (*l) = y1 - (*m) * x1;
        return true;
    } else
        return false;
}

double SectionTimberCncr::NULSMinNormal(){
    // TODO
    return 0.0;
}

double SectionTimberCncr::NULSMaxNormal(){
    // TODO
    return 0.0;
}

double SectionTimberCncr::NULSNormal(double l, double m, QList<Point2DModel *> *sects){
    // TODO
    return 0.0;
}

void SectionTimberCncr::MULSNormal(double * yRet, double * zRet, double l, double m, QList<Point2DModel *> *sects){
    // TODO
    *yRet = 0.0; *zRet = 0.0;
    double y=0.0, z=0.0;
}

void SectionTimberCncr::MULSNormal(double * yRet, double *zRet,
                                   double NSd,
                                   double yCen, double zCen,
                                   double rot,
                                   QList<Point2DModel *> *sects,
                                   double accuracy){
    *yRet = 0.0;
    *zRet = 0.0;
    // TODO
}

void SectionTimberCncr::MULS( DoublePlus * yRet, DoublePlus *zRet, DoublePlus * NSd, Point2DPlus * cen, DoublePlus * rot,
                              QList<Point2DModel * > * sects, DoublePlus * accuracy){
    double yM = 0.0, zM = 0.0;
    double acc = 1.0e-6;
    if( accuracy ){ // accuracy != NULL
        acc = accuracy->valueNormal();
    }
    double yCen = 0.0, zCen = 0.0;
    if( cen ){
        yCen = cen->y->valueNormal();
        zCen = cen->z->valueNormal();
    }
    double r = 0.0;
    if( rot ){
        r = rot->valueNormal();
    }
    MULSNormal( &yM, &zM, NSd->valueNormal(), yCen, zCen, r, sects, acc);
    yRet->setValueNormal( yM );
    zRet->setValueNormal( zM );
}

QPolygonF SectionTimberCncr::MULSyMULSz( DoublePlus * NSd, Point2DPlus * cen, int propNP){
    int nP = propNP > 4 ? propNP: 4;
    double dRot = 2.0 * M_PI / nP;

    double yRet = 0.0, zRet = 0.0;
    double rot = 0.0;

    SectionTimberCncr sectRCncrRot(m_unitMeasure, m_d->materialModel, "", parent() );
    sectRCncrRot = *this;
    sectRCncrRot.translateNormal( -(cen->y->valueNormal()), -(cen->z->valueNormal()) );

    QPolygonF polygon;
    for( int i=0; i< (nP+1); i++){
        sectRCncrRot.MULSNormal( &yRet, &zRet, NSd->valueNormal(), 0.0, 0.0, 0.0 );
        rotate2D( &yRet, &zRet, rot );
        polygon << QPointF( yRet * m_unitMeasure->scale(UnitMeasure::loadM), zRet * m_unitMeasure->scale(UnitMeasure::loadM));

        sectRCncrRot.rotateNormal( -dRot);
        rot += dRot;
    }
    return polygon;
}

QPolygonF SectionTimberCncr::MULSN( Point2DPlus * cen, DoublePlus * rot, int propNP){
    // TODO
    QPolygonF polygon;

    return polygon;
}

QGraphicsItem * SectionTimberCncr::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsItemGroup;
        if( m_d->sectionCncr->qGraphicsItem() ){
            m_d->qGraphicsItem->addToGroup( m_d->sectionCncr->qGraphicsItem() );
        }
        if( m_d->sectionTimber->qGraphicsItem() ){
            m_d->qGraphicsItem->addToGroup( m_d->sectionTimber->qGraphicsItem() );
        }
    }
    return m_d->qGraphicsItem;
}

double SectionTimberCncr::ANormal(){
    return m_d->sectionCncr->A->valueNormal();
}

double SectionTimberCncr::SyNormal(){
    return m_d->sectionCncr->Sy->valueNormal();
}

double SectionTimberCncr::SzNormal(){
    return m_d->sectionCncr->Sz->valueNormal();
}

double SectionTimberCncr::IyyNormal(){
    return m_d->sectionCncr->Iyy->valueNormal();
}

double SectionTimberCncr::IzzNormal(){
    return m_d->sectionCncr->Izz->valueNormal();
}

double SectionTimberCncr::IyzNormal(){
    return m_d->sectionCncr->Iyz->valueNormal();
}

double SectionTimberCncr::Sz3DyDzNormal(){
    return m_d->sectionCncr->Sz3DyDz->valueNormal();
}

double SectionTimberCncr::Sy3DyDzNormal(){
    return m_d->sectionCncr->Sy3DyDz->valueNormal();
}

double SectionTimberCncr::Syz2DyDzNormal(){
    return m_d->sectionCncr->Syz2DyDz->valueNormal();
}

double SectionTimberCncr::Sy2zDyDzNormal(){
    return m_d->sectionCncr->Sy2zDyDz->valueNormal();
}

double SectionTimberCncr::pWNormal(){
    return m_d->sectionCncr->pWNormal();
}
