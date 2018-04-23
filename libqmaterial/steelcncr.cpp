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

#include "steelcncr.h"

#include <QString>
#include <QXmlAttributes>
#include <QMap>
#include <QCoreApplication>

#include "qstringplus.h"
#include "doubleplus.h"

#include <cmath>

class BAnchorConcreteEnumVal{
public:
    BAnchorConcreteEnumVal(SteelCncr::ModelFEps v, const QString & ns, const QString & s):
        val(v),
        normalStr(ns),
        str(s){
    }
    SteelCncr::ModelFEps val;
    QString normalStr;
    QString str;
};

class SteelCncrModelfEpsPlusPrivate{
    Q_DECLARE_TR_FUNCTIONS(SteelCncrModelfEpsPlusPrivate)
public:
    SteelCncrModelfEpsPlusPrivate(SteelCncr::ModelFEps v):
        value(v){
        enumList.append( BAnchorConcreteEnumVal( SteelCncr::horizontalTopBranch, "HorizontalTopBranch", trUtf8("Elastico-perfettamente plastico")) );
        enumList.append( BAnchorConcreteEnumVal( SteelCncr::inclinedTopBranch, "InclinedTopBranch", trUtf8("Elastoplastico-incrudente")) );
    }
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    SteelCncr::ModelFEps value;
    QList<BAnchorConcreteEnumVal> enumList;
};

SteelCncrModelfEpsPlus::SteelCncrModelfEpsPlus( SteelCncr::ModelFEps vv,
                                                const QString & nn,
                                                bool ro):
    EnumPlus( nn, ro ),
    m_d( new  SteelCncrModelfEpsPlusPrivate(vv) ){
}

SteelCncrModelfEpsPlus::SteelCncrModelfEpsPlus( SteelCncrModelfEpsPlus & val ):
    EnumPlus(val),
    m_d( new  SteelCncrModelfEpsPlusPrivate(val.value()) ) {
    m_d->value = val.m_d->value;
}

SteelCncrModelfEpsPlus::~SteelCncrModelfEpsPlus(){
    delete m_d;
}

QString SteelCncrModelfEpsPlus::typeVar(){
    return QString("SteelCncrModelULS");
}

SteelCncrModelfEpsPlus & SteelCncrModelfEpsPlus::operator=( SteelCncrModelfEpsPlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SteelCncrModelfEpsPlus::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SteelCncrModelfEpsPlus * valSteelCncrModelPlus = dynamic_cast<SteelCncrModelfEpsPlus *>(&val);
        if(valSteelCncrModelPlus) {
            setValueNormal( valSteelCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

SteelCncr::ModelFEps SteelCncrModelfEpsPlus::value(){
    return m_d->value;
}

SteelCncr::ModelFEps SteelCncrModelfEpsPlus::valueNormal(){
    return m_d->value;
}

QString SteelCncrModelfEpsPlus::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SteelCncrModelfEpsPlus::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void SteelCncrModelfEpsPlus::setValueNormal( SteelCncr::ModelFEps vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void SteelCncrModelfEpsPlus::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SteelCncrModelfEpsPlus::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void SteelCncrModelfEpsPlus::setValue( SteelCncr::ModelFEps vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SteelCncrModelfEpsPlus::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int SteelCncrModelfEpsPlus::enumCount() {
    return m_d->enumList.size();
}

QString SteelCncrModelfEpsPlus::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SteelCncrModelfEpsPlus::valueNormalInt() {
    return m_d->valueIndex();
}

SteelCncr::SteelCncr(UnitMeasure * ump, const QString & n, QObject * parent ):
    Material( ump, n, parent ) {
    initVar();

    if( n.isEmpty() ){
        name->setValue( "B450C" );
    } else {
        name->setValue( n );
    }
    fyk->setRichName( "f<span style=\" vertical-align:sub;\">yk</span>");
    fyk->setValueNormal( 4.5e+8 );
    ftk->setRichName( "f<span style=\" vertical-align:sub;\">tk</span>");
    ftk->setValueNormal( 5.4e+8 );
    k->setValueNormal( 1.15 );
    E->setValueNormal( 2.0e+11 );
    nu->setValueNormal( 0.3 );
    epsUk->setValueNormal( 0.075 );
    ratioEpsUdEpsUk->setValueNormal( 0.90 );
}

SteelCncr::SteelCncr( SteelCncr & val ): Material( val ){
    initVar();
    *this = val;
}

SteelCncr::SteelCncr( SteelCncr * val ): Material( val ){
    initVar();
    *this = *val;
}

SteelCncr::SteelCncr(UnitMeasure * ump, const QXmlStreamAttributes &attrs, QObject * parent ):
    Material(ump, "", parent){
    initVar();
    loadFromXML( attrs );
}


SteelCncr::~SteelCncr(){
}

MaterialModel::MaterialType SteelCncr::materialType() {
    return MaterialModel::SteelCncrMaterial;
}

void SteelCncr::initVar(){
    *m_typeNameInternal = "SteelCncr";

    typeName->setValue( trUtf8("Acciaio C.A."));
    gammaW->setValueNormal( 7.85e+4 );
    alpha->setValueNormal( 1.0e-5);

    fyk = new DoublePlus( 0.0, "fyk", m_unitMeasure, UnitMeasure::tension );
    fyk->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">yk</span>") );
    addVarToContainer( fyk );

    gammaS = new DoublePlus( 1.15, "gammaS", m_unitMeasure, UnitMeasure::noDimension );
    gammaS->setRichName( trUtf8("γ<span style=\"vertical-align:sub;\">s</span>") );
    addVarToContainer( gammaS );

    fyd = new DoublePlus( 0.0, "fyd", m_unitMeasure, UnitMeasure::tension, true );
    fyd->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">yd</span>") );
    addVarToContainer( fyd );
    fyd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SteelCncr::setFyd) );
    fyd->addConnectedVars(2, fyk, gammaS);

    ftk = new DoublePlus( 0.0, "ftk", m_unitMeasure, UnitMeasure::tension );
    ftk->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">tk</span>") );
    addVarToContainer( ftk );

    k = new DoublePlus( 1.15, "k", m_unitMeasure, UnitMeasure::noDimension );
    addVarToContainer( k );

    fRare = new DoublePlus( 0.0, "fRare", m_unitMeasure, UnitMeasure::tension, true );
    fRare->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">rara</span>") );
    addVarToContainer( fRare );
    fRare->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SteelCncr::setFRare) );
    fRare->addConnectedVars(1, fyk);

    epsYd = new DoublePlus( 0.0, "epsYd", m_unitMeasure, UnitMeasure::deformation, true );
    epsYd->setRichName( trUtf8("ε<span style=\"vertical-align:sub;\">yd</span>") );
    addVarToContainer( epsYd );
    epsYd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SteelCncr::setEpsYd) );
    epsYd->addConnectedVars(2, fyd, E);

    epsUk = new DoublePlus( 0.0, "epsUk", m_unitMeasure, UnitMeasure::deformation );
    epsUk->setRichName( trUtf8("ε<span style=\"vertical-align:sub;\">uk</span>") );
    addVarToContainer( epsUk );

    ratioEpsUdEpsUk = new DoublePlus( 0.0, "ratioEpsUdEpsUk", m_unitMeasure, UnitMeasure::noDimension );
    ratioEpsUdEpsUk->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">ud</span>/ε<span style=\" vertical-align:sub;\">uk</span>") );
    addVarToContainer( ratioEpsUdEpsUk );

    epsUd = new DoublePlus( 0.0, "epsUd", m_unitMeasure, UnitMeasure::deformation, true );
    epsUd->setRichName( trUtf8("ε<span style=\"vertical-align:sub;\">ud</span>") );
    addVarToContainer( epsUd );
    epsUd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SteelCncr::setEpsUd) );
    epsUd->addConnectedVars(2, epsUk, ratioEpsUdEpsUk);

    fud = new DoublePlus( 0.0, "fud", m_unitMeasure, UnitMeasure::tension, true );
    fud->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">ud</span>") );
    addVarToContainer( fud );
    fud->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SteelCncr::setFud) );
    fud->addConnectedVars(4, fyd, k, E, epsUd);

    modelfEps = new SteelCncrModelfEpsPlus( SteelCncr::horizontalTopBranch, "modelfEps" );
    addVarToContainer( modelfEps );

    updateReadOnlyVars();
}

double SteelCncr::fULSNormal( double e ){
    double absE = fabs( e );
    double signE = (e < 0.0) ?  -1.0 : ( (e == 0.0 ) ? 0 : 1.0) ;
    if( absE > epsYd->valueNormal() ){
        if( modelfEps->valueNormal() == SteelCncr::inclinedTopBranch ){
            if( absE > epsUd->valueNormal() )
                return 0.0;
            else {
                return signE * (fyd->valueNormal() + ((fud->valueNormal() - fyd->valueNormal())/(epsUd->valueNormal() - epsYd->valueNormal() )) * (e - epsYd->valueNormal() ));
            }
        } else { // SteelCncr::defaultModelULS SteelCncr::horizontalTopBranch
            return (signE * fyd->valueNormal());
        }
    } else {
        return ( E->valueNormal() * e );
    }
    return 0.0;
}

bool SteelCncr::cracked( const double & e ){
    if( modelfEps->valueNormal() == SteelCncr::inclinedTopBranch ){
        if( fabs(e) > epsUd->valueNormal() )
            return true;
    }
    return false;
}

void SteelCncr::setFyd( bool emitAuto ) {
    fyd->setValueNormal( fyk->valueNormal() / gammaS->valueNormal(), emitAuto );
}

void SteelCncr::setFud( bool emitAuto ) {
    double ret = 0.0;
    if( E->valueNormal() != 0.0 ){
        double epsYd = fyd->valueNormal() / E->valueNormal();
        double D = epsUk->valueNormal() - epsYd;
        if( D != 0.0 ){
            ret = fyd->valueNormal() + (k->valueNormal() - 1.0 ) * fyd->valueNormal() * (epsUd->valueNormal() - epsYd) / D;
        }
    }
    fud->setValueNormal( ret, emitAuto );
}

void SteelCncr::setFRare( bool emitAuto ) {
    fRare->setValueNormal( 0.80 * fyk->valueNormal(), emitAuto );
}

void SteelCncr::setEpsYd( bool emitAuto ) {
    epsYd->setValueNormal( fyd->valueNormal() / E->valueNormal(), emitAuto );
}

void SteelCncr::setEpsUd( bool emitAuto ) {
    epsUd->setValueNormal( epsUk->valueNormal() * ratioEpsUdEpsUk->valueNormal(), emitAuto );
}
