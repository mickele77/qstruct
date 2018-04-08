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
#include "sectionarc.h"

#include "point2dplus.h"
#include "doubleplus.h"
#include "qstringplus.h"

#include <QGraphicsPathItem>
#include <QPen>

#include "structmath.h"
#include <cmath>

class SectionArcPrivate{
public:
    SectionArcPrivate():
        qGraphicsItem(NULL){
    }
    ~SectionArcPrivate() {
        if( qGraphicsItem != NULL ){
            delete qGraphicsItem;
        }
    }
    QGraphicsPathItem * qGraphicsItem;
};

SectionArc::SectionArc( UnitMeasure * ump, Material * mat, const QString &nn, QObject * parent):
    SectionThinSimple( ump, mat, nn, parent ),
    m_d(new SectionArcPrivate()){

    initVar();
}


SectionArc::SectionArc(UnitMeasure * ump, const QXmlStreamAttributes & attrs, MaterialModel * mModel, SectionLoadPhaseModel *lModel, QObject * parent ):
    SectionThinSimple( ump, NULL, "", parent ),
    m_d(new SectionArcPrivate()){
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

SectionArc::~SectionArc(){
    delete m_d;
}

SectionArc & SectionArc::operator=( SectionArc & val ){
    if( &val != this){
        this->Section::operator=( val );

        t->setValueNormal( val.t->valueNormal() );
        startAngle->setValueNormal( val.startAngle->valueNormal() );
        sweepAngle->setValueNormal( val.sweepAngle->valueNormal() );
        C->y->setValueNormal( val.C->y->valueNormal() );
        C->z->setValueNormal( val.C->z->valueNormal() );
    }
    return *this;
}

SectionThinSimple & SectionArc::operator=( SectionThinSimple & v ){
    if( this != &v ){
        SectionArc * val = dynamic_cast<SectionArc *>(&v);
        if( val ){
            *this = *val;
        } else {
            this->Section::operator=( v );
        }
    }
    return *this;
}

SectionSpace::SectionType SectionArc::sectionType() {
    return SectionSpace::ArcSection;
}

void SectionArc::initVar() {
    *m_typeNameInternal = "SectionArc";
    typeName->setValue( trUtf8("Arco"));

    startAngle = new DoublePlus(0.0, "startAngle", m_unitMeasure, UnitMeasure::angle);
    startAngle->setRichName( trUtf8("α<span style=\" vertical-align:sub;\">i</span>"));
    startAngle->setToolTip( trUtf8("Angolo iniziale dell'arco"));
    // startAngle->setMinNormal( 0.0 );
    // startAngle->setMaxNormal( 2.0 * M_PI );
    addVarToContainer( startAngle );

    sweepAngle = new DoublePlus(0.0, "sweepAngle", m_unitMeasure, UnitMeasure::angle);
    sweepAngle->setRichName( trUtf8("Δα"));
    sweepAngle->setToolTip( trUtf8("Angolo spazzato dall'arco"));
    // sweepAngle->setMinNormal( 0.0 );
    addVarToContainer( sweepAngle );

    C = new Point2DPlus( "C", "yC", 0.0, "zC", 0.0, m_unitMeasure, UnitMeasure::sectL ) ;
    C->y->setRichName( trUtf8("y<span style=\" vertical-align:sub;\">C</span>"));
    C->y->setToolTip( trUtf8("Ascissa del centro dell'arco"));
    C->z->setRichName( trUtf8("z<span style=\" vertical-align:sub;\">C</span>"));
    C->z->setToolTip( trUtf8("Ordinata del centro dell'arco"));
    addVarToContainer( C );

    R = new DoublePlus( 0.0, "R", m_unitMeasure, UnitMeasure::sectL ) ;;
    R->setRichName( trUtf8("R"));
    R->setToolTip( trUtf8("Raggio dell'arco"));
    R->setMinNormal( 0.0 );
    addVarToContainer( R );

    L->setReadOnly( true );
    L->addConnectedVars( 2, R, sweepAngle );

    Sy->setReadOnly( true );
    Sy->clearConnectedVars();
    Sy->addConnectedVars( 5, startAngle, sweepAngle, t, R, C );

    Sz->setReadOnly( true );
    Sz->clearConnectedVars();
    Sz->addConnectedVars( 5, startAngle, sweepAngle, t, R, C );

    Iyy->setReadOnly( true );
    Iyy->clearConnectedVars();
    Iyy->addConnectedVars( 5, startAngle, sweepAngle, t, R, C );

    Izz->setReadOnly( true );
    Izz->clearConnectedVars();
    Izz->addConnectedVars( 5, startAngle, sweepAngle, t, R, C );

    Iyz->setReadOnly( true );
    Iyz->clearConnectedVars();
    Iyz->addConnectedVars( 5, startAngle, sweepAngle, t, R, C );

    Sy3DyDz->setReadOnly( true );
    Sy3DyDz->clearConnectedVars();
    Sy3DyDz->addConnectedVars( 5, startAngle, sweepAngle, t, R, C );

    Sz3DyDz->setReadOnly( true );
    Sz3DyDz->clearConnectedVars();
    Sz3DyDz->addConnectedVars( 5, startAngle, sweepAngle, t, R, C );

    Sy2zDyDz->setReadOnly( true );
    Sy2zDyDz->clearConnectedVars();
    Sy2zDyDz->addConnectedVars( 5, startAngle, sweepAngle, t, R, C );

    Syz2DyDz->setReadOnly( true );
    Syz2DyDz->clearConnectedVars();
    Syz2DyDz->addConnectedVars( 5, startAngle, sweepAngle, t, R, C );

    IT->setReadOnly( true );
    IT->clearConnectedVars();
    IT->addConnectedVars( 3, sweepAngle, R, t );

    G->setReadOnly( true );
    IGyy->setReadOnly( true );
    IGzz->setReadOnly( true );
    IGyz->setReadOnly( true );
    ICyy->setReadOnly( true );
    ICzz->setReadOnly( true );
    ICyz->setReadOnly( true );
    thetaC->setReadOnly( true );

    connect( C, &Point2DPlus::valueChanged, this, &SectionArc::updateQGraphics );
    connect( startAngle, &DoublePlus::valueChanged, this, &SectionArc::updateQGraphics );
    connect( sweepAngle, &DoublePlus::valueChanged, this, &SectionArc::updateQGraphics );
    connect( R, &DoublePlus::valueChanged, this, &SectionArc::updateQGraphics );
    connect( t, &DoublePlus::valueChanged, this, &SectionArc::updateQGraphics );
}

void SectionArc::translateNormal( double yTrans, double zTrans, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }
    C->translateNormal( yTrans, zTrans );
    if( suspSignals ){
        suspendSignals( false );
    }
}

void SectionArc::rotateNormal( double rot, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }
    C->rotateNormal( rot );
    startAngle->setValueNormal( startAngle->valueNormal() + rot );
    if( suspSignals ){
        suspendSignals( false );
    }
}


double SectionArc::LNormal() {
    return fabs(sweepAngle->valueNormal()) * R->valueNormal();
}

QGraphicsItem * SectionArc::qGraphicsItem() {
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsPathItem();
        updateQGraphics();
    }
    return m_d->qGraphicsItem;
}

double SectionArc::SyNormal(){
    return sgn(sweepAngle->valueNormal()) *
            ( t->valueNormal() * R->valueNormal() *
              ( C->z->valueNormal() * sweepAngle->valueNormal() -
                R->valueNormal() * ( cos(startAngle->valueNormal()+sweepAngle->valueNormal()) - cos(startAngle->valueNormal()) ) ) );
}

double SectionArc::SzNormal(){
    return sgn(sweepAngle->valueNormal()) *
            ( t->valueNormal() * R->valueNormal() *
              (C->y->valueNormal() * sweepAngle->valueNormal() +
               R->valueNormal() * (sin(startAngle->valueNormal()+sweepAngle->valueNormal()) - sin(startAngle->valueNormal()) ) ) );
}

double SectionArc::IzzNormal(){
    return sgn(sweepAngle->valueNormal()) *
            ( t->valueNormal() * R->valueNormal() *
              ( ( pow(C->y->valueNormal(), 2.0) + pow(R->valueNormal(), 2.0) / 2.0) * sweepAngle->valueNormal() +
                2.0 * R->valueNormal() * C->y->valueNormal() * (sin(startAngle->valueNormal()+sweepAngle->valueNormal()) - sin(startAngle->valueNormal())) +
                ( pow(R->valueNormal(), 2.0) / 4.0 ) * (sin(2.0*(startAngle->valueNormal()+sweepAngle->valueNormal())) - sin(2.0*startAngle->valueNormal())) ) );
}

double SectionArc::IyyNormal(){
    return sgn(sweepAngle->valueNormal()) *
            ( t->valueNormal() * R->valueNormal() *
              ( (pow(C->z->valueNormal(),2.0) + pow(R->valueNormal(),2.0) / 2.0) * sweepAngle->valueNormal() -
                2.0 * R->valueNormal() * C->z->valueNormal() * (cos(startAngle->valueNormal()+sweepAngle->valueNormal()) - cos(startAngle->valueNormal())) -
                ( pow(R->valueNormal(), 2.0) / 4.0 ) * (sin(2.0*(startAngle->valueNormal()+sweepAngle->valueNormal())) - sin(2.0*startAngle->valueNormal())) ) );
}

double SectionArc::IyzNormal(){
    return  sgn(sweepAngle->valueNormal()) *
            ( t->valueNormal() * R->valueNormal() *
              ( C->y->valueNormal() * C->z->valueNormal() * sweepAngle->valueNormal() -
                R->valueNormal() * C->y->valueNormal() * (cos(startAngle->valueNormal()+sweepAngle->valueNormal()) - cos(startAngle->valueNormal())) +
                R->valueNormal() * C->z->valueNormal() * (sin(startAngle->valueNormal()+sweepAngle->valueNormal()) - sin(startAngle->valueNormal())) -
                pow(R->valueNormal(), 2.0) * (cos(2.0*(startAngle->valueNormal()+sweepAngle->valueNormal())) - cos(2.0*startAngle->valueNormal()) ) ) );
}

double SectionArc::Sz3DyDzNormal(){
    return  sgn(sweepAngle->valueNormal()) *
            ( t->valueNormal() * R->valueNormal() *
              (C->z->valueNormal() * ( pow(C->z->valueNormal(),2.0) + 3.0 * pow(R->valueNormal(),2.0) / 2.0) * sweepAngle->valueNormal() -
               R->valueNormal() * (3.0 * pow(C->z->valueNormal(),2.0) + pow(R->valueNormal(),2.0) ) * (cos(startAngle->valueNormal()+sweepAngle->valueNormal()) - cos(startAngle->valueNormal()) ) -
               3.0 * C->z->valueNormal() * pow(R->valueNormal(),2.0) * (sin(2.0 * (startAngle->valueNormal()+sweepAngle->valueNormal())) - sin(2.0 * startAngle->valueNormal())) / 4.0 +
               pow(R->valueNormal(),3.0) * ( pow(cos(startAngle->valueNormal()+sweepAngle->valueNormal()),3.0) - pow(cos(startAngle->valueNormal()),3.0) ) / 3.0 ) );
}

double SectionArc::Sy3DyDzNormal(){
    return  sgn(sweepAngle->valueNormal()) *
            ( t->valueNormal() * R->valueNormal() *
              (C->y->valueNormal() * ( pow(C->y->valueNormal(),2.0) + 3.0 * pow(R->valueNormal(),2.0) / 2.0) * sweepAngle->valueNormal() +
               R->valueNormal() * (3.0 * pow(C->y->valueNormal(),2.0) + pow(R->valueNormal(),2.0) ) * (sin(startAngle->valueNormal()+sweepAngle->valueNormal()) - sin(startAngle->valueNormal()) ) +
               3.0 * C->y->valueNormal() * pow(R->valueNormal(),2.0) * (sin(2.0 * (startAngle->valueNormal()+sweepAngle->valueNormal())) - sin(2.0 * startAngle->valueNormal())) / 4.0 -
               pow(R->valueNormal(),3.0) * ( pow(sin(startAngle->valueNormal()+sweepAngle->valueNormal()),3.0) - pow(sin(startAngle->valueNormal()),3.0) ) / 3.0 ) );
}

double SectionArc::Syz2DyDzNormal(){
    return  sgn(sweepAngle->valueNormal()) *
            ( t->valueNormal() * R->valueNormal() *
              (C->y->valueNormal() * ( pow(C->z->valueNormal(),2.0) + pow(R->valueNormal(),2.0) / 2.0) * sweepAngle->valueNormal() -
               2.0 * R->valueNormal() * C->y->valueNormal() * C->z->valueNormal() * (cos(startAngle->valueNormal()+sweepAngle->valueNormal()) - cos(startAngle->valueNormal()) ) +
               R->valueNormal() * pow(C->z->valueNormal(),2.0) * (sin(startAngle->valueNormal()+sweepAngle->valueNormal()) - sin(startAngle->valueNormal())) -
               pow(R->valueNormal(),2.0) * C->y->valueNormal() * (sin(2.0*(startAngle->valueNormal()+sweepAngle->valueNormal())) - sin(2.0*startAngle->valueNormal())) / 4.0 -
               pow(R->valueNormal(),2.0) * C->z->valueNormal() * (cos(2.0*(startAngle->valueNormal()+sweepAngle->valueNormal())) - cos(2.0*startAngle->valueNormal())) / 2.0 +
               pow(R->valueNormal(),3.0) * ( pow(sin(startAngle->valueNormal()+sweepAngle->valueNormal()),3.0) - pow(sin(startAngle->valueNormal()),3.0) ) / 3.0 ) );
}

double SectionArc::Sy2zDyDzNormal(){
    return  sgn(sweepAngle->valueNormal()) *
            ( t->valueNormal() * R->valueNormal() *
              (C->z->valueNormal() * ( pow(C->y->valueNormal(),2.0) + pow(R->valueNormal(),2.0) / 2.0) * sweepAngle->valueNormal() +
               2.0 * R->valueNormal() * C->y->valueNormal() * C->z->valueNormal() * (sin(startAngle->valueNormal()+sweepAngle->valueNormal()) - sin(startAngle->valueNormal()) ) -
               R->valueNormal() * pow(C->y->valueNormal(),2.0) * (cos(startAngle->valueNormal()+sweepAngle->valueNormal()) - cos(startAngle->valueNormal())) +
               pow(R->valueNormal(),2.0) * C->z->valueNormal() * (sin(2.0*(startAngle->valueNormal()+sweepAngle->valueNormal())) - sin(2.0*startAngle->valueNormal())) / 4.0 -
               pow(R->valueNormal(),2.0) * C->y->valueNormal() * (cos(2.0*(startAngle->valueNormal()+sweepAngle->valueNormal())) - cos(2.0*startAngle->valueNormal())) / 2.0 -
               pow(R->valueNormal(),3.0) * ( pow(cos(startAngle->valueNormal()+sweepAngle->valueNormal()),3.0) - pow(cos(startAngle->valueNormal()),3.0) ) / 3.0 ) );
}

double SectionArc::ITNormal(){
    double l =  fabs(sweepAngle->valueNormal()) * R->valueNormal();
    return pow( t->valueNormal(), 3.0) * l / 3.0;
}

void SectionArc::updateQGraphics() {
    if( m_d->qGraphicsItem != NULL ){
        QPen pen = m_d->qGraphicsItem->pen();
        pen.setWidthF( t->value() );
        m_d->qGraphicsItem->setPen( pen );

        QPainterPath path;
        path.moveTo( C->y->value() + R->value() * cos(startAngle->valueNormal()), C->z->value() + R->value() * sin(startAngle->valueNormal()));
        path.arcTo( C->y->value() - R->value(), C->z->value() - R->value(), 2.0 * R->value(), 2.0 * R->value(), -startAngle->valueNormal() * 180.0 / M_PI, -sweepAngle->valueNormal() * 180.0 / M_PI );
        m_d->qGraphicsItem->setPath( path );
    }
}
