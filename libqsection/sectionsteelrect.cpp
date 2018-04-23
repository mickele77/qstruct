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
#include "sectionsteelrect.h"

#include "point2dmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"

#include <QGraphicsPolygonItem>
#include <QBrush>
#include <QObject>
#include <cmath>

class SectionSteelRectPrivate{
public:
    SectionSteelRectPrivate( DoublePlus ** p ) {
        np = 2;

        l = p[0];
        l->setToolTip( QObject::trUtf8("lunghezza"));
        l->setRichName( QObject::trUtf8("l"));
        l->setUnitMeasure( UnitMeasure::sectL );
        l->setEnabled( true );

        t = p[1];
        t->setToolTip( QObject::trUtf8("Spessore"));
        t->setRichName( QObject::trUtf8("t"));
        t->setUnitMeasure( UnitMeasure::sectL );
        t->setEnabled( true );

        qGraphicsItem = NULL;
    }
    ~SectionSteelRectPrivate(){
        if( qGraphicsItem ){
            delete qGraphicsItem;
        }
    }

    int np;
    DoublePlus * l;
    DoublePlus * t;

    QGraphicsPolygonItem * qGraphicsItem;
};

SectionSteelRect::SectionSteelRect(DoublePlus ** p, Steel * st, UnitMeasure * ump) :
    SectionSteelProfile(st, ump),
    m_d( new SectionSteelRectPrivate(p) ) {
    connect( m_d->t, &DoublePlus::valueChanged, this, &SectionSteelRect::updateSectionSteelProfile );
    connect( m_d->l, &DoublePlus::valueChanged, this, &SectionSteelRect::updateSectionSteelProfile );
}

SectionSteelRect::~SectionSteelRect(){
    delete m_d;
}

SectionSteel::SectionShape SectionSteelRect::sectionShape() {
    return SectionSteel::sectionRect;
}

int SectionSteelRect::np(){
    return m_d->np;
}

double SectionSteelRect::ANormal(){
    return (m_d->l->valueNormal() * m_d->t->valueNormal());
}

double SectionSteelRect::SyNormal(){
    return 0.0;
}

double SectionSteelRect::SzNormal(){
    return 0.0;
}

double SectionSteelRect::IyyNormal(){
    return (m_d->l->valueNormal() * pow(m_d->t->valueNormal(),3.0) / 12.0);
}

double SectionSteelRect::IzzNormal(){
    return (m_d->t->valueNormal() * pow(m_d->l->valueNormal(),3.0) / 12.0);
}

double SectionSteelRect::IyzNormal(){
    return 0.0;
}

double SectionSteelRect::Sz3DyDzNormal(){
    return 0.0;
}

double SectionSteelRect::Sy3DyDzNormal(){
    return 0.0;
}

double SectionSteelRect::Syz2DyDzNormal(){
    return 0.0;
}

double SectionSteelRect::Sy2zDyDzNormal(){
    return 0.0;
}

double SectionSteelRect::tyNormal(){
    return 1.2;
}

double SectionSteelRect::tzNormal(){
    return 1.2;
}

double SectionSteelRect::tyzNormal(){
    return 0.0;
}

double SectionSteelRect::maxTNormal(){
    if( m_d->t->valueNormal() < m_d->l->valueNormal() ){
        return m_d->t->valueNormal();
    } else {
        return m_d->l->valueNormal();
    }
}

double SectionSteelRect::ITNormal(){
    double a = m_d->l->valueNormal();
    double b = m_d->t->valueNormal();
    if( a < b ){
        b = m_d->l->valueNormal();
        a = m_d->t->valueNormal();
    }
    if( a == 0.0 ){
        return 0.0;
    } else {
        return a * pow(b, 3.0) / (3.0 + 1.8 * b/a);
    }
}

double SectionSteelRect::IwNormal(){
    // TODO
    return 0.0;
}

double SectionSteelRect::WyNormal(){
    return m_d->l->valueNormal() * pow(m_d->t->valueNormal(), 2.0) / 6.0;
}

double SectionSteelRect::WzNormal(){
    return pow(m_d->l->valueNormal(),2.0) * m_d->t->valueNormal() / 6.0;
}

double SectionSteelRect::WPlMyNormal(){
    return m_d->l->valueNormal() * pow(m_d->t->valueNormal(), 2.0) / 4.0;
}

double SectionSteelRect::WPlMzNormal(){
    return pow(m_d->l->valueNormal(),2.0) * m_d->t->valueNormal() / 4.0;
}

double SectionSteelRect::WElMyNormal(){
    return WyNormal();
}

double SectionSteelRect::WElMzNormal(){
    return WzNormal();
}

void SectionSteelRect::EffNMinusNormal( double * AEff, double * yGEff, double * zGEff, double ){
    // TODO
    *AEff = 0.0;
    *yGEff = 0.0;
    *zGEff = 0.0;
}

double SectionSteelRect::WEffMyMaxNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelRect::WEffMyMinNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelRect::WEffMzMaxNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelRect::WEffMzMinNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelRect::AVyNormal(){
    return (m_d->l->valueNormal() * m_d->t->valueNormal()) / 1.2;
}

double SectionSteelRect::AVzNormal(){
    return (m_d->l->valueNormal() * m_d->t->valueNormal()) / 1.2;
}

QString SectionSteelRect::NClass(){
    return QString::number(3);
}

QString SectionSteelRect::MyClass(){
    return QString::number(3);
}

QString SectionSteelRect::MyMaxClass(){
    return MyClass();
}

QString SectionSteelRect::MyMinClass(){
    return MyClass();
}

QString SectionSteelRect::MzClass(){
    return MyClass();
}

QString SectionSteelRect::MzMaxClass(){
    return MzClass();
}

QString SectionSteelRect::MzMinClass(){
    return MzClass();
}

QGraphicsItem * SectionSteelRect::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsPolygonItem();
        m_d->qGraphicsItem->setBrush( QBrush( Qt::lightGray ));
        updateQGraphics();
    }
    return m_d->qGraphicsItem;
}

void SectionSteelRect::updateQGraphics(){
    if( m_d->qGraphicsItem ){
        QPolygonF polygon;
        double xp = 0.50 * m_d->l->value();
        double yp = 0.50 * m_d->t->value();
        polygon << QPointF(xp, yp) << QPointF(-xp, yp) << QPointF(-xp, -yp) << QPointF(xp, -yp);
        m_d->qGraphicsItem->setPolygon( polygon );
    }
}

void SectionSteelRect::updateSectionSteelProfile() {
    updateSectionPoints();
    updateQGraphics();

    if( !m_signalsSuspended ) {
        emit sectionSteelProfileChanged();
    }
}

void SectionSteelRect::classDiagram(QList<QPolygonF *> *, double NULSMax, double MyULSMax, double MzULSMax){
    // TODO
}

void SectionSteelRect::point( int i, double *y, double *z ){
    switch( i ){
    case 0:{
        *y = m_d->l->valueNormal() *0.50;
        *z = 0.0;
        break; }
    case 1:{
        *y = m_d->l->valueNormal() *0.50;
        *z = m_d->t->valueNormal() *0.50;
        break;}
    case 2:{
        *y = 0.0;
        *z = m_d->t->valueNormal() *0.50;
        break;}
    case 3:{
        *y = - m_d->l->valueNormal() *0.50;
        *z = m_d->t->valueNormal() *0.50;
        break;}
    case 4:{
        *y = - m_d->l->valueNormal() *0.50;
        *z = 0.0;
        break;}
    case 5:{
        *y = - m_d->l->valueNormal() *0.50;
        *z = - m_d->t->valueNormal() *0.50;
        break;}
    case 6:{
        *y = 0.0;
        *z = - m_d->t->valueNormal() *0.50;
        break;}
    case 7:{
        *y = m_d->l->valueNormal() *0.50;
        *z = - m_d->t->valueNormal() *0.50;
        break;}
    default:{
        *y = 0.0;
        *z = 0.0;
        break; }
    }
}

int SectionSteelRect::nPoints(){
    return 8;
}

void SectionSteelRect::updateSectionPoints(){
    if( m_sectionPoints == NULL ){
        m_sectionPoints = new Point2DModel();
    }
    m_sectionPoints->resize(4);
    double h = m_d->t->valueNormal();
    double b = m_d->l->valueNormal();
    m_sectionPoints->setYZNormal( 0, -b/2.0, -h/2.0 );
    m_sectionPoints->setYZNormal( 1, b/2.0, -h/2.0 );
    m_sectionPoints->setYZNormal( 2, b/2.0, h/2.0 );
    m_sectionPoints->setYZNormal( 3, -b/2.0, h/2.0 );
}
