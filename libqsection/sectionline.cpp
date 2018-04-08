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

#include "sectionline.h"

#include "point2dplus.h"
#include "doubleplus.h"

#include "structmath.h"

#include "qstringplus.h"

#include <QGraphicsLineItem>
#include <QPen>
#include <cmath>


class SectionLinePrivate{
public:
    SectionLinePrivate():
        graphicsItem(NULL){
    }
    ~SectionLinePrivate(){
        if( graphicsItem != NULL ){
            delete graphicsItem;
        }
    }
    QGraphicsLineItem * graphicsItem;
};

SectionLine::SectionLine( UnitMeasure * ump, Material * mat, const QString &nn, QObject * parent ):
    SectionThinSimple( ump, mat, nn, parent ),
    m_d(new SectionLinePrivate()){

    initVar();
}

SectionLine::SectionLine( UnitMeasure * ump, const QXmlStreamAttributes & attrs, MaterialModel * mModel, SectionLoadPhaseModel * lModel, QObject * parent ):
    SectionThinSimple( ump, NULL, "", parent ),
    m_d(new SectionLinePrivate()){
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

SectionLine::~SectionLine(){
    delete m_d;
}

SectionLine & SectionLine::operator=( SectionLine & val ){
    if( &val != this){
        this->Section::operator=( val );

        t->setValueNormal( val.t->valueNormal() );
        P1->y->setValueNormal( val.P1->y->valueNormal() );
        P1->z->setValueNormal( val.P1->z->valueNormal() );
        P2->y->setValueNormal( val.P2->y->valueNormal() );
        P2->z->setValueNormal( val.P2->z->valueNormal() );
    }
    return *this;
}

SectionThinSimple & SectionLine::operator=( SectionThinSimple & v ){
    if( this != &v ){
        SectionLine * val = dynamic_cast<SectionLine *>(&v);
        if( val ){
            *this = *val;
        } else {
            this->Section::operator=( v );
        }
    }
    return *this;
}

void SectionLine::initVar() {
    *m_typeNameInternal = "SectionLine";
    typeName->setValue( trUtf8("Linea"));

    P1 = new Point2DPlus( "P1", "y1", 0.0, "z1", 0.0, m_unitMeasure, UnitMeasure::sectL ) ;
    P1->y->setRichName( trUtf8("y<span style=\" vertical-align:sub;\">1</span>"));
    P1->y->setToolTip( trUtf8("Ascissa del punto iniziale del segmento"));
    P1->z->setRichName( trUtf8("z<span style=\" vertical-align:sub;\">1</span>"));
    P1->z->setToolTip( trUtf8("Ordinata del punto iniziale del segmento"));
    addVarToContainer( P1 );

    P2 = new Point2DPlus( "P2", "y1", 0.0, "z1", 0.0, m_unitMeasure, UnitMeasure::sectL ) ;
    P2->y->setRichName( trUtf8("y<span style=\" vertical-align:sub;\">2</span>"));
    P2->y->setToolTip( trUtf8("Ascissa del punto finale del segmento"));
    P2->z->setRichName( trUtf8("z<span style=\" vertical-align:sub;\">2</span>"));
    P2->z->setToolTip( trUtf8("Ordinata del punto finale del segmento"));
    addVarToContainer( P2 );

    L->setReadOnly( true );
    L->addConnectedVars( 2, P1, P2 );

    Sy->setReadOnly( true );
    Sy->clearConnectedVars();
    Sy->addConnectedVars( 3, t, P1, P2 );

    Sz->setReadOnly( true );
    Sz->clearConnectedVars();
    Sz->addConnectedVars( 3, t, P1, P2 );

    Iyy->setReadOnly( true );
    Iyy->clearConnectedVars();
    Iyy->addConnectedVars( 3, t, P1, P2 );

    Izz->setReadOnly( true );
    Izz->addConnectedVars( 3, t, P1, P2 );

    Iyz->setReadOnly( true );
    Iyz->addConnectedVars( 3, t, P1, P2 );

    G->setReadOnly( true );
    G->y->clearConnectedVars();
    G->y->addConnectedVars( 2, P1, P2 );
    G->z->clearConnectedVars();
    G->z->addConnectedVars( 2, P1, P2 );

    IGyy->setReadOnly( true );
    IGyy->clearConnectedVars();
    IGyy->addConnectedVars( 3, t, P1, P2 );

    IGzz->setReadOnly( true );
    IGzz->clearConnectedVars();
    IGzz->addConnectedVars( 3, t, P1, P2 );

    IGyz->setReadOnly( true );
    IGyz->clearConnectedVars();
    IGyz->addConnectedVars( 3, t, P1, P2 );

    ICyy->setReadOnly( true );
    ICyy->clearConnectedVars();

    ICzz->setReadOnly( true );
    ICzz->clearConnectedVars();
    ICzz->addConnectedVars( 3, t, P1, P2 );

    ICyz->setReadOnly( true );
    ICyz->clearConnectedVars();

    thetaC->setReadOnly( true );
    thetaC->addConnectedVars( 2, P1, P2 );

    connect( P1, &Point2DPlus::valueChanged, this, &SectionLine::updateQGraphics );
    connect( P2, &Point2DPlus::valueChanged, this, &SectionLine::updateQGraphics );
    connect( t, &Point2DPlus::valueChanged, this, &SectionLine::updateQGraphics );
}

double SectionLine::LNormal() {
    return length2D(P1->y->valueNormal(), P1->z->valueNormal(), P2->y->valueNormal(), P2->z->valueNormal());
}

double SectionLine::GyNormal() {
    return (P1->y->valueNormal() + P2->y->valueNormal()) * 0.50;
}

double SectionLine::GzNormal() {
    return (P1->z->valueNormal() + P2->z->valueNormal()) * 0.50;
}

QGraphicsItem * SectionLine::qGraphicsItem() {
    if( m_d->graphicsItem == NULL ){
        m_d->graphicsItem = new QGraphicsLineItem();
        updateQGraphics();
    }
    return m_d->graphicsItem;
}

void SectionLine::updateQGraphics(){
    if( m_d->graphicsItem != NULL ){
        QPen pen = m_d->graphicsItem->pen();
        pen.setWidthF( t->value() );
        m_d->graphicsItem->setPen( pen );
        m_d->graphicsItem->setLine( P1->y->value(), P1->z->value(), P2->y->value(), P2->z->value() );
    }
}

void SectionLine::translateNormal( double yTrans, double zTrans, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }

    P1->translateNormal( yTrans, zTrans );
    P2->translateNormal( yTrans, zTrans );

    if( suspSignals ){
        suspendSignals( false );
    }
}

void SectionLine::rotateNormal( double rot, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }

    P1->rotateNormal( rot );
    P2->rotateNormal( rot );

    if( suspSignals ){
        suspendSignals( false );
    }
}

double SectionLine::SyNormal(){
    return ( t->valueNormal() * length2D(P1->y->valueNormal(), P1->z->valueNormal(), P2->y->valueNormal(), P2->z->valueNormal() ) *
             (P1->z->valueNormal() + P2->z->valueNormal() ) * 0.50 );
}

double SectionLine::SzNormal(){
    return ( t->valueNormal() * length2D(P1->y->valueNormal(), P1->z->valueNormal(), P2->y->valueNormal(), P2->z->valueNormal() ) *
             (P1->y->valueNormal() + P2->y->valueNormal() ) * 0.50 );
}

double SectionLine::IyyNormal(){
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = sqrt( Dy*Dy+Dz*Dz );
    return ( t->valueNormal() * l *
             (pow(P1->z->valueNormal(),2.0) + Dz * (P1->z->valueNormal() + Dz / 3.0 )) );
}

double SectionLine::IzzNormal(){
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = pow( Dy*Dy+Dz*Dz, 0.50 );
    return ( t->valueNormal() * l *
             (pow(P1->y->valueNormal(),2.0) + Dy * (P1->y->valueNormal() + Dy / 3.0 )) );
}

double SectionLine::IyzNormal(){
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = pow( Dy*Dy+Dz*Dz, 0.50 );
    return ( t->valueNormal() * l *
             ( P1->y->valueNormal() * P1->z->valueNormal() + (P1->y->valueNormal() * Dz + P1->z->valueNormal() * Dy ) / 2.0 + Dy * Dz / 3.0 ) );
}

double SectionLine::Sz3DyDzNormal(){
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = pow( Dy*Dy+Dz*Dz, 0.50 );
    return ( t->valueNormal() * l *
             (pow(P1->z->valueNormal(),3.0) +
              3.0 * pow(P1->z->valueNormal(),2.0) * Dz / 2.0 +
              P1->z->valueNormal() * pow(Dz,2.0) +
              pow(Dz,3.0) / 4.0 ) );
}

double SectionLine::Sy3DyDzNormal(){
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = pow( Dy*Dy+Dz*Dz, 0.50 );
    return ( t->valueNormal() * l *
             (pow(P1->y->valueNormal(),3.0) +
              3.0 * pow(P1->y->valueNormal(),2.0) * Dy / 2.0 +
              P1->y->valueNormal() * pow(Dy,2.0) +
              pow(Dy,3.0) / 4.0 ) );
}

double SectionLine::Syz2DyDzNormal(){
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = pow( Dy*Dy+Dz*Dz, 0.50 );
    return ( t->valueNormal() * l *
             (P1->y->valueNormal() * pow(P1->z->valueNormal(),2.0) +
              (2.0 * P1->y->valueNormal() * P1->z->valueNormal() * Dz + Dy * pow(P1->z->valueNormal(),2.0)) / 2.0 +
              (Dz * Dz * P1->y->valueNormal() + 2.0 * P1->z->valueNormal() * Dy * Dz) / 3.0 +
              Dy * Dz * Dz / 4.0 ) );
}

double SectionLine::Sy2zDyDzNormal(){
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = pow( Dy*Dy+Dz*Dz, 0.50 );
    return ( t->valueNormal() * l *
             (P1->z->valueNormal() * pow(P1->y->valueNormal(),2.0) +
              (2.0 * P1->y->valueNormal() * P1->z->valueNormal() * Dy + Dz * pow(P1->y->valueNormal(),2.0)) / 2.0 +
              (Dy * Dy * P1->z->valueNormal() + 2.0 * P1->y->valueNormal() * Dy * Dz) / 3.0 +
              Dy * Dy * Dz / 4.0 ) );
}

double SectionLine::ITNormal(){
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = sqrt( Dy*Dy+Dz*Dz );
    return pow( t->valueNormal(), 3.0) * l / 3.0;
}

double SectionLine::IGyyNormal(){
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = sqrt( Dy*Dy+Dz*Dz );
    double tC = atan2( Dz, Dy);
    if( l != 0.0 ){
        return t->valueNormal() * pow(l,3.0) * pow(sin(tC), 2.0) / 12.0;
    }
    return 0.0;
}

double SectionLine::IGzzNormal() {
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = sqrt( Dy*Dy+Dz*Dz );
    double tC = atan2( Dz, Dy);
    if( l != 0.0 ){
        return t->valueNormal() * pow(l,3.0) * pow(cos(tC), 2.0) / 12.0;
    }
    return 0.0;
}

double SectionLine::IGyzNormal() {
    double Dy = P2->y->valueNormal() - P1->y->valueNormal();
    double Dz = P2->z->valueNormal() - P1->z->valueNormal();
    double l = sqrt( Dy*Dy+Dz*Dz );
    double tC = atan2( Dz, Dy);
    if( l != 0.0 ){
        return t->valueNormal() * pow(l,3.0) * sin(tC) * cos(tC) / 12.0;
    }
    return 0.0;
}

double SectionLine::ICyyNormal() {
    return 0.0;
}

double SectionLine::ICzzNormal() {
    return t->valueNormal() * pow(L->valueNormal(), 3.0) / 12.0;
}

double SectionLine::ICyzNormal() {
    return 0.0;
}

double SectionLine::thetaCNormal() {
    double Dy = (P2->y->valueNormal() - P1->y->valueNormal());
    double Dz = (P2->z->valueNormal() - P1->z->valueNormal());
    return atan2( Dz, Dy);
}

double SectionLine::yMaxNormal() {
    return qMax( P1->y->valueNormal(), P2->y->valueNormal() );
}

double SectionLine::yMinNormal() {
    return qMin( P1->y->valueNormal(), P2->y->valueNormal() );
}

double SectionLine::zMaxNormal() {
    return qMax( P1->z->valueNormal(), P2->z->valueNormal() );
}

double SectionLine::zMinNormal() {
    return qMin( P1->z->valueNormal(), P2->z->valueNormal() );
}
