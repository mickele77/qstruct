#include "line2dplus.h"

#include "point2dplus.h"
#include "doubleplus.h"
#include "structmath.h"

#include <cmath>
#include <QGraphicsLineItem>
#include <QStringList>
#include <QPen>

class Line2DPlusPrivate{
public:
    Line2DPlusPrivate(double y1Val,
                      double z1Val,
                      double y2Val,
                      double z2Val,
                      UnitMeasure * ump,
                      UnitMeasure::unitMeasure um,
                      bool em){
        qGraphicsItem = NULL;
        P1 = new Point2DPlus( "P1", "y1", y1Val, "z1", z1Val, ump, um, em );
        P2 = new Point2DPlus( "P2", "y2", y2Val, "z2", z2Val, ump, um, em );
    };
    ~Line2DPlusPrivate(){
        VarPlus::deleteVarPlus( P1 );
        VarPlus::deleteVarPlus( P2 );
    }

    QGraphicsLineItem * qGraphicsItem;
    Point2DPlus * P1;
    Point2DPlus * P2;
};

Line2DPlus::Line2DPlus(  const QString & nn,
                         double y1Val,
                         double z1Val,
                         double y2Val,
                         double z2Val,
                         UnitMeasure * ump,
                         UnitMeasure::unitMeasure um,
                         bool em):
    VarPlus(nn, ump, um, em),
    m_d( new Line2DPlusPrivate(y1Val, z1Val, y2Val, z2Val, ump, um, em) ){
    connect(m_d->P1, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
    connect(m_d->P2, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
}

Line2DPlus::Line2DPlus( Line2DPlus & vv ):
    VarPlus(vv),
    m_d( new Line2DPlusPrivate(vv.m_d->P1->y->valueNormal(), vv.m_d->P1->z->valueNormal(),
                               vv.m_d->P2->y->valueNormal(), vv.m_d->P2->z->valueNormal(),
                               vv.m_unitMeasurePointer, vv.m_unitMeasureValue, vv.readOnly()) ){
    connect(m_d->P1, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
    connect(m_d->P2, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
}

Line2DPlus::~Line2DPlus(){
    delete m_d;
}

Line2DPlus & Line2DPlus::operator=( Line2DPlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        *(m_d->P1) = *(val.m_d->P1);
        *(m_d->P2) = *(val.m_d->P2);
    }
    return *this;
}

VarPlus & Line2DPlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        Line2DPlus *valLine2D = dynamic_cast<Line2DPlus*>(&val);
        if(valLine2D) {
            *(m_d->P1) = *(valLine2D->m_d->P1);
            *(m_d->P2) = *(valLine2D->m_d->P2);
        }
    }
    return *this;
}

QString Line2DPlus::typeVar(){
    return QString("Line2D");
}

QString Line2DPlus::valueNormalStr(){
    return m_d->P1->valueNormalStr() + QString(" - ") + m_d->P2->valueNormalStr();
}

QString Line2DPlus::valueStr(){
    return m_d->P1->valueStr() + QString(" - ") + m_d->P2->valueStr();
}

void Line2DPlus::setValue( const QString & str ){
    QStringList coord = str.split(" - ");
    if( coord.count() > 2 ){
        m_d->P1->setValue( coord.at(0) );
        m_d->P2->setValue( coord.at(1));
    }
}

void Line2DPlus::setValueNormal( const QString & str ){
    QStringList coord = str.split(" - ");
    if( coord.count() > 2 ){
        m_d->P1->setValueNormal( coord.at(0) );
        m_d->P2->setValueNormal( coord.at(1));
    }
}

void Line2DPlus::setReadOnly( bool f ){
    VarPlus::setReadOnly( f );
    m_d->P1->setReadOnly( f );
    m_d->P2->setReadOnly( f );
}

void Line2DPlus::emitValueChanged(){
    if( !m_signalsSuspended ){
        emit valueChanged( valueStr() );
    }
}

void Line2DPlus::translate( Point2DPlus * trans){
    m_d->P1->translate( trans );
    m_d->P2->translate( trans );
}

void Line2DPlus::translateNormal( double Dy, double Dz){
    m_d->P1->translateNormal( Dy, Dz );
    m_d->P2->translateNormal( Dy, Dz );
}

void Line2DPlus::rotate( DoublePlus * rot){
    m_d->P1->rotate( rot );
    m_d->P2->rotate( rot );
}

void Line2DPlus::rotateNormal( double rot){
    m_d->P1->rotateNormal( rot );
    m_d->P2->rotateNormal( rot );
}

Point2DPlus * Line2DPlus::P1(){
    return m_d->P1;
}

Point2DPlus * Line2DPlus::P2(){
    return m_d->P2;
}

QGraphicsItem * Line2DPlus::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsLineItem;
        updateGraphics();
        connect( m_d->P1, SIGNAL(valueChanged(QString)), this, SLOT(updateGraphics()) );
        connect( m_d->P2, SIGNAL(valueChanged(QString)), this, SLOT(updateGraphics()) );
    }
    return m_d->qGraphicsItem;
}

void Line2DPlus::updateGraphics(){
    if( m_d->qGraphicsItem != NULL ){
        m_d->qGraphicsItem->setLine( m_d->P1->y->valueNormal(), m_d->P1->z->valueNormal(), m_d->P2->y->valueNormal(), m_d->P2->z->valueNormal() );
        emit graphicsChanged();
    }
}

void Line2DPlus::setGraphicsWidth( double v ){
    if( m_d->qGraphicsItem != NULL ){
        m_d->qGraphicsItem->pen().setWidthF( v );
        emit graphicsChanged();
    }
}
