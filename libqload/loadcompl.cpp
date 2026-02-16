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

#include "loadcompl.h"

#include "loaddurationclass.h"
#include "qstringplus.h"
#include "boolplus.h"
#include "doubleplus.h"

LoadCompl::LoadCompl( UnitMeasure * ump,
                      const QString & nn,
                      double gg,
                      LoadType::Type lType,
                      LoadDurationClass::Class ldc,
                      QObject * parent ):
    QObject( parent ),
    VarPlusContainer( ump, "LoadCompl", nn) {
    initVar();

    gamma->setValueNormal(gg);
    loadType->setValueNormal( lType );
    durationClass->setValueNormal( ldc );
}

LoadCompl::LoadCompl( UnitMeasure * ump, const QXmlStreamAttributes &attrs, QObject *parent):
    QObject( parent ),
    VarPlusContainer( ump, "LoadCompl", ""){
    initVar();
    loadFromXML( attrs );
}

void LoadCompl::initVar(){
    name->setRichName( tr("Denominazione"));

    gamma = new DoublePlus(1.5, "gamma", m_unitMeasure, UnitMeasure::noDimension );
    gamma->setRichName( tr("γ"));
    addVarToContainer(gamma);

    variable = new BoolPlus(false, "variable"  );
    addVarToContainer( variable );

    permute = new BoolPlus(true, "permute"  );
    permute->setRichName( tr("permutazione"));
    addVarToContainer( permute );

    loadType = new LoadType(LoadType::loadPoint, "loadType"  );
    addVarToContainer( loadType );
    connect( loadType, &LoadType::valueChanged, this, &LoadCompl::loadChanged );

    durationClass = new LoadDurationClass( LoadDurationClass::shortTerm, "durationClass" );
    addVarToContainer( durationClass );
    connect( durationClass, &LoadDurationClass::valueChanged, this, &LoadCompl::loadChanged );

    for(int i=0; i<3; i++){
        m_psi[i] = new DoublePlus(1.0, "psi"+QString::number(i), m_unitMeasure, UnitMeasure::noDimension, true );
        m_psi[i]->setRichName( tr("ψ") + QString("<span style=\" vertical-align:sub;\">") + QString::number(i) + QString("</span>"));
        addVarToContainer( m_psi[i] );
    }
    connect( variable, &BoolPlus::valueChangedBool, this, &LoadCompl::updatePsi );

    for( int i=0; i < LOADCOMPL_NP; i++){
        m_P[i] = new DoublePlus( 0.0, "P" + QString::number(i), m_unitMeasure, UnitMeasure::noDimension  );
        addVarToContainer( m_P[i] );
        connect( m_P[i], &DoublePlus::valueChanged, this, &LoadCompl::loadChanged );
    }
    connect( loadType, &LoadType::valueChanged, this, &LoadCompl::updateP );
    updateP();
}

int LoadCompl::nPMax() {
    return LOADCOMPL_NP;
}

int LoadCompl::nP(){
    return m_nP;
}

DoublePlus * LoadCompl::P( int i ){
    if( i >= 0 && i < LOADCOMPL_NP ){
        return m_P[i];
    } else {
        return NULL;
    }
}

void LoadCompl::updateP(){
    switch( loadType->valueNormal() ){
    case LoadType::loadPoint :{
        m_nP = 4;
        m_P[0]->setEnabled( true );
        m_P[0]->setUnitMeasure( UnitMeasure::loadF );
        m_P[0]->setName(tr("Py"));
        m_P[0]->setRichName( tr("P<span style=\" vertical-align:sub;\">x</span>") );
        m_P[0]->setToolTip( tr("Componente orizzontale del carico concetrato (coordinate globali)"));
        m_P[1]->setEnabled( true );
        m_P[1]->setUnitMeasure( UnitMeasure::loadF );
        m_P[1]->setName(tr("Px"));
        m_P[1]->setRichName( tr("P<span style=\" vertical-align:sub;\">y</span>") );
        m_P[1]->setToolTip( tr("Componente verticale del carico concetrato (coordinate globali)"));
        m_P[2]->setEnabled( true );
        m_P[2]->setUnitMeasure( UnitMeasure::loadM );
        m_P[2]->setName(tr("Mz"));
        m_P[2]->setRichName( tr("M<span style=\" vertical-align:sub;\">z</span>") );
        m_P[2]->setToolTip( tr("Coppia concentrata (coordinate globali)"));
        m_P[3]->setEnabled( true );
        m_P[3]->setUnitMeasure( UnitMeasure::length );
        m_P[3]->setName(tr("xlP"));
        m_P[3]->setRichName( tr("x<span style=\" vertical-align:sub;\">l,P</span>") );
        m_P[3]->setToolTip( tr("Ascissa curvilinea del punto in cui il carico è applicato (coordinate locali)"));
        for( int i=4; i < LOADCOMPL_NP; i++ ){
            m_P[i]->setEnabled( false );
            m_P[i]->setUnitMeasure( UnitMeasure::noDimension );
            m_P[i]->setName( "" );
            m_P[i]->setRichName( "" );
            m_P[i]->setToolTip( ""  );
        }
        break; }
    case LoadType::loadArea :{
        m_nP = 8;
        m_P[0]->setEnabled( true );
        m_P[0]->setUnitMeasure( UnitMeasure::loadFArea );
        m_P[0]->setName(tr("qxz1"));
        m_P[0]->setRichName( tr("q<span style=\" vertical-align:sub;\">xz,1</span>") );
        m_P[0]->setToolTip( tr("Componente orizzontale iniziale del carico (in coordinate globali)") );
        m_P[1]->setEnabled( true );
        m_P[1]->setUnitMeasure( UnitMeasure::loadFArea );
        m_P[1]->setName(tr("qyz1"));
        m_P[1]->setRichName( tr("q<span style=\" vertical-align:sub;\">yz,1</span>") );
        m_P[1]->setToolTip( tr("Componente verticale iniziale del carico distribuito (in coordinate globali)") );
        m_P[2]->setEnabled( true );
        m_P[2]->setUnitMeasure( UnitMeasure::length );
        m_P[2]->setName(tr("zq1"));
        m_P[2]->setRichName( tr("z<span style=\" vertical-align:sub;\">q,1</span>") );
        m_P[2]->setToolTip( tr("Lunghezza di influenza del carico in corrispondenza dell'ascissa iniziale di applicazione del carico (in coordinate globali)") );
        m_P[3]->setEnabled( true );
        m_P[3]->setUnitMeasure( UnitMeasure::length );
        m_P[3]->setName(tr("xlq1"));
        m_P[3]->setRichName( tr("x<span style=\" vertical-align:sub;\">l,q,1</span>") );
        m_P[3]->setToolTip( tr("Ascissa iniziale del tratto in cui il carico è applicato (in coordinate locali)") );

        m_P[4]->setEnabled( true );
        m_P[4]->setUnitMeasure( UnitMeasure::loadFArea );
        m_P[4]->setName(tr("qxz2"));
        m_P[4]->setRichName( tr("q<span style=\" vertical-align:sub;\">xz,2</span>") );
        m_P[4]->setToolTip( tr("Componente orizzontale finale del carico (in coordinate globali)") );
        m_P[5]->setEnabled( true );
        m_P[5]->setUnitMeasure( UnitMeasure::loadFArea );
        m_P[5]->setName(tr("qyz2"));
        m_P[5]->setRichName( tr("q<span style=\" vertical-align:sub;\">yz,2</span>") );
        m_P[5]->setToolTip( tr("Componente verticale finale del carico distribuito (in coordinate globali)") );
        m_P[6]->setEnabled( true );
        m_P[6]->setUnitMeasure( UnitMeasure::length );
        m_P[6]->setName(tr("zq2"));
        m_P[6]->setRichName( tr("z<span style=\" vertical-align:sub;\">q,2</span>") );
        m_P[6]->setToolTip( tr("Lunghezza di influenza del carico in corrispondenza dell'ascissa finale di applicazione del carico (in coordinate globali)") );
        m_P[7]->setEnabled( true );
        m_P[7]->setUnitMeasure( UnitMeasure::length );
        m_P[7]->setName(tr("xlq2"));
        m_P[7]->setRichName( tr("x<span style=\" vertical-align:sub;\">l,q,2</span>") );
        m_P[7]->setToolTip( tr("Ascissa finale del tratto in cui il carico è applicato (in coordinate locali)") );
        break; }
    case LoadType::loadVolume :{
        m_nP = 1;
        m_P[0]->setEnabled( true );
        m_P[0]->setUnitMeasure( UnitMeasure::loadFVolume );
        m_P[0]->setName(tr("gamma"));
        m_P[0]->setRichName(tr("γ<span style=\" vertical-align:sub;\">V</span>"));
        for( int i=1; i < LOADCOMPL_NP; i++ ){
            m_P[i]->setEnabled( false );
            m_P[i]->setUnitMeasure( UnitMeasure::noDimension );
            m_P[i]->setName( "" );
            m_P[i]->setRichName( "" );
            m_P[i]->setToolTip( ""  );
        }
        break; }
    default: { // comprende LoadType::loadLine
        m_nP = 6;

        m_P[0]->setEnabled( true );
        m_P[0]->setUnitMeasure( UnitMeasure::loadFLine );
        m_P[0]->setName(tr("qx1"));
        m_P[0]->setRichName( tr("q<span style=\" vertical-align:sub;\">x,1</span>") );
        m_P[0]->setToolTip( tr("Componente orizzontale iniziale del carico distribuito (in coordinate globali)") );
        m_P[1]->setEnabled( true );
        m_P[1]->setUnitMeasure( UnitMeasure::loadFLine );
        m_P[1]->setName(tr("qy1"));
        m_P[1]->setRichName( tr("q<span style=\" vertical-align:sub;\">y,1</span>") );
        m_P[1]->setToolTip( tr("Componente verticale iniziale del carico distribuito (in coordinate globali)") );
        m_P[2]->setEnabled( true );
        m_P[2]->setUnitMeasure( UnitMeasure::length );
        m_P[2]->setName(tr("xlq1"));
        m_P[2]->setRichName( tr("x<span style=\" vertical-align:sub;\">l,q,1</span>") );
        m_P[2]->setToolTip( tr("Ascissa iniziale del tratto in cui il carico è applicato (in coordinate locali)") );

        m_P[3]->setEnabled( true );
        m_P[3]->setUnitMeasure( UnitMeasure::loadFLine );
        m_P[3]->setName(tr("qx2"));
        m_P[3]->setRichName( tr("q<span style=\" vertical-align:sub;\">x,2</span>") );
        m_P[3]->setToolTip( tr("Componente orizzontale finale del carico distribuito (in coordinate globali)") );
        m_P[4]->setEnabled( true );
        m_P[4]->setUnitMeasure( UnitMeasure::loadFLine );
        m_P[4]->setName(tr("qy2"));
        m_P[4]->setRichName( tr("q<span style=\" vertical-align:sub;\">y,2</span>") );
        m_P[4]->setToolTip( tr("Componente verticale finale del carico distribuito (in coordinate globali)") );
        m_P[5]->setEnabled( true );
        m_P[5]->setUnitMeasure( UnitMeasure::length );
        m_P[5]->setName(tr("xlq2"));
        m_P[5]->setRichName( tr("x<span style=\" vertical-align:sub;\">l,q,2</span>") );
        m_P[5]->setToolTip( tr("Ascissa finale del tratto in cui il carico è applicato (in coordinate locali)") );

        for( int i=6; i < LOADCOMPL_NP; i++ ){
            m_P[i]->setEnabled( false );
            m_P[i]->setUnitMeasure( UnitMeasure::noDimension );
            m_P[i]->setName( "" );
            m_P[i]->setRichName( "" );
            m_P[i]->setToolTip( ""  );
        }

        break; }
    }
}

DoublePlus * LoadCompl::psi(int i){
    if( i >= 0 && i < 3 ){
        return m_psi[i];
    }
    return NULL;
}

void LoadCompl::updatePsi(){
    if( variable->valueNormal() ){
        for(int i=0; i < 3; i++ ){
            m_psi[i]->setReadOnly( false );
        }
    } else {
        for(int i=0; i < 3; i++ ){
            m_psi[i]->setReadOnly( true );
            m_psi[i]->setValueNormal( 1.0 );
        }
    }
}
