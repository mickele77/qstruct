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
#include "sectionsteelcirc.h"

#include "point2dmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"

#include <QGraphicsPathItem>
#include <QBrush>
#include <QObject>
#include <cmath>

#define ARC_PREC 32

class SectionSteelCircPrivate{
public:
    SectionSteelCircPrivate( DoublePlus ** p ) {
        np = 2;

        de = p[0];
        de->setToolTip( QObject::trUtf8("Diametro esterno"));
        de->setRichName( QObject::trUtf8("d<span style=\" vertical-align:sub;\">e</span>"));
        de->setUnitMeasure( UnitMeasure::sectL );
        de->setEnabled( true );

        t = p[1];
        t->setToolTip( QObject::trUtf8("Spessore"));
        t->setRichName( QObject::trUtf8("t"));
        t->setUnitMeasure( UnitMeasure::sectL );
        t->setEnabled( true );

        qGraphicsItem = NULL;
    }
    ~SectionSteelCircPrivate(){
        if( qGraphicsItem ){
            delete qGraphicsItem;
        }
    }

    int np;
    DoublePlus * de;
    DoublePlus * t;

    QGraphicsPathItem * qGraphicsItem;
};

SectionSteelCirc::SectionSteelCirc( DoublePlus ** p, Steel * st, UnitMeasure * ump) :
    SectionSteelProfile( st, ump ),
    m_d( new SectionSteelCircPrivate(p) ) {
    connect( m_d->de, &DoublePlus::valueChanged, this, &SectionSteelProfile::updateSectionSteelProfile );
    connect( m_d->t, &DoublePlus::valueChanged, this, &SectionSteelProfile::updateSectionSteelProfile );
}

SectionSteelCirc::~SectionSteelCirc(){
    delete m_d;
}

SectionSteel::SectionShape SectionSteelCirc::sectionShape() {
    return SectionSteel::sectionCirc;
}

int SectionSteelCirc::np(){
    return m_d->np;
}

double SectionSteelCirc::ANormal(){
    double re = m_d->de->valueNormal()/2.0;
    double ri = re - m_d->t->valueNormal();
    return (M_PI * (re*re - ri*ri));
}

double SectionSteelCirc::SyNormal(){
    return 0.0;
}

double SectionSteelCirc::SzNormal(){
    return 0.0;
}

double SectionSteelCirc::IyyNormal(){
    double re = m_d->de->valueNormal()/2.0;
    double ri = re - m_d->t->valueNormal();
    return ( M_PI * (pow(re, 4.0) - pow(ri, 4.0)) / 4.0 );
}

double SectionSteelCirc::IzzNormal(){
    double re = m_d->de->valueNormal()/2.0;
    double ri = re - m_d->t->valueNormal();
    return ( M_PI * (pow(re, 4.0) - pow(ri, 4.0)) / 4.0 );
}

double SectionSteelCirc::IyzNormal(){
    return 0.0;
}

double SectionSteelCirc::Sz3DyDzNormal(){
    return 0.0;
}

double SectionSteelCirc::Sy3DyDzNormal(){
    return 0.0;
}

double SectionSteelCirc::Syz2DyDzNormal(){
    return 0.0;
}

double SectionSteelCirc::Sy2zDyDzNormal(){
    return 0.0;
}

double SectionSteelCirc::tyNormal(){
    return 1.2;
}

double SectionSteelCirc::tzNormal(){
    return 1.2;
}

double SectionSteelCirc::tyzNormal(){
    return 0.0;
}

double SectionSteelCirc::maxTNormal(){
    return m_d->t->valueNormal();
}

double SectionSteelCirc::ITNormal(){
    double re = m_d->de->valueNormal()/2.0;
    double ri = re - m_d->t->valueNormal();
    return ( M_PI * (pow(re, 4.0) - pow(ri, 4.0)) / 2.0 );
}

double SectionSteelCirc::IwNormal(){
    return 0.0;
}

double SectionSteelCirc::WElMyNormal(){
    if( m_d->de->valueNormal() == 0.0 ){
        return 0.0;
    } else {
        double re = m_d->de->valueNormal()/2.0;
        double ri = re - m_d->t->valueNormal();
        return M_PI * (pow( re, 4.0) - pow( ri, 4.0)) / ( 4.0 * re);
    }
}

double SectionSteelCirc::WElMzNormal(  ){
    return WElMyNormal();
}

double SectionSteelCirc::WPlMyNormal(){
    double re = m_d->de->valueNormal()/2.0;
    double ri = re - m_d->t->valueNormal();
    return ( 4.0 * (pow(re,3.0) - pow(ri, 3.0)) / 3.0 );
}

double SectionSteelCirc::WPlMzNormal(){
    return WPlMyNormal();
}

void SectionSteelCirc::EffNMinusNormal( double * AEff, double * yGEff, double * zGEff, double){
    // TODO
    *AEff = 0.0;
    *yGEff = 0.0;
    *zGEff = 0.0;
}

double SectionSteelCirc::WEffMyMaxNormal(double){
    // TODO
    return 0.0;
}

double SectionSteelCirc::WEffMyMinNormal(double){
    // TODO
    return 0.0;
}

double SectionSteelCirc::WEffMzMaxNormal(double){
    // TODO
    return 0.0;
}

double SectionSteelCirc::WEffMzMinNormal(double){
    // TODO
    return 0.0;
}

double SectionSteelCirc::AVyNormal(){
    // EC3 6.2.6
    double re = m_d->de->valueNormal()/2.0;
    double ri = re - m_d->t->valueNormal();
    return 2.0 * (re*re - ri*ri);
}

double SectionSteelCirc::AVzNormal(){
    return AVyNormal();
}

QString SectionSteelCirc::NClass(){
    if( fykNormal() != 0.0 && m_d->t->valueNormal() != 0.0 ){
        double eps = pow( 2.35e+8 /  fykNormal() , 0.50);
        double ratio = m_d->de->valueNormal() / m_d->t->valueNormal() / pow(eps, 2.0);
        if( ratio <= 50.0 ){
            return QString::number(1);
        } else if( ratio <= 70.0 ) {
            return QString::number(2);
        } else if( ratio <= 90.0 ) {
            return QString::number(3);
        } else {
            return QString::number(4);
        }
    } else {
        return QString::number(0);
    }
}

QString SectionSteelCirc::MyClass(){
    if( fykNormal() != 0.0 && m_d->t->valueNormal() != 0.0 ){
        double eps = pow( 2.35e+8 /  fykNormal() , 0.50);
        double ratio = m_d->de->valueNormal() / m_d->t->valueNormal() / pow(eps, 2.0);
        if( ratio <= 50.0 ){
            return QString::number(1);
        } else if( ratio <= 70.0 ) {
            return QString::number(2);
        } else if( ratio <= 90.0 ) {
            return QString::number(3);
        } else {
            return QString::number(4);
        }
    } else {
        return QString::number(0);
    }
}

QString SectionSteelCirc::MyMaxClass(){
    return MyClass();
}

QString SectionSteelCirc::MyMinClass(){
    return MyClass();
}

QString SectionSteelCirc::MzClass(){
    return MyClass();
}

QString SectionSteelCirc::MzMaxClass(){
    return MzClass();
}

QString SectionSteelCirc::MzMinClass(){
    return MzClass();
}

QGraphicsItem * SectionSteelCirc::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsPathItem();
        m_d->qGraphicsItem->setBrush( QBrush( Qt::lightGray ) );
        updateQGraphics();
    }
    return m_d->qGraphicsItem;
}

void SectionSteelCirc::updateQGraphics(){
    if( m_d->qGraphicsItem != NULL ){
        QPainterPath path;

        double re = m_d->de->value()/2.0;
        double ri = re - m_d->t->value();
        path.moveTo( re, 0 );
        path.arcTo( -re, -re, 2.0 * re, 2.0 * re, 0.0, 360.0 );
        path.moveTo( ri, 0 );
        path.arcTo( -ri, -ri, 2.0 * ri, 2.0 * ri, 0.0, 360.0 );

        m_d->qGraphicsItem->setPath( path );
    }
}

void SectionSteelCirc::updateSectionSteelProfile(){
    updateSectionPoints();
    updateQGraphics();

    if( !m_signalsSuspended ){
        emit sectionSteelProfileChanged();
    }
}

void SectionSteelCirc::updateSectionPoints(){
    if( m_sectionPoints == NULL ){
        m_sectionPoints = new Point2DModel();
    }
    m_sectionPoints->resize( 2*(ARC_PREC+1) );

    double re = m_d->de->value()/2.0;
    double ri = re - m_d->t->value();
    double dt = M_PI / ARC_PREC;
    double t = 0.0;
    for( int i=0; i < ARC_PREC; ++i ){
        m_sectionPoints->setYZNormal( i, re*cos(t), re*sin(t) );
        t += dt;
    }
    m_sectionPoints->setYZNormal( ARC_PREC, re, 0.0 );
    t = 0.0;
    for( int i=ARC_PREC+1; i < 2*ARC_PREC+1; ++i ){
        m_sectionPoints->setYZNormal( i, ri*cos(t), ri*sin(t) );
        t += dt;
    }
    m_sectionPoints->setYZNormal( 2*ARC_PREC+1, ri, 0.0 );
}

void SectionSteelCirc::classDiagram(QList<QPolygonF *> *, double NULSMax, double MyULSMax, double MzULSMax){
    // TODO
}

void SectionSteelCirc::point( int i, double * yCopy, double * zCopy){
    if( m_sectionPoints != NULL ){
        if( i < m_sectionPoints->nPoints() ){
            *yCopy = m_sectionPoints->yNormal(i);
            *zCopy = m_sectionPoints->zNormal(i);
        }
    }
}

int SectionSteelCirc::nPoints(){
    if( m_sectionPoints != NULL ){
        return m_sectionPoints->nPoints();
    }
    return 0;
}
