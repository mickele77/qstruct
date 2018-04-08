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
    name->setRichName( trUtf8("Denominazione"));

    gamma = new DoublePlus(1.5, "gamma", m_unitMeasure, UnitMeasure::noDimension );
    gamma->setRichName( trUtf8("γ"));
    addVarToContainer(gamma);

    variable = new BoolPlus(false, "variable"  );
    addVarToContainer( variable );

    permute = new BoolPlus(true, "permute"  );
    permute->setRichName( trUtf8("permutazione"));
    addVarToContainer( permute );

    loadType = new LoadType(LoadType::loadPoint, "loadType"  );
    addVarToContainer( loadType );
    connect( loadType, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()));

    durationClass = new LoadDurationClass( LoadDurationClass::shortTerm, "durationClass" );
    addVarToContainer( durationClass );
    connect( durationClass, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()));

    for(int i=0; i<3; i++){
        m_psi[i] = new DoublePlus(1.0, "psi"+QString::number(i), m_unitMeasure, UnitMeasure::noDimension, true );
        m_psi[i]->setRichName( trUtf8("ψ") + QString("<span style=\" vertical-align:sub;\">") + QString::number(i) + QString("</span>"));
        addVarToContainer( m_psi[i] );
    }
    connect( variable, SIGNAL(valueChangedBool(bool)), this, SLOT(updatePsi()) );

    for( int i=0; i < LOADCOMPL_NP; i++){
        m_P[i] = new DoublePlus( 0.0, "P" + QString::number(i), m_unitMeasure, UnitMeasure::noDimension  );
        addVarToContainer( m_P[i] );
        connect( m_P[i], SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()));
    }
    connect( loadType, SIGNAL(valueChanged(QString)), this, SLOT(updateP()));
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
        m_P[0]->setName(trUtf8("Py"));
        m_P[0]->setRichName( trUtf8("P<span style=\" vertical-align:sub;\">x</span>") );
        m_P[0]->setToolTip( trUtf8("Componente orizzontale del carico concetrato (coordinate globali)"));
        m_P[1]->setEnabled( true );
        m_P[1]->setUnitMeasure( UnitMeasure::loadF );
        m_P[1]->setName(trUtf8("Px"));
        m_P[1]->setRichName( trUtf8("P<span style=\" vertical-align:sub;\">y</span>") );
        m_P[1]->setToolTip( trUtf8("Componente verticale del carico concetrato (coordinate globali)"));
        m_P[2]->setEnabled( true );
        m_P[2]->setUnitMeasure( UnitMeasure::loadM );
        m_P[2]->setName(trUtf8("Mz"));
        m_P[2]->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">z</span>") );
        m_P[2]->setToolTip( trUtf8("Coppia concentrata (coordinate globali)"));
        m_P[3]->setEnabled( true );
        m_P[3]->setUnitMeasure( UnitMeasure::length );
        m_P[3]->setName(trUtf8("xlP"));
        m_P[3]->setRichName( trUtf8("x<span style=\" vertical-align:sub;\">l,P</span>") );
        m_P[3]->setToolTip( trUtf8("Ascissa curvilinea del punto in cui il carico è applicato (coordinate locali)"));
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
        m_P[0]->setName(trUtf8("qxz1"));
        m_P[0]->setRichName( trUtf8("q<span style=\" vertical-align:sub;\">xz,1</span>") );
        m_P[0]->setToolTip( trUtf8("Componente orizzontale iniziale del carico (in coordinate globali)") );
        m_P[1]->setEnabled( true );
        m_P[1]->setUnitMeasure( UnitMeasure::loadFArea );
        m_P[1]->setName(trUtf8("qyz1"));
        m_P[1]->setRichName( trUtf8("q<span style=\" vertical-align:sub;\">yz,1</span>") );
        m_P[1]->setToolTip( trUtf8("Componente verticale iniziale del carico distribuito (in coordinate globali)") );
        m_P[2]->setEnabled( true );
        m_P[2]->setUnitMeasure( UnitMeasure::length );
        m_P[2]->setName(trUtf8("zq1"));
        m_P[2]->setRichName( trUtf8("z<span style=\" vertical-align:sub;\">q,1</span>") );
        m_P[2]->setToolTip( trUtf8("Lunghezza di influenza del carico in corrispondenza dell'ascissa iniziale di applicazione del carico (in coordinate globali)") );
        m_P[3]->setEnabled( true );
        m_P[3]->setUnitMeasure( UnitMeasure::length );
        m_P[3]->setName(trUtf8("xlq1"));
        m_P[3]->setRichName( trUtf8("x<span style=\" vertical-align:sub;\">l,q,1</span>") );
        m_P[3]->setToolTip( trUtf8("Ascissa iniziale del tratto in cui il carico è applicato (in coordinate locali)") );

        m_P[4]->setEnabled( true );
        m_P[4]->setUnitMeasure( UnitMeasure::loadFArea );
        m_P[4]->setName(trUtf8("qxz2"));
        m_P[4]->setRichName( trUtf8("q<span style=\" vertical-align:sub;\">xz,2</span>") );
        m_P[4]->setToolTip( trUtf8("Componente orizzontale finale del carico (in coordinate globali)") );
        m_P[5]->setEnabled( true );
        m_P[5]->setUnitMeasure( UnitMeasure::loadFArea );
        m_P[5]->setName(trUtf8("qyz2"));
        m_P[5]->setRichName( trUtf8("q<span style=\" vertical-align:sub;\">yz,2</span>") );
        m_P[5]->setToolTip( trUtf8("Componente verticale finale del carico distribuito (in coordinate globali)") );
        m_P[6]->setEnabled( true );
        m_P[6]->setUnitMeasure( UnitMeasure::length );
        m_P[6]->setName(trUtf8("zq2"));
        m_P[6]->setRichName( trUtf8("z<span style=\" vertical-align:sub;\">q,2</span>") );
        m_P[6]->setToolTip( trUtf8("Lunghezza di influenza del carico in corrispondenza dell'ascissa finale di applicazione del carico (in coordinate globali)") );
        m_P[7]->setEnabled( true );
        m_P[7]->setUnitMeasure( UnitMeasure::length );
        m_P[7]->setName(trUtf8("xlq2"));
        m_P[7]->setRichName( trUtf8("x<span style=\" vertical-align:sub;\">l,q,2</span>") );
        m_P[7]->setToolTip( trUtf8("Ascissa finale del tratto in cui il carico è applicato (in coordinate locali)") );
        break; }
    case LoadType::loadVolume :{
        m_nP = 1;
        m_P[0]->setEnabled( true );
        m_P[0]->setUnitMeasure( UnitMeasure::loadFVolume );
        m_P[0]->setName(trUtf8("gamma"));
        m_P[0]->setRichName(trUtf8("γ<span style=\" vertical-align:sub;\">V</span>"));
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
        m_P[0]->setName(trUtf8("qx1"));
        m_P[0]->setRichName( trUtf8("q<span style=\" vertical-align:sub;\">x,1</span>") );
        m_P[0]->setToolTip( trUtf8("Componente orizzontale iniziale del carico distribuito (in coordinate globali)") );
        m_P[1]->setEnabled( true );
        m_P[1]->setUnitMeasure( UnitMeasure::loadFLine );
        m_P[1]->setName(trUtf8("qy1"));
        m_P[1]->setRichName( trUtf8("q<span style=\" vertical-align:sub;\">y,1</span>") );
        m_P[1]->setToolTip( trUtf8("Componente verticale iniziale del carico distribuito (in coordinate globali)") );
        m_P[2]->setEnabled( true );
        m_P[2]->setUnitMeasure( UnitMeasure::length );
        m_P[2]->setName(trUtf8("xlq1"));
        m_P[2]->setRichName( trUtf8("x<span style=\" vertical-align:sub;\">l,q,1</span>") );
        m_P[2]->setToolTip( trUtf8("Ascissa iniziale del tratto in cui il carico è applicato (in coordinate locali)") );

        m_P[3]->setEnabled( true );
        m_P[3]->setUnitMeasure( UnitMeasure::loadFLine );
        m_P[3]->setName(trUtf8("qx2"));
        m_P[3]->setRichName( trUtf8("q<span style=\" vertical-align:sub;\">x,2</span>") );
        m_P[3]->setToolTip( trUtf8("Componente orizzontale finale del carico distribuito (in coordinate globali)") );
        m_P[4]->setEnabled( true );
        m_P[4]->setUnitMeasure( UnitMeasure::loadFLine );
        m_P[4]->setName(trUtf8("qy2"));
        m_P[4]->setRichName( trUtf8("q<span style=\" vertical-align:sub;\">y,2</span>") );
        m_P[4]->setToolTip( trUtf8("Componente verticale finale del carico distribuito (in coordinate globali)") );
        m_P[5]->setEnabled( true );
        m_P[5]->setUnitMeasure( UnitMeasure::length );
        m_P[5]->setName(trUtf8("xlq2"));
        m_P[5]->setRichName( trUtf8("x<span style=\" vertical-align:sub;\">l,q,2</span>") );
        m_P[5]->setToolTip( trUtf8("Ascissa finale del tratto in cui il carico è applicato (in coordinate locali)") );

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
