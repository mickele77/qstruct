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

#include "steel.h"

#include <QMap>
#include <QXmlAttributes>

#include "qstringplus.h"
#include "doubleplus.h"

#include <cmath>

Steel::Steel( UnitMeasure * ump, const QString &n, QObject *parent):
    Material( ump, n, parent ) {

    initVar();

    if( n.isEmpty() ){
        name->setValue( "S235" );
    } else {
        name->setValue( n );
    }
    E->setValueNormal( 2.10e+11 );
    nu->setValueNormal( 0.30);
    alpha->setValueNormal( 1.20e-5);
    ftk->setValueNormal( 3.60e+8);
    fyk->setValueNormal(2.35e+8);
    ftk40->setValueNormal( 3.60e+8);
    fyk40->setValueNormal(2.15e+8);
    gammaW->setValueNormal( 7.85e+4 );
}

Steel::Steel(UnitMeasure * ump, const QXmlStreamAttributes &attrs , QObject *parent):
    Material(ump, "", parent){
    initVar();
    loadFromXML( attrs);
}

MaterialModel::MaterialType Steel::materialType() {
    return MaterialModel::SteelMaterial;
}

double Steel::fULSNormal(double e) {
    if( E->valueNormal() != 0.0 ){
        double epsY = fyd(0)->valueNormal() / E->valueNormal();
        if( fabs( e ) <= epsY ){
            return E->valueNormal() * e;
        } else if( e > epsY ) {
            return fyd(0)->valueNormal();
        } else { //  e < epsY
            return -fyd(0)->valueNormal();
        }
    }
    return 0.0;
}

void Steel::initVar(  ){
    *m_typeNameInternal = "Steel";

    typeName->setValue( trUtf8("Acciaio"));
    G->setReadOnly( true );
    alpha->setValueNormal( 1.2e-5, false );
    gammaW->setValueNormal( 7.850e+4, false );

    fyk = new DoublePlus( 0.0, "fyk", m_unitMeasure, UnitMeasure::tension );
    fyk->setReadOnly( false );
    fyk->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">yk</span>") );
    addVarToContainer( fyk );


    fyk40 = new DoublePlus( 0.0, "fyk40", m_unitMeasure, UnitMeasure::tension );
    fyk40->setReadOnly( false );
    fyk40->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">yk,40</span>") );
    addVarToContainer( fyk40 );

    ftk = new DoublePlus( 0.0, "ftk", m_unitMeasure, UnitMeasure::tension );
    ftk->setReadOnly( false );
    ftk->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">tk</span>") );
    addVarToContainer( ftk );

    ftk40 = new DoublePlus( 0.0, "ftk40", m_unitMeasure, UnitMeasure::tension );
    ftk40->setReadOnly( false );
    ftk40->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">tk,40</span>") );
    addVarToContainer( ftk40 );

    m_gammaS[0] = new DoublePlus( 1.05, "gammaS0", m_unitMeasure, UnitMeasure::noDimension );
    m_gammaS[0]->setReadOnly( false );
    m_gammaS[0]->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">M0</span>") );
    m_gammaS[0]->setToolTip( trUtf8("Resistenza delle Sezioni di Classe 1-2-3-4"));
    addVarToContainer( m_gammaS[0]);

    m_gammaS[1] = new DoublePlus( 1.05, "gammaS1", m_unitMeasure, UnitMeasure::noDimension );
    m_gammaS[1]->setReadOnly( false );
    m_gammaS[1]->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">M1</span>") );
    m_gammaS[1]->setToolTip( trUtf8("Resistenza all'instabilità  delle membrature. Per ponti stradali e ferroviari assumere 1.10."));
    addVarToContainer( m_gammaS[1]);

    m_gammaS[2] = new DoublePlus( 1.25, "gammaS2", m_unitMeasure, UnitMeasure::noDimension );
    m_gammaS[2]->setReadOnly( false );
    m_gammaS[2]->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">M2</span>") );
    m_gammaS[2]->setToolTip( trUtf8("Resistenza, nei riguardi della frattura, delle sezioni tese (indebolite dai fori)"));
    addVarToContainer( m_gammaS[2]);

    m_fyd[0] = new DoublePlus( 0.0, "fyd0", m_unitMeasure, UnitMeasure::tension );
    m_fyd[0]->setReadOnly( true );
    m_fyd[0]->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">yd0</span>"));
    addVarToContainer( m_fyd[0]);
    m_fyd[0]->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Steel::setFyd_0) );
    m_fyd[0]->addConnectedVars(2, fyk, m_gammaS[0]);

    m_fyd[1] = new DoublePlus( 0.0, "fyd1", m_unitMeasure, UnitMeasure::tension );
    m_fyd[1]->setReadOnly( true );
    m_fyd[1]->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">yd1</span>"));
    addVarToContainer( m_fyd[1]);
    m_fyd[1]->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Steel::setFyd_1) );
    m_fyd[1]->addConnectedVars(2, fyk, m_gammaS[1]);

    m_fyd[2] = new DoublePlus( 0.0, "ftd2", m_unitMeasure, UnitMeasure::tension );
    m_fyd[2]->setReadOnly( true );
    m_fyd[2]->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">td2</span>"));
    addVarToContainer( m_fyd[2]);
    m_fyd[2]->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Steel::setFyd_2) );
    m_fyd[2]->addConnectedVars(2, fyk, m_gammaS[2]);

    m_fyd40[0] = new DoublePlus( 0.0, "fyd0_40", m_unitMeasure, UnitMeasure::tension );
    m_fyd40[0]->setReadOnly( true );
    m_fyd40[0]->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">yd0,40</span>"));
    addVarToContainer( m_fyd40[0]);
    m_fyd40[0]->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Steel::setFyd40_0) );
    m_fyd40[0]->addConnectedVars(2, fyk40, m_gammaS[0]);

    m_fyd40[1] = new DoublePlus( 0.0, "fyd1_40", m_unitMeasure, UnitMeasure::tension );
    m_fyd40[1]->setReadOnly( true );
    m_fyd40[1]->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">yd1,40</span>"));
    addVarToContainer( m_fyd40[1]);
    m_fyd40[1]->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Steel::setFyd40_1) );
    m_fyd40[1]->addConnectedVars(2, fyk40, m_gammaS[1]);

    m_fyd40[2] = new DoublePlus( 0.0, "ftd2_40", m_unitMeasure, UnitMeasure::tension );
    m_fyd40[2]->setReadOnly( true );
    m_fyd40[2]->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">td2,40</span>"));
    addVarToContainer( m_fyd40[2]);
    m_fyd40[2]->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Steel::setFyd40_2) );
    m_fyd40[2]->addConnectedVars(2, fyk40, m_gammaS[2]);
}

DoublePlus * Steel::gammaS( int ii ){
    int i = 0;
    if( ii > 0 && ii < 3 )
        i = ii;
    return m_gammaS[i];
}

DoublePlus * Steel::fyd( int ii ){
    int i = 0;
    if( ii > 0 && ii < 3 )
        i = ii;
    return m_fyd[i];
}

DoublePlus * Steel::fyd40( int ii ){
    int i = 0;
    if( ii > 0 && ii < 3 )
        i = ii;
    return m_fyd40[i];
}

double Steel::epsilon( double t ){
    if( fabs(t) > 0.04 ){
        if( fyk40->valueNormal() == 0.0 ){
            return 0.0;
        } else {
            return sqrt( 2.35e+8 / fyk40->valueNormal() );
        }
    } else {
        if( fyk->valueNormal() == 0.0 ){
            return 0.0;
        } else {
            return sqrt( 2.35e+8 / fyk->valueNormal() );
        }
    }
}

void Steel::setFyd( int i, bool emitAuto ){
    if( i > -1 && i < 3 ){
        if( i < 2 ){
            m_fyd[i]->setValueNormal( fyk->valueNormal() / m_gammaS[i]->valueNormal(), emitAuto );
        } else {
            m_fyd[i]->setValueNormal( ftk->valueNormal() / m_gammaS[i]->valueNormal(), emitAuto );
        }
    }
}

void Steel::setFyd40( int i, bool emitAuto ){
    if( i > -1 && i < 3 ){
        if( i < 2 ){
            m_fyd40[i]->setValueNormal( fyk40->valueNormal() / m_gammaS[i]->valueNormal(), emitAuto );
        } else {
            m_fyd40[i]->setValueNormal( ftk40->valueNormal() / m_gammaS[i]->valueNormal(), emitAuto );
        }
    }
}

void Steel::setFyd_0( bool emitAuto )   { setFyd(0, emitAuto); }
void Steel::setFyd40_0( bool emitAuto ) { setFyd40(0, emitAuto); }
void Steel::setFyd_1( bool emitAuto )   { setFyd(1, emitAuto); }
void Steel::setFyd40_1( bool emitAuto ) { setFyd40(1, emitAuto); }
void Steel::setFyd_2( bool emitAuto )   { setFyd(2, emitAuto); }
void Steel::setFyd40_2( bool emitAuto ) { setFyd40(2, emitAuto); }
