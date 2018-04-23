#include "point2dplus.h"

#include "doubleplus.h"
#include "structmath.h"

#include <QXmlStreamAttributes>
#include <QMap>
#include <QStringList>
#include <cmath>

Point2DPlus::Point2DPlus( const QString & nn,
                          const QString & yN,
                          double yVal,
                          const QString & zN,
                          double zVal,
                          UnitMeasure * ump,
                          UnitMeasure::unitMeasure um,
                          bool rOnly):
    VarPlus(nn, ump, um, rOnly),
    y(new DoublePlus( yVal, yN, ump, um, rOnly )),
    z(new DoublePlus( zVal, zN, ump, um, rOnly )){
    connect(y, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
    connect(z, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
}

Point2DPlus::Point2DPlus( const QXmlStreamAttributes & attrs,
                          const QString & yN,
                          const QString & zN,
                          UnitMeasure * ump,
                          UnitMeasure::unitMeasure um,
                          bool em):
    VarPlus("", ump, um, em),
    y(new DoublePlus( 0.0, yN, ump, um, em )),
    z(new DoublePlus( 0.0, zN, ump, um, em )){
    loadFromXML( attrs );
    connect(y, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
    connect(z, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
}

Point2DPlus::Point2DPlus( Point2DPlus & vv ):
    VarPlus( vv ),
    y(new DoublePlus( vv.y->valueNormal(), vv.y->name(), vv.m_unitMeasurePointer, vv.m_unitMeasureValue, vv.m_readOnly )),
    z(new DoublePlus( vv.z->valueNormal(), vv.z->name(), vv.m_unitMeasurePointer, vv.m_unitMeasureValue, vv.m_readOnly )){
    connect(y, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
    connect(z, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
}

Point2DPlus::~Point2DPlus(){
    VarPlus::deleteVarPlus( y );
    VarPlus::deleteVarPlus( z );
}

Point2DPlus & Point2DPlus::operator=( Point2DPlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        y->setValueNormal( val.y->valueNormal() );
        z->setValueNormal( val.z->valueNormal() );
    }
    return *this;
}

VarPlus & Point2DPlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        Point2DPlus *valPoint2D = dynamic_cast<Point2DPlus*>(&val);
        if( valPoint2D ) {
            *this = *valPoint2D;
        }
    }
    return *this;
}

Point2DPlus & Point2DPlus::operator+( Point2DPlus & val){
    *y = *y + *(val.y);
    *z = *z + *(val.z);
    return *this;
}

Point2DPlus & Point2DPlus::operator-( Point2DPlus &val ){
    *y = *y - *(val.y);
    *z = *z - *(val.z);
    return *this;
}

Point2DPlus & Point2DPlus::operator*( double l ){
    *y = *y * l;
    *z = *z * l;
    return *this;
}

bool Point2DPlus::operator==( Point2DPlus &val){
    return (*y == *(val.y)) && (*z == *(val.z));
}

void Point2DPlus::writeXml(QXmlStreamWriter *writer){
    writer->writeStartElement( "Point2d" );
    writer->writeAttribute( "name", name() );
    writer->writeAttribute( y->xmlAttribute() );
    writer->writeAttribute( z->xmlAttribute() );
    writer->writeEndElement();
}

void Point2DPlus::suspendSignals(bool susp) {
    // TODO
}

void Point2DPlus::loadFromXML( const QXmlStreamAttributes &attrs ){
    for ( int i=0; i<attrs.count(); i++ ) {
        if( attrs.at(i).name().toString().toUpper() == "NAME" ){
            *m_name = attrs.at(i).value().toString();
        } else if( attrs.at(i).name().toString().toUpper() == y->name().toUpper() ){
            y->setValueNormal( attrs.at(i).value().toString() );
        } else if( attrs.at(i).name().toString().toUpper() == z->name().toUpper() ){
            z->setValueNormal( attrs.at(i).value().toString() );
        }
    }
}

void Point2DPlus::normalize(){
    double l = length2D( 0.0, 0.0, y->valueNormal(), z->valueNormal() );
    if( l != 0.0 ){
        y->setValueNormal( y->valueNormal() / l );
        z->setValueNormal( z->valueNormal() / l );
    }
}

QString Point2DPlus::typeVar(){
    return QString("Point2D");
}

QString Point2DPlus::valueNormalStr(){
    return QString("(") + y->valueNormalStr() + QString(";") + z->valueNormalStr() + QString(")");
}

QString Point2DPlus::valueStr(){
    return QString("(") + y->valueStr() + QString(";") + z->valueStr() + QString(")");
}

void Point2DPlus::setValue( const QString & str, bool emitAuto ){
    QString strcp = str;
    strcp.remove( '(');
    strcp.remove( ')');
    QStringList coord = strcp.split(";");
    if( coord.count() >= 2 ){
        y->setValue( coord.at(0), emitAuto );
        z->setValue( coord.at(1), emitAuto );
    }
}

void Point2DPlus::setValueNormal( const QString & str, bool emitAuto ){
    QString strcp = str;
    strcp.remove( '(');
    strcp.remove( ')');
    QStringList coord = strcp.split(";");
    if( coord.count() >= 2 ){
        y->setValueNormal( coord.at(0), emitAuto );
        z->setValueNormal( coord.at(1), emitAuto );
    }
}

void Point2DPlus::setValue(double yVal, double zVal, bool emitAuto ){
    y->setValue( yVal, emitAuto );
    z->setValue( zVal, emitAuto );
}

void Point2DPlus::setValueNormal(double yVal, double zVal, bool emitAuto ){
    y->setValueNormal( yVal, emitAuto );
    z->setValueNormal( zVal, emitAuto );
}

void Point2DPlus::setReadOnly( bool f ){
    VarPlus::setReadOnly( f );
    y->setReadOnly( f );
    z->setReadOnly( f );
}

void Point2DPlus::emitValueChanged(){
    if( !m_signalsSuspended ){
        emit valueChanged( valueStr() );
    }
}

void Point2DPlus::translate( Point2DPlus * trans){
    translateNormal( trans->y->valueNormal(), trans->z->valueNormal());
}

void Point2DPlus::translateNormal( double Dy, double Dz){
    y->setValueNormal( y->valueNormal() + Dy );
    z->setValueNormal( z->valueNormal() + Dz );
}

void Point2DPlus::rotate( DoublePlus * rot){
    rotateNormal( rot->valueNormal() );
}

void Point2DPlus::rotateNormal( double rot){
    double yTmp = y->valueNormal();
    double zTmp = z->valueNormal();
    rotate2D( &yTmp, &zTmp, rot );
    y->setValueNormal( yTmp );
    z->setValueNormal( zTmp );
}

void Point2DPlus::updateValue(bool emitAuto) {
    y->updateValue( emitAuto );
    z->updateValue( emitAuto );
}

