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
#include "sectionsteelcncr.h"

#include "sectionloadphasemodel.h"
#include "steelcncr.h"
#include "unitmeasure.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "qstringplus.h"

#include <QXmlStreamAttributes>
#include <QXmlStreamAttribute>
#include <QGraphicsEllipseItem>
#include <QBrush>

#include <cmath>

class SectionSteelCncrPrivate{
public:
    SectionSteelCncrPrivate():
        qGraphicsItem( NULL ){

    }
    ~SectionSteelCncrPrivate(){
        if( qGraphicsItem != NULL ){
            delete qGraphicsItem;
        }
    }

    QGraphicsEllipseItem * qGraphicsItem;
};

SectionSteelCncr::SectionSteelCncr( UnitMeasure * ump, SteelCncr * s, const QString &n, QObject * parent ):
    Section( ump, s, n, parent ),
    m_d( new SectionSteelCncrPrivate() ){
    initVar();
}

SectionSteelCncr::SectionSteelCncr(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent):
    Section( ump, NULL, "", parent ),
    m_d( new SectionSteelCncrPrivate() ){
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

SectionSteelCncr::~SectionSteelCncr(){
    delete m_d;
}

void SectionSteelCncr::initVar(){
    *m_typeNameInternal = "SectionSteelCncr";
    typeName->setValue( trUtf8("Armatura Acciaio C.A."));

    P = new Point2DPlus( "P", "y", 0.0, "z", 0.0, m_unitMeasure, UnitMeasure::sectL);
    P->setReadOnly( false );
    addVarToContainer(P);

    Sy->setReadOnly( true );
    Sy->addConnectedVars( 2, A, P);

    Sz->setReadOnly( true );
    Sz->addConnectedVars( 2, A, P);

    Iyy->setReadOnly( true );
    Iyy->addConnectedVars( 2, A, P);

    Izz->setReadOnly( true );
    Izz->addConnectedVars( 2, A, P);

    Iyz->setReadOnly( true );
    Iyz->addConnectedVars( 2, A, P);

    Sy3DyDz->setReadOnly( true );
    Sy3DyDz->addConnectedVars( 2, A, P);

    Sz3DyDz->setReadOnly( true );
    Sz3DyDz->addConnectedVars( 2, A, P);

    Syz2DyDz->setReadOnly( true );
    Syz2DyDz->addConnectedVars( 2, A, P);

    Sy2zDyDz->setReadOnly( true );
    Sy2zDyDz->addConnectedVars( 2, A, P);

    connect( A, &DoublePlus::valueChanged, this, &SectionSteelCncr::updateQGraphics );
    connect( P, &Point2DPlus::valueChanged, this, &SectionSteelCncr::updateQGraphics );
}

void SectionSteelCncr::loadFromXML( const QXmlStreamAttributes &attrs, MaterialModel * mModel, SectionLoadPhaseModel * loadModel ){
    for ( int i=0; i<attrs.count(); ++i ) {
        for( QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin(); iter != m_varContainer->end(); ++iter ){
            if ( attrs.at(i).name().toString().toUpper() == (*iter)->name().toUpper() ) {
                (*iter)->setValueNormal( attrs.at(i).value().toString(), false );
            } else if ( attrs.at(i).name().toString().toUpper() == "MATERIAL" ) {
                SteelCncr * steelCncr = dynamic_cast<SteelCncr *>(mModel->materialId( attrs.at(i).value().toString().toUInt() ));
                if( steelCncr == NULL ){
                    steelCncr = dynamic_cast<SteelCncr *>(mModel->firstMaterial( MaterialModel::SteelCncrMaterial ) );
                }
                if( steelCncr != NULL ){
                    setMaterial( steelCncr );
                }
            } else if ( attrs.at(i).name().toString().toUpper() == "LOADPHASE" ) {
                if( loadModel != NULL ){
                    setLoadPhase( loadModel->loadPhaseId( attrs.at(i).value().toString().toUInt() ), false );
                }
            }
        }
    }
    updateReadOnlyVars();
}

void SectionSteelCncr::translateNormal( double yTrans, double zTrans, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }
    P->translateNormal( yTrans, zTrans );
    if( suspSignals ){
        suspendSignals( false );
    }
}

void SectionSteelCncr::rotateNormal( double rot, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }
    P->rotateNormal( rot );
    if( suspSignals ){
        suspendSignals( false );
    }
}

double SectionSteelCncr::fSLSNormal(double l, double my, double mz) {
    if( m_material != NULL ){
        return m_material->E->valueNormal() * (l + my * P->y->valueNormal() + mz * P->z->valueNormal());
    }
    return 0.0;
}

void SectionSteelCncr::MULSNormal(double *yRet,
                                  double *zRet,
                                  double l,
                                  double my,
                                  QList<Point2DModel *> *sects ){
    Q_UNUSED(sects);
    double n = NULSNormal(l, my );
    *yRet = n * P->z->valueNormal();
    *zRet = - n * P->y->valueNormal();
}

double SectionSteelCncr::NULSNormal(double l,
                                    double my,
                                    QList<Point2DModel *> *sects){
    Q_UNUSED(sects);
    double e = l + my * P->z->valueNormal();
    return ( A->valueNormal() * steelCncr()->fULSNormal( e ) );
}

double SectionSteelCncr::NULSNormal(double l,
                                    double my,
                                    double mz,
                                    QList<Point2DModel *> *sects){
    Q_UNUSED(sects);
    double e = l + my * P->z->valueNormal() + mz * P->y->valueNormal();
    return ( A->valueNormal() * steelCncr()->fULSNormal( e ) );
}

void SectionSteelCncr::MULSNormal(double *yRet,
                                  double *zRet,
                                  double l,
                                  double my,
                                  double mz,
                                  QList<Point2DModel *> *sects ){
    Q_UNUSED(sects);
    double n = NULSNormal(l, my, mz );
    *yRet = n * P->z->valueNormal();
    *zRet = - n * P->y->valueNormal();
}

SteelCncr * SectionSteelCncr::steelCncr(){
    return dynamic_cast<SteelCncr *> (m_material);
}

void SectionSteelCncr::setMaterial( Material * mat){
    if( dynamic_cast<SteelCncr *>(mat) != NULL ){
        Section::setMaterial( mat );
    }
}

void SectionSteelCncr::setSteelCncr( SteelCncr * s){
    Section::setMaterial( s );
}

double SectionSteelCncr::SyNormal(){
    return ( A->valueNormal() * P->z->valueNormal());
}

double SectionSteelCncr::SzNormal(){
    return ( A->valueNormal() * P->y->valueNormal());
}

double SectionSteelCncr::IyyNormal(){
    return ( A->valueNormal() * pow(P->z->valueNormal(), 2.0) );
}

double SectionSteelCncr::IzzNormal(){
    return ( A->valueNormal() * pow(P->y->valueNormal(), 2.0) );
}

double SectionSteelCncr::IyzNormal(){
    return ( A->valueNormal() * P->y->valueNormal() * P->z->valueNormal() );
}

double SectionSteelCncr::Sz3DyDzNormal(){
    return ( A->valueNormal() * pow(P->z->valueNormal(), 3.0) );
}

double SectionSteelCncr::Sy3DyDzNormal(){
    return ( A->valueNormal() * pow(P->y->valueNormal(), 3.0) );
}

double SectionSteelCncr::Syz2DyDzNormal(){
    return ( A->valueNormal() * P->y->valueNormal() * pow(P->z->valueNormal(), 2.0) );
}

double SectionSteelCncr::Sy2zDyDzNormal(){
    return ( A->valueNormal() * pow(P->y->valueNormal(), 2.0) * P->z->valueNormal() );
}


QGraphicsItem * SectionSteelCncr::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsEllipseItem;
    }
    updateQGraphics();
    return m_d->qGraphicsItem;
}

void SectionSteelCncr::updateQGraphics(){
    if( m_d->qGraphicsItem != NULL ){
        double r = pow( A->value()/ M_PI, 0.50 );
        double x = P->y->value() - r;
        double y = P->z->value() - r;
        double w = 2.0 * r;
        double h = w;

        QBrush b( Qt::darkGray );

        m_d->qGraphicsItem->setBrush( b );
        m_d->qGraphicsItem->setRect(x, y, w, h);
    }
}
