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

#include "material.h"

#include "section.h"
#include "boolplus.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "uintplus.h"

#include <QXmlAttributes>
#include <QString>
#include <QMap>
#include <QList>

Material::Material( UnitMeasure * ump, const QString & n, QObject * parent ):
    VarPlusContainerPlus( ump, "Material", n, parent ){
    initVar();
}

Material::Material(UnitMeasure *ump, const QXmlStreamAttributes &attrs, QObject *parent):
    VarPlusContainerPlus( ump, "Material", "", parent ){
    initVar();
    loadFromXML( attrs );
}

MaterialModel::MaterialType Material::materialType() {
    return MaterialModel::GenericMaterial;
}

Material::Material( Material & val ):
    VarPlusContainerPlus( val.m_unitMeasure, *(val.m_typeNameInternal), val.name->valueStr(), val.parent() ) {
    initVar();
    *this = val;
}

Material::Material( Material * val ):
    VarPlusContainerPlus( val->m_unitMeasure, *(val->m_typeNameInternal), val->name->valueStr(), val->parent() ) {
    initVar();
    *this = *val;
}

Material::~Material() {
    VarPlus::deleteVarPlus( typeName );
}

Material & Material::operator=( Material & val){
    setUnitMeasure( val.unitMeasure() );
    QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin();
    while( iter != m_varContainer->end() ){
        **iter = *(val.m_varContainer->value( iter.key() ));
        iter++;
    }
    return *this;
}

void Material::initVar(){
    connect( this, &Material::containerChanged, this, &Material::materialChanged );

    m_sectionUsingContainer = new QList<Section *>;

    typeName = new QStringPlus( trUtf8("Generico"), "typeName", true );
    typeName->setRichName( trUtf8("Tipo") );

    E = new DoublePlus( 0.0, "E", m_unitMeasure, UnitMeasure::tension, false );
    E->setToolTip( trUtf8("Modulo di elasticità  normale per il calcolo elastico"));
    E->setRichName( trUtf8("E"));
    addVarToContainer( E );

    nu = new DoublePlus( 0.0, "nu", m_unitMeasure, UnitMeasure::noDimension );
    nu->setReadOnly( false );
    nu->setToolTip( trUtf8("Modulo di Poisson per il calcolo elastico"));
    nu->setRichName( trUtf8("ν"));
    addVarToContainer( nu );

    G = new DoublePlus( 0.0, "G", m_unitMeasure, UnitMeasure::tension );
    G->setReadOnly( false );
    G->setToolTip( trUtf8("Modulo di elasticità  tangenziale per il calcolo elastico"));
    G->setRichName( trUtf8("G"));
    addVarToContainer( G );
    G->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Material::setG) );
    G->addConnectedVars( 2, E, nu );

    alpha = new DoublePlus( 1.0e-5, "alpha", m_unitMeasure, UnitMeasure::alphaTemp );
    alpha->setReadOnly( false );
    alpha->setRichName( trUtf8("α") );
    addVarToContainer( alpha );

    gammaW = new DoublePlus( 1.0e+4, "gammaW", m_unitMeasure, UnitMeasure::loadFVolume );
    gammaW->setReadOnly( false );
    gammaW->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">w</span>") );
    addVarToContainer( gammaW );

    isExisting = new BoolPlus( false, "isExisting" );
    isExisting->setRichName( trUtf8("esistente") );
    addVarToContainer( isExisting );

    FC = new DoublePlus( 1.35, "FC", m_unitMeasure, UnitMeasure::noDimension );
    FC->setReadOnly( false );
    FC->setRichName( trUtf8("FC") );
    addVarToContainer( FC );
}

void Material::fULS( DoublePlus * ret, DoublePlus * e ){
    ret->setValueNormal( fULSNormal(e->valueNormal() ));
}

double Material::fSLSNormal(double e) {
    return E->valueNormal() * e;
}

double Material::fULSNormal( double ){
    return 0.0;
}

void Material::setG(bool emitAuto ){
    G->setValueNormal( E->valueNormal() / (2.0 * (1.0 + nu->valueNormal() ) ), emitAuto);
}

/*void Material::updateReadOnlyVars(bool emitAuto) {
    G->updateValue( emitAuto );
}*/
