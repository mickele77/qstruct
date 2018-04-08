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
#include "sectionsteell.h"

#include "steel.h"
#include "point2dmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "structmath.h"

#include <QList>
#include <QPolygonF>
#include <QGraphicsPathItem>
#include <QBrush>
#include <QObject>
#include <cmath>

#define ARC_PREC 4

class SectionSteelLPrivate {
public:
    SectionSteelLPrivate( DoublePlus ** p, UnitMeasure * ump ){
        np = 5;

        h = p[0];
        h->setToolTip( QObject::trUtf8("Altezza del profilo"));
        h->setRichName( QObject::trUtf8("h"));
        h->setUnitMeasure( UnitMeasure::sectL );
        h->setEnabled( true );

        b = p[1];
        b->setToolTip( QObject::trUtf8("Larghezza del profilo"));
        b->setRichName( QObject::trUtf8("b"));
        b->setUnitMeasure( UnitMeasure::sectL );
        b->setEnabled( true );

        t = p[2];
        t->setToolTip( QObject::trUtf8("Spessore dei due lati"));
        t->setRichName( QObject::trUtf8("t"));
        t->setUnitMeasure( UnitMeasure::sectL );
        t->setEnabled( true );

        r1 = p[3];
        r1->setToolTip( QObject::trUtf8("Raggio di raccordo tra i due lati"));
        r1->setRichName( QObject::trUtf8("r<span style=\" vertical-align:sub;\">1</span>"));
        r1->setUnitMeasure( UnitMeasure::sectL );
        r1->setEnabled( true );

        r2 = p[4];
        r2->setToolTip( QObject::trUtf8("Smusso dei bordi"));
        r2->setRichName( QObject::trUtf8("r<span style=\" vertical-align:sub;\">2</span>"));
        r2->setUnitMeasure( UnitMeasure::sectL );
        r2->setEnabled( true );

        qGraphicsItem = NULL;
    }
    ~SectionSteelLPrivate(){
        if( qGraphicsItem ){
            delete qGraphicsItem;
        }
    }

    // numero parametri
    int np;
    DoublePlus * h;
    DoublePlus * b;
    DoublePlus * t;
    DoublePlus * r1;
    DoublePlus * r2;

    QGraphicsPathItem * qGraphicsItem;
};

SectionSteelL::SectionSteelL( DoublePlus ** p, Steel * st, UnitMeasure * ump ):
    SectionSteelProfile( st, ump ),
    m_d( new SectionSteelLPrivate(p, ump )){
    connect( m_d->h, &DoublePlus::valueChanged, this, &SectionSteelL::updateSectionSteelProfile );
    connect( m_d->b, &DoublePlus::valueChanged, this, &SectionSteelL::updateSectionSteelProfile );
    connect( m_d->t, &DoublePlus::valueChanged, this, &SectionSteelL::updateSectionSteelProfile );
    connect( m_d->r1, &DoublePlus::valueChanged, this, &SectionSteelL::updateSectionSteelProfile );
    connect( m_d->r2, &DoublePlus::valueChanged, this, &SectionSteelL::updateSectionSteelProfile );
}

SectionSteelL::~SectionSteelL(){
    delete m_d;
}

SectionSteel::SectionShape SectionSteelL::sectionShape() {
    return SectionSteel::sectionL;
}

int SectionSteelL::np(){
    return m_d->np;
}

double SectionSteelL::ANormal(){
    return m_sectionPoints->ANormal();
}

double SectionSteelL::SyNormal(){
    return m_sectionPoints->SyNormal();
}

double SectionSteelL::SzNormal(){
    return m_sectionPoints->SzNormal();
}

double SectionSteelL::IyyNormal(){
    return m_sectionPoints->IyyNormal();
}

double SectionSteelL::IzzNormal(){
    return m_sectionPoints->IzzNormal();
}

double SectionSteelL::IyzNormal(){
    return m_sectionPoints->IyzNormal();
}

double SectionSteelL::Sz3DyDzNormal(){
    return m_sectionPoints->Sz3DyDzNormal();
}

double SectionSteelL::Sy3DyDzNormal(){
    return m_sectionPoints->Sy3DyDzNormal();
}

double SectionSteelL::Syz2DyDzNormal(){
    return m_sectionPoints->Syz2DyDzNormal();
}

double SectionSteelL::Sy2zDyDzNormal(){
    return m_sectionPoints->Sy2zDyDzNormal();
}

double SectionSteelL::maxTNormal(){
    return m_d->t->valueNormal();
}

double SectionSteelL::ITNormal(){
    // TODO
    return 0.0;
}


double SectionSteelL::IwNormal(){
    // TODO
    return 0.0;
}

double SectionSteelL::tyNormal(){
    return 1.2;
}

double SectionSteelL::tzNormal(){
    return 1.2;
}

double SectionSteelL::tyzNormal(){
    return 0.0;
}

double SectionSteelL::WElMyNormal(){
    if( m_d->h->valueNormal() == 0.0 ){
        return 0.0;
    } else {
        return ( IyyNormal() * 2.0 / m_d->h->valueNormal());
    }
}

double SectionSteelL::WElMzNormal(){
    if( m_d->b->valueNormal() == 0 ){
        return 0.0;
    } else {
        return ( IzzNormal() * 2.0 / m_d->b->valueNormal());
    }
}

double SectionSteelL::WPlMyNormal(){
    // TODO
    return 0.0;
}

double SectionSteelL::WPlMzNormal(){
    // TODO
    return 0.0;
}

void SectionSteelL::EffNMinusNormal( double * AEff, double * yGEff, double * zGEff, double){

    // TODO

    *AEff = ANormal();
    if( *AEff != 0.0 ){
        *yGEff = SzNormal() / *AEff;
        *zGEff = SyNormal() / *AEff;
    } else {
        *yGEff = 0.0;
        *zGEff = 0.0;
    }
}

double SectionSteelL::WEffMyMaxNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelL::WEffMyMinNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelL::WEffMzMaxNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelL::WEffMzMinNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelL::AVyNormal(){
    return (m_d->h->valueNormal() * m_d->t->valueNormal());
}

double SectionSteelL::AVzNormal(){
    return (m_d->b->valueNormal() * m_d->t->valueNormal());
}

QString SectionSteelL::NClass(){
    if( m_d->t->valueNormal() != 0 ) {
        double eps = pow( 2.35e+8 /  fykNormal(), 0.50 );
        if( (m_d->b->valueNormal() / m_d->t->valueNormal() < 15.0 * eps) &&
                (m_d->h->valueNormal() / m_d->t->valueNormal() < 15.0 * eps) &&
                (( m_d->b->valueNormal() + m_d->h->valueNormal() ) / (2.0 * m_d->t->valueNormal()) < 11.5 * eps)  ){
            return QString::number(3);
        }
    }
    return QString::number(4);
}

QString SectionSteelL::MyClass(){
    // TODO
    return QString::number(3);
}

QString SectionSteelL::MyMaxClass(){
    return MyClass();
}

QString SectionSteelL::MyMinClass(){
    return MyClass();
}

double SectionSteelL::MyClassLimitNormal(){
    // TODO
    return 0.0;
}

QString SectionSteelL::MzClass(){
    return QString::number(3);
}

QString SectionSteelL::MzMaxClass(){
    return MzClass();
}

QString SectionSteelL::MzMinClass(){
    return MzClass();
}

double SectionSteelL::MzClassLimitNormal(){
    // TODO
    return 0.0;
}

QGraphicsItem * SectionSteelL::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsPathItem();
        // m_d->qGraphicsItem->setBrush( QBrush( Qt::lightGray ));
        updateQGraphics();
    }
    return m_d->qGraphicsItem;
}

void SectionSteelL::updateQGraphics(){
    if( m_d->qGraphicsItem != NULL ){
        QPainterPath path;
        /* if( m_sectionPoints ){
            for( int i=0; i < m_sectionPoints->pCount(); ++i ){
                if( i == 0 ){
                    path.moveTo( m_sectionPoints->P(0)->y->value(), m_sectionPoints->P(0)->z->value() );
                } else {
                    path.lineTo( m_sectionPoints->P(i)->y->value(), m_sectionPoints->P(i)->z->value() );
                }
            }
            if( m_sectionPoints->pCount() > 0 ){
                path.lineTo( m_sectionPoints->P(0)->y->value(), m_sectionPoints->P(0)->z->value() );
            }
        } */
        if( m_d->r1->valueNormal() == 0 && m_d->r2->valueNormal() == 0 ){
            path.lineTo( 0.0, m_d->h->value() );
            path.lineTo( m_d->t->value(), m_d->h->value() );
            path.lineTo( m_d->t->value(), m_d->t->value() );
            path.lineTo( m_d->b->value(), m_d->t->value() );
            path.lineTo( m_d->b->value(), 0.0 );
            path.lineTo( 0.0, 0.0 );
        } else if( m_d->r1->valueNormal() == 0 ){
            path.lineTo( 0.0, m_d->h->value() );
            path.lineTo( m_d->t->value() - m_d->r2->value(), m_d->h->value() );
            path.arcTo( m_d->t->value() - 2.0 * m_d->r2->value(), m_d->h->value() - 2.0 * m_d->r2->value(), 2.0*m_d->r2->value(), 2.0 * m_d->r2->value(), -90.0, 90.0 );
            path.lineTo( m_d->t->value(), m_d->h->value() - m_d->r2->value() );
            path.lineTo( m_d->t->value(), m_d->t->value() );
            path.arcTo( m_d->b->value()-2.0*m_d->r2->value(), m_d->t->value() - 2.0*m_d->r2->value(), 2.0*m_d->r2->value(), 2.0*m_d->r2->value(), -90.0, 90.0 );
            path.lineTo( m_d->b->value(), m_d->t->value() - m_d->r2->value() );
            path.lineTo( m_d->b->value(), 0.0 );
            path.lineTo( 0.0, 0.0 );
        } else if( m_d->r2->valueNormal() == 0 ){
            path.lineTo( 0.0, m_d->h->value() );
            path.lineTo( m_d->t->value(), m_d->h->value() );
            path.arcTo( m_d->t->value(), m_d->t->value(), 2.0*m_d->r1->value(), 2.0*m_d->r1->value(), 180.0, -90.0 );
            path.lineTo( m_d->t->value() + m_d->r1->value(), m_d->t->value() );
            path.lineTo( m_d->b->value(), m_d->t->value() );
            path.lineTo( m_d->b->value(), 0.0 );
            path.lineTo( 0.0, 0.0 );
        } else {
            path.lineTo( 0.0, m_d->h->value() );
            path.lineTo( m_d->t->value() - m_d->r2->value(), m_d->h->value() );
            path.arcTo( m_d->t->value() - 2.0 * m_d->r2->value(), m_d->h->value() - 2.0 * m_d->r2->value(), 2.0*m_d->r2->value(), 2.0 * m_d->r2->value(), -90.0, 90.0 );
            path.arcTo( m_d->t->value(), m_d->t->value(), 2.0*m_d->r1->value(), 2.0*m_d->r1->value(), 180.0, -90.0 );
            path.arcTo( m_d->b->value()-2.0*m_d->r2->value(), m_d->t->value() - 2.0*m_d->r2->value(), 2.0*m_d->r2->value(), 2.0*m_d->r2->value(), -90.0, 90.0 );
            path.lineTo( m_d->b->value(), m_d->t->value() - m_d->r2->value() );
            path.lineTo( m_d->b->value(), 0.0 );
            path.lineTo( 0.0, 0.0 );
        }
        m_d->qGraphicsItem->setPath( path );
    }
}

void SectionSteelL::classDiagram( QList<QPolygonF *> * clDiagram, double NULSMax, double MyULSMax, double MzULSMax ){
    // TODO
}

void SectionSteelL::updateSectionSteelProfile() {
    updateSectionPoints();
    updateQGraphics();

    emit sectionSteelProfileChanged();
}

void SectionSteelL::updateSectionPoints(){
    if( m_sectionPoints == NULL ){
        m_sectionPoints = new Point2DModel();
    }
    int arc_prec_r1 = 0;
    if( m_d->r1->valueNormal() > 0.0 ){
        arc_prec_r1 = ARC_PREC;
    }
    int arc_prec_r2 = 0;
    if( m_d->r2->valueNormal() > 0.0 ){
        arc_prec_r2 = ARC_PREC;
    }
    int np = 3 + arc_prec_r2 + 1 + arc_prec_r1 + 1 + arc_prec_r2 + 1;
    m_sectionPoints->resize( np );

    double a, y0, z0;
    double da_r1 = 0.0;
    if( arc_prec_r1 != 0.0 ){
        da_r1 = M_PI / ( 2.0 * arc_prec_r1 );
    }
    double da_r2 = 0.0;
    if( arc_prec_r2 != 0.0 ){
        da_r2 = M_PI / ( 2.0 * arc_prec_r2 );
    }

    double h = m_d->h->valueNormal();
    double b = m_d->b->valueNormal();
    double t = m_d->t->valueNormal();
    double r1 = m_d->r1->valueNormal();
    double r2 = m_d->r2->valueNormal();

    m_sectionPoints->setYZNormal( 0, 0.0, 0.0 );
    m_sectionPoints->setYZNormal( 1, b, 0.0 );
    m_sectionPoints->setYZNormal( 2, b, t-r2 );
    y0 = b - r2;
    z0 = t - r2;
    a = da_r2;
    for( int i=0; i < arc_prec_r2; i++ ){
        m_sectionPoints->setYZNormal( 3+i, y0 + r2 * cos(a), z0 + r2 * sin(a) );
        a += da_r2;
    }
    m_sectionPoints->setYZNormal( 3+arc_prec_r2, t+r1, t);
    y0 = t + r1;
    z0 = t + r1;
    a = 3.0 * M_PI / 2.0 - da_r1;
    for( int i=0; i < arc_prec_r1; i++ ){
        m_sectionPoints->setYZNormal( 4+arc_prec_r2+i, y0 + r1 * cos(a), z0 + r1 * sin(a) );
        a -= da_r1;
    }
    m_sectionPoints->setYZNormal( 4+arc_prec_r2+arc_prec_r1, t, h-r2);
    a = da_r2;
    y0 = t - r2;
    z0 = h - r2;
    for( int i=0; i < arc_prec_r2; i++ ){
        m_sectionPoints->setYZNormal( 5+arc_prec_r2+arc_prec_r1+i, y0 + r2 * cos(a), z0 + r2 * sin(a) );
        a += da_r2;
    }
    m_sectionPoints->setYZNormal( 5+arc_prec_r2+arc_prec_r1+arc_prec_r2, 0, h);
}

void SectionSteelL::point( int i, double *y, double *z ){
    if( i < m_sectionPoints->nPoints() ){
        *y = m_sectionPoints->yNormal(i);
        *z = m_sectionPoints->zNormal(i);
    }
}

int SectionSteelL::nPoints(){
    return m_sectionPoints->nPoints();
}

