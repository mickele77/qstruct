#include "point3dplus.h"

#include "structmath.h"
#include "doubleplus.h"

#include <cmath>
#include <QStringList>

Point3DPlus::Point3DPlus(  const QString & nn,
                           const QString & xN,
                           double xVal,
                           const QString & yN,
                           double yVal,
                           const QString & zN,
                           double zVal,
                           UnitMeasure * ump,
                           UnitMeasure::unitMeasure um,
                           bool em):
    VarPlus(nn, ump, um, em),
    x(new DoublePlus( xVal, xN, ump, um, em )),
    y(new DoublePlus( yVal, yN, ump, um, em )),
    z(new DoublePlus( zVal, zN, ump, um, em )){
    connect(x, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
    connect(y, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
    connect(z, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
}

Point3DPlus::Point3DPlus( Point3DPlus & vv ):
    VarPlus(vv),
    x(new DoublePlus( vv.x->valueNormal(), vv.x->name(), vv.m_unitMeasurePointer, vv.m_unitMeasureValue, vv.m_readOnly )),
    y(new DoublePlus( vv.y->valueNormal(), vv.y->name(), vv.m_unitMeasurePointer, vv.m_unitMeasureValue, vv.m_readOnly )),
    z(new DoublePlus( vv.z->valueNormal(), vv.z->name(), vv.m_unitMeasurePointer, vv.m_unitMeasureValue, vv.m_readOnly )){
    connect(x, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
    connect(y, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
    connect(z, SIGNAL(valueChanged(QString)), this, SLOT(emitValueChanged()));
}

Point3DPlus::~Point3DPlus(){
    VarPlus::deleteVarPlus( x );
    VarPlus::deleteVarPlus( y );
    VarPlus::deleteVarPlus( z );
}

Point3DPlus & Point3DPlus::operator=( Point3DPlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        *x = *(val.x);
        *y = *(val.y);
        *z = *(val.z);
    }
    return *this;
}

VarPlus & Point3DPlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        Point3DPlus *valPoint3D = dynamic_cast<Point3DPlus*>(&val);
        if(valPoint3D) {
            *x = *(valPoint3D->x);
            *y = *(valPoint3D->y);
            *z = *(valPoint3D->z);
        }
    }
    return *this;
}

QString Point3DPlus::typeVar(){
    return QString("Point3D");
}

QString Point3DPlus::valueNormalStr(){
    return QString("(") + x->valueNormalStr() + QString(";") + y->valueNormalStr() + QString(";") + z->valueNormalStr() + QString(")");
}

QString Point3DPlus::valueStr(){
    return QString("(") + x->valueStr() + QString(";") + y->valueStr() + QString(";") + z->valueStr() + QString(")");
}

void Point3DPlus::setValue( const QString & str, bool emitAuto ){
    QString strcp = str;
    strcp.remove( '(');
    strcp.remove( ')');
    QStringList coord = strcp.split(";");
    if( coord.count() >= 3 ){
        z->setValue( coord.at(0), emitAuto );
        y->setValue( coord.at(1), emitAuto );
        z->setValue( coord.at(2), emitAuto );
    }
}

void Point3DPlus::setValueNormal( const QString & str, bool emitAuto ){
    QString strcp = str;
    strcp.remove( '(');
    strcp.remove( ')');
    QStringList coord = strcp.split(";");
    if( coord.count() >= 3 ){
        x->setValueNormal( coord.at(0), emitAuto );
        y->setValueNormal( coord.at(1), emitAuto );
        z->setValueNormal( coord.at(2), emitAuto );
    }
}

void Point3DPlus::setValue(double xVal, double yVal, double zVal, bool emitAuto ){
    x->setValue( xVal, emitAuto );
    y->setValue( yVal, emitAuto );
    z->setValue( zVal, emitAuto );
}

void Point3DPlus::setValueNormal(double xVal, double yVal, double zVal, bool emitAuto ){
    x->setValueNormal( xVal, emitAuto );
    y->setValueNormal( yVal, emitAuto );
    z->setValueNormal( zVal, emitAuto );
}

void Point3DPlus::setReadOnly( bool f ){
    VarPlus::setReadOnly( f );
    x->setReadOnly( f );
    y->setReadOnly( f );
    z->setReadOnly( f );
}

void Point3DPlus::emitValueChanged(){
    if( !m_signalsSuspended ){
        emit valueChanged( valueStr() );
    }
}

void Point3DPlus::translate( Point3DPlus * trans){
    translateNormal( trans->x->valueNormal(), trans->y->valueNormal(), trans->z->valueNormal());
}

void Point3DPlus::translateNormal( double Dx, double Dy, double Dz){
    x->setValueNormal( x->valueNormal() + Dx);
    y->setValueNormal( y->valueNormal() + Dy);
    z->setValueNormal( z->valueNormal() + Dz);
}
