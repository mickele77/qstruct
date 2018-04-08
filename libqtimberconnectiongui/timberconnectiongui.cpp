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
#include "timberconnectiongui.h"
#include "ui_timberconnectiongui.h"

#include "varplusguiutility.h"
#include "timberconnection.h"
#include "timberconnector.h"
#include "timber.h"
#include "steel.h"
#include "materialmodel.h"

#include "serviceclass.h"
#include "loaddurationclass.h"
#include "lstype.h"
#include "doubleplus.h"
#include "qstringplus.h"
#include "uintplus.h"

#include <QString>
#include <QMap>

class TimberConnectionGUIPrivate{
public:
    TimberConnectionGUIPrivate( MaterialModel * matModel ):
        ui(new Ui::TimberConnectionGUI),
        materialModel( matModel ),
        connection( NULL ){
    }
    Ui::TimberConnectionGUI * ui;

    MaterialModel * materialModel;
    TimberConnection * connection;
    DoublePlus * FAxEd;
    DoublePlus * FVEd;
    DoublePlus * result;
};

TimberConnectionGUI::TimberConnectionGUI( UnitMeasure * ump, MaterialModel * matModel, QWidget *parent) :
    QWidget(parent),
    VarPlusContainer( ump, "TimberConnectionGUI", ""),
    m_d( new TimberConnectionGUIPrivate(matModel) ) {

    m_d->ui->setupUi(this);

    initVar();
    connect( m_d->ui->verifyPButton, SIGNAL(clicked()), this, SLOT(verify()));
}

TimberConnectionGUI::~TimberConnectionGUI() {
    delete m_d;
}

void TimberConnectionGUI::setConnection( TimberConnection * con ){
    if( m_d->connection ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectEnumVar( m_d->ui->serviceClassLabel, m_d->ui->serviceClassComboBox );
        VarPlusGUI::disconnectEnumVar( m_d->ui->durationClassLabel, m_d->ui->durationClassComboBox );
        VarPlusGUI::disconnectEnumVar( m_d->ui->LSTypeLabel, m_d->ui->LSTypeComboBox );
        VarPlusGUI::disconnectEnumVar( m_d->ui->connectorTypeLabel, m_d->ui->connectorTypeComboBox );
        VarPlusGUI::disconnectVar( m_d->ui->connectorAlphaLabel, m_d->ui->connectorAlphaLEdit, m_d->ui->connectorAlphaUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->connectorFukLabel, m_d->ui->connectorFukLEdit, m_d->ui->connectorFukUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->connectorFykLabel, m_d->ui->connectorFykLEdit, m_d->ui->connectorFykUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->connectorDLabel, m_d->ui->connectorDLEdit, m_d->ui->connectorDUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->connectorDHeadLabel, m_d->ui->connectorDHeadLEdit, m_d->ui->connectorDHeadUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->connectorDEfLabel, m_d->ui->connectorDEfLEdit, m_d->ui->connectorDEfUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->connectorLEfLabel, m_d->ui->connectorLEfLEdit, m_d->ui->connectorLEfUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->connectorMyRkLabel, m_d->ui->connectorMyRkLEdit, m_d->ui->connectorMyRkUMLabel, m_d->ui->connectorMyRkCBox );
        VarPlusGUI::disconnectVar( m_d->ui->nParLabel, m_d->ui->nParLEdit, m_d->ui->nParUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->nPerpLabel, m_d->ui->nPerpLEdit, m_d->ui->nPerpUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->a1Label, m_d->ui->a1LEdit, m_d->ui->a1UMLabel );
        VarPlusGUI::disconnectEnumVar( m_d->ui->connectionTypeLabel, m_d->ui->connectionTypeComboBox );
        VarPlusGUI::disconnectVar( m_d->ui->tTimber1Label, m_d->ui->tTimber1LEdit, m_d->ui->tTimber1UMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->tTimber2Label, m_d->ui->tTimber2LEdit, m_d->ui->tTimber2UMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->tSteelLabel, m_d->ui->tSteelLEdit, m_d->ui->tSteelUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->fAxk1ALabel, m_d->ui->fAxk1ALEdit, m_d->ui->fAxk1AUMLabel, m_d->ui->fAxk1ACheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fAxk1BLabel, m_d->ui->fAxk1BLEdit, m_d->ui->fAxk1BUMLabel, m_d->ui->fAxk1BCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fAxk2ALabel, m_d->ui->fAxk2ALEdit, m_d->ui->fAxk2AUMLabel, m_d->ui->fAxk2ACheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fAxk2BLabel, m_d->ui->fAxk2BLEdit, m_d->ui->fAxk2BUMLabel, m_d->ui->fAxk2BCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FAxRkLabel, m_d->ui->FAxRkLEdit, m_d->ui->FAxRkUMLabel, m_d->ui->FAxRkCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FAxRdLabel, m_d->ui->FAxRdLEdit, m_d->ui->FAxRdUMLabel, m_d->ui->FAxRdCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->nAxEfLabel, m_d->ui->nAxEfLEdit, m_d->ui->nAxEfUMLabel, m_d->ui->nAxEfCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FAxRdTotLabel, m_d->ui->FAxRdTotLEdit, m_d->ui->FAxRdTotUMLabel, m_d->ui->FAxRdTotCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fh0k1Label, m_d->ui->fh0k1LEdit, m_d->ui->fh0k1UMLabel, m_d->ui->fh0k1CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fhAlphak1Label, m_d->ui->fhAlphak1LEdit, m_d->ui->fhAlphak1UMLabel, m_d->ui->fhAlphak1CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fh0k2Label, m_d->ui->fh0k2LEdit, m_d->ui->fh0k2UMLabel, m_d->ui->fh0k2CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fhAlphak2Label, m_d->ui->fhAlphak2LEdit, m_d->ui->fhAlphak2UMLabel, m_d->ui->fhAlphak2CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->betaLabel, m_d->ui->betaLEdit, m_d->ui->betaUMLabel, m_d->ui->betaCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->gammaMLabel, m_d->ui->gammaMLEdit, m_d->ui->gammaMUMLabel, m_d->ui->gammaMCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->kmodLabel, m_d->ui->kmodLEdit, m_d->ui->kmodUMLabel, m_d->ui->kmodCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FVRk1ALabel, m_d->ui->FVRk1ALEdit, m_d->ui->FVRk1AUMLabel, m_d->ui->FVRk1ACheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FVRk1BLabel, m_d->ui->FVRk1BLEdit, m_d->ui->FVRk1BUMLabel, m_d->ui->FVRk1BCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FVRk1CLabel, m_d->ui->FVRk1CLEdit, m_d->ui->FVRk1CUMLabel, m_d->ui->FVRk1CCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FVRk2ALabel, m_d->ui->FVRk2ALEdit, m_d->ui->FVRk2AUMLabel, m_d->ui->FVRk2ACheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FVRk2BLabel, m_d->ui->FVRk2BLEdit, m_d->ui->FVRk2BUMLabel, m_d->ui->FVRk2BCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FVRk3Label, m_d->ui->FVRk3LEdit, m_d->ui->FVRk3UMLabel, m_d->ui->FVRk3CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FVRkLabel, m_d->ui->FVRkLEdit, m_d->ui->FVRkUMLabel, m_d->ui->FVRkCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FVRdLabel, m_d->ui->FVRdLEdit, m_d->ui->FVRdUMLabel, m_d->ui->FVRdCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->nVEfLabel, m_d->ui->nVEfLEdit, m_d->ui->nVEfUMLabel, m_d->ui->nVEfCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FVRdTotLabel, m_d->ui->FVRdTotLEdit, m_d->ui->FVRdTotUMLabel, m_d->ui->FVRdTotCheckBox );

        disconnect( m_d->connection->connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setVisibleGBox()));
        disconnect( m_d->ui->timber1CBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTimber1(int)));
        disconnect( m_d->ui->timber2CBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTimber2(int)));
        disconnect( m_d->ui->steelCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setSteel(int)));
    }

    m_d->connection = con;

    if( m_d->connection ){
        VarPlusGUI::connectVar( m_d->connection->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectEnumVar( m_d->connection->sClass(), m_d->ui->serviceClassLabel, m_d->ui->serviceClassComboBox );
        VarPlusGUI::connectEnumVar( m_d->connection->ldClass(), m_d->ui->durationClassLabel, m_d->ui->durationClassComboBox );
        VarPlusGUI::connectEnumVar( m_d->connection->lsType(), m_d->ui->LSTypeLabel, m_d->ui->LSTypeComboBox );
        VarPlusGUI::connectEnumVar( m_d->connection->connector()->connectorType, m_d->ui->connectorTypeLabel, m_d->ui->connectorTypeComboBox );
        VarPlusGUI::connectVar( m_d->connection->connector()->alpha, m_d->ui->connectorAlphaLabel, m_d->ui->connectorAlphaLEdit, m_d->ui->connectorAlphaUMLabel );
        VarPlusGUI::connectVar( m_d->connection->connector()->fyk, m_d->ui->connectorFukLabel, m_d->ui->connectorFukLEdit, m_d->ui->connectorFukUMLabel );
        VarPlusGUI::connectVar( m_d->connection->connector()->fuk, m_d->ui->connectorFykLabel, m_d->ui->connectorFykLEdit, m_d->ui->connectorFykUMLabel );
        VarPlusGUI::connectVar( m_d->connection->connector()->d, m_d->ui->connectorDLabel, m_d->ui->connectorDLEdit, m_d->ui->connectorDUMLabel );
        VarPlusGUI::connectVar( m_d->connection->connector()->dHead, m_d->ui->connectorDHeadLabel, m_d->ui->connectorDHeadLEdit, m_d->ui->connectorDHeadUMLabel );
        VarPlusGUI::connectVar( m_d->connection->connector()->dEf, m_d->ui->connectorDEfLabel, m_d->ui->connectorDEfLEdit, m_d->ui->connectorDEfUMLabel );
        VarPlusGUI::connectVar( m_d->connection->connector()->lEf, m_d->ui->connectorLEfLabel, m_d->ui->connectorLEfLEdit, m_d->ui->connectorLEfUMLabel );
        VarPlusGUI::connectVar( m_d->connection->connector()->MyRk, m_d->ui->connectorMyRkLabel, m_d->ui->connectorMyRkLEdit, m_d->ui->connectorMyRkUMLabel, m_d->ui->connectorMyRkCBox );
        VarPlusGUI::connectVar( m_d->connection->nPar, m_d->ui->nParLabel, m_d->ui->nParLEdit, m_d->ui->nParUMLabel );
        VarPlusGUI::connectVar( m_d->connection->nPerp, m_d->ui->nPerpLabel, m_d->ui->nPerpLEdit, m_d->ui->nPerpUMLabel );
        VarPlusGUI::connectVar( m_d->connection->a1, m_d->ui->a1Label, m_d->ui->a1LEdit, m_d->ui->a1UMLabel );
        VarPlusGUI::connectEnumVar( m_d->connection->connectionType, m_d->ui->connectionTypeLabel, m_d->ui->connectionTypeComboBox );
        VarPlusGUI::connectVar( m_d->connection->tTimber1, m_d->ui->tTimber1Label, m_d->ui->tTimber1LEdit, m_d->ui->tTimber1UMLabel );
        VarPlusGUI::connectVar( m_d->connection->tTimber2, m_d->ui->tTimber2Label, m_d->ui->tTimber2LEdit, m_d->ui->tTimber2UMLabel );
        VarPlusGUI::connectVar( m_d->connection->tSteel, m_d->ui->tSteelLabel, m_d->ui->tSteelLEdit, m_d->ui->tSteelUMLabel );
        VarPlusGUI::connectVar( m_d->connection->fAxk1A, m_d->ui->fAxk1ALabel, m_d->ui->fAxk1ALEdit, m_d->ui->fAxk1AUMLabel, m_d->ui->fAxk1ACheckBox );
        VarPlusGUI::connectVar( m_d->connection->fAxk1B, m_d->ui->fAxk1BLabel, m_d->ui->fAxk1BLEdit, m_d->ui->fAxk1BUMLabel, m_d->ui->fAxk1BCheckBox );
        VarPlusGUI::connectVar( m_d->connection->fAxk2A, m_d->ui->fAxk2ALabel, m_d->ui->fAxk2ALEdit, m_d->ui->fAxk2AUMLabel, m_d->ui->fAxk2ACheckBox );
        VarPlusGUI::connectVar( m_d->connection->fAxk2B, m_d->ui->fAxk2BLabel, m_d->ui->fAxk2BLEdit, m_d->ui->fAxk2BUMLabel, m_d->ui->fAxk2BCheckBox );
        VarPlusGUI::connectVar( m_d->connection->FAxRk, m_d->ui->FAxRkLabel, m_d->ui->FAxRkLEdit, m_d->ui->FAxRkUMLabel, m_d->ui->FAxRkCheckBox );
        VarPlusGUI::connectVar( m_d->connection->FAxRd, m_d->ui->FAxRdLabel, m_d->ui->FAxRdLEdit, m_d->ui->FAxRdUMLabel, m_d->ui->FAxRdCheckBox );
        VarPlusGUI::connectVar( m_d->connection->nAxEf, m_d->ui->nAxEfLabel, m_d->ui->nAxEfLEdit, m_d->ui->nAxEfUMLabel, m_d->ui->nAxEfCheckBox );
        VarPlusGUI::connectVar( m_d->connection->FAxRdTot, m_d->ui->FAxRdTotLabel, m_d->ui->FAxRdTotLEdit, m_d->ui->FAxRdTotUMLabel, m_d->ui->FAxRdTotCheckBox );
        VarPlusGUI::connectVar( m_d->connection->fh0k1, m_d->ui->fh0k1Label, m_d->ui->fh0k1LEdit, m_d->ui->fh0k1UMLabel, m_d->ui->fh0k1CheckBox );
        VarPlusGUI::connectVar( m_d->connection->fhAlphak1, m_d->ui->fhAlphak1Label, m_d->ui->fhAlphak1LEdit, m_d->ui->fhAlphak1UMLabel, m_d->ui->fhAlphak1CheckBox );
        VarPlusGUI::connectVar( m_d->connection->fh0k2, m_d->ui->fh0k2Label, m_d->ui->fh0k2LEdit, m_d->ui->fh0k2UMLabel, m_d->ui->fh0k2CheckBox );
        VarPlusGUI::connectVar( m_d->connection->fhAlphak2, m_d->ui->fhAlphak2Label, m_d->ui->fhAlphak2LEdit, m_d->ui->fhAlphak2UMLabel, m_d->ui->fhAlphak2CheckBox );
        VarPlusGUI::connectVar( m_d->connection->beta, m_d->ui->betaLabel, m_d->ui->betaLEdit, m_d->ui->betaUMLabel, m_d->ui->betaCheckBox );
        VarPlusGUI::connectVar( m_d->connection->gammaM, m_d->ui->gammaMLabel, m_d->ui->gammaMLEdit, m_d->ui->gammaMUMLabel, m_d->ui->gammaMCheckBox );
        VarPlusGUI::connectVar( m_d->connection->kmod, m_d->ui->kmodLabel, m_d->ui->kmodLEdit, m_d->ui->kmodUMLabel, m_d->ui->kmodCheckBox );
        VarPlusGUI::connectVar( m_d->connection->FVRk1A, m_d->ui->FVRk1ALabel, m_d->ui->FVRk1ALEdit, m_d->ui->FVRk1AUMLabel, m_d->ui->FVRk1ACheckBox );
        VarPlusGUI::connectVar( m_d->connection->FVRk1B, m_d->ui->FVRk1BLabel, m_d->ui->FVRk1BLEdit, m_d->ui->FVRk1BUMLabel, m_d->ui->FVRk1BCheckBox );
        VarPlusGUI::connectVar( m_d->connection->FVRk1C, m_d->ui->FVRk1CLabel, m_d->ui->FVRk1CLEdit, m_d->ui->FVRk1CUMLabel, m_d->ui->FVRk1CCheckBox );
        VarPlusGUI::connectVar( m_d->connection->FVRk2A, m_d->ui->FVRk2ALabel, m_d->ui->FVRk2ALEdit, m_d->ui->FVRk2AUMLabel, m_d->ui->FVRk2ACheckBox );
        VarPlusGUI::connectVar( m_d->connection->FVRk2B, m_d->ui->FVRk2BLabel, m_d->ui->FVRk2BLEdit, m_d->ui->FVRk2BUMLabel, m_d->ui->FVRk2BCheckBox );
        VarPlusGUI::connectVar( m_d->connection->FVRk3, m_d->ui->FVRk3Label, m_d->ui->FVRk3LEdit, m_d->ui->FVRk3UMLabel, m_d->ui->FVRk3CheckBox );
        VarPlusGUI::connectVar( m_d->connection->FVRk, m_d->ui->FVRkLabel, m_d->ui->FVRkLEdit, m_d->ui->FVRkUMLabel, m_d->ui->FVRkCheckBox );
        VarPlusGUI::connectVar( m_d->connection->FVRd, m_d->ui->FVRdLabel, m_d->ui->FVRdLEdit, m_d->ui->FVRdUMLabel, m_d->ui->FVRdCheckBox );
        VarPlusGUI::connectVar( m_d->connection->nVEf, m_d->ui->nVEfLabel, m_d->ui->nVEfLEdit, m_d->ui->nVEfUMLabel, m_d->ui->nVEfCheckBox );
        VarPlusGUI::connectVar( m_d->connection->FVRdTot, m_d->ui->FVRdTotLabel, m_d->ui->FVRdTotLEdit, m_d->ui->FVRdTotUMLabel, m_d->ui->FVRdTotCheckBox );

        connect( m_d->connection->connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setVisibleGBox()));
        setVisibleGBox();
        connect( m_d->ui->timber1CBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTimber1(int)));
        connect( m_d->ui->timber2CBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTimber2(int)));
        connect( m_d->ui->steelCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setSteel(int)));
    }
}

void TimberConnectionGUI::showEvent ( QShowEvent * event ){
    Q_UNUSED(event);
    if( m_d->connection ){
        if( m_d->connection->timber1() == NULL ){
            m_d->connection->setTimber1( dynamic_cast<Timber *>(m_d->materialModel->firstMaterial(MaterialModel::TimberMaterial)) );
        }
        if( m_d->connection->timber2() == NULL ){
            m_d->connection->setTimber2( dynamic_cast<Timber *>(m_d->materialModel->firstMaterial(MaterialModel::TimberMaterial) ));
        }
        if( m_d->connection->steel() == NULL ){
            m_d->connection->setSteel( dynamic_cast<Steel *>(m_d->materialModel->firstMaterial(MaterialModel::SteelMaterial) ));
        }
    }

    QList< Material * > matList = m_d->materialModel->materialsList( MaterialModel::TimberMaterial );
    m_d->ui->timber1CBox->clear();
    m_d->ui->timber2CBox->clear();
    for( QList< Material * >::iterator i=matList.begin(); i!=matList.end(); ++i ){
        m_d->ui->timber1CBox->addItem( (*i)->name->valueStr(), QVariant((*i)->id->valueNormal()) );
        m_d->ui->timber2CBox->addItem( (*i)->name->valueStr(), QVariant((*i)->id->valueNormal()) );
    }
    if( m_d->connection ){
        if( m_d->connection->timber1() != NULL ){
            m_d->ui->timber1CBox->setCurrentIndex( m_d->ui->timber1CBox->findData( QVariant( m_d->connection->timber1()->id->valueNormal()) ) );
        }
        if( m_d->connection->timber2() != NULL ){
            m_d->ui->timber2CBox->setCurrentIndex( m_d->ui->timber2CBox->findData( QVariant( m_d->connection->timber2()->id->valueNormal()) ) );
        }
    }
    connect( m_d->ui->timber1CBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTimber1(int)));
    connect( m_d->ui->timber2CBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTimber2(int)));

    matList = m_d->materialModel->materialsList( MaterialModel::SteelMaterial );
    m_d->ui->steelCBox->clear();
    for( QList< Material * >::iterator i=matList.begin(); i!=matList.end(); ++i ){
        m_d->ui->steelCBox->addItem( (*i)->name->valueStr(), QVariant((*i)->id->valueNormal()) );
    }
    if( m_d->connection ){
        if( m_d->connection->steel() != NULL ){
            m_d->ui->steelCBox->setCurrentIndex( m_d->ui->steelCBox->findData( QVariant( m_d->connection->steel()->id->valueNormal() ) ) );
        }
    }
    connect( m_d->ui->steelCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setSteel(int)));
}

void TimberConnectionGUI::hideEvent ( QHideEvent * ){
    disconnect( m_d->ui->timber1CBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTimber1(int)));
    disconnect( m_d->ui->timber2CBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTimber2(int)));
    disconnect( m_d->ui->steelCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setSteel(int)));
}

void TimberConnectionGUI::initVar() {
    m_d->FAxEd = new DoublePlus( 0.0, "FAxEd", m_unitMeasure, UnitMeasure::loadF );
    m_d->FAxEd->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">Ax,Ed</span>") );
    m_d->FAxEd->setToolTip( trUtf8("Carico assiale agente"));
    m_d->FAxEd->setReadOnly( false );
    addVarToContainer( m_d->FAxEd );
    VarPlusGUI::connectVar( m_d->FAxEd, m_d->ui->FAxEdLabel, m_d->ui->FAxEdLEdit, m_d->ui->FAxEdUMLabel );

    m_d->FVEd = new DoublePlus( 0.0, "FVEd", m_unitMeasure, UnitMeasure::loadF );
    m_d->FVEd->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Ed</span>") );
    m_d->FVEd->setToolTip( trUtf8("Carico trasversale agente"));
    m_d->FVEd->setReadOnly( false );
    addVarToContainer( m_d->FVEd );
    VarPlusGUI::connectVar(m_d->FVEd, m_d->ui->FVEdLabel, m_d->ui->FVEdLEdit, m_d->ui->FVEdUMLabel );

    m_d->result = new DoublePlus( 0.0, "result", m_unitMeasure, UnitMeasure::noDimension );
    m_d->result->setRichName( trUtf8("Risultato") );
    m_d->result->setToolTip( trUtf8("Livello di sollecitazione della connessione (per essere verificato deve essere minore di 1)"));
    m_d->result->setReadOnly( false );
    addVarToContainer( m_d->result );
    VarPlusGUI::connectVar( m_d->result, m_d->ui->resultLabel, m_d->ui->resultLEdit );
}

void TimberConnectionGUI::setTimber1( int currIndex ){
    Timber * timber = dynamic_cast<Timber *> (m_d->materialModel->materialId( m_d->ui->timber1CBox->itemData( currIndex ).toUInt()));
    if( timber != NULL ){
        m_d->connection->setTimber1( timber );
    }
}

void TimberConnectionGUI::setTimber2( int currIndex ){
    Timber * timber = dynamic_cast<Timber *> (m_d->materialModel->materialId( m_d->ui->timber2CBox->itemData( currIndex ).toUInt()));
    if( timber != NULL ){
        m_d->connection->setTimber2( timber );
    }
}

void TimberConnectionGUI::setSteel( int currIndex ){
    Steel * steel = dynamic_cast<Steel *> (m_d->materialModel->materialId( m_d->ui->steelCBox->itemData( currIndex ).toUInt()));
    if( steel != NULL ){
        m_d->connection->setSteel( steel );
    }
}

void TimberConnectionGUI::setVisibleGBox() {
    if( m_d->connection->connectionType->valueNormal() == TimberConnection::TTDoubleShear ||
            m_d->connection->connectionType->valueNormal() == TimberConnection::TTSingleShear ){
        m_d->ui->timber2GBox->setVisible( true );
        m_d->ui->steelGBox->setVisible( false );
    } else if( m_d->connection->connectionType->valueNormal() == TimberConnection::TSDoubleShearCentralSteel ||
               m_d->connection->connectionType->valueNormal() == TimberConnection::TSDoubleShearOuterSteel ||
               m_d->connection->connectionType->valueNormal() == TimberConnection::TSSingleShear ){
        m_d->ui->timber2GBox->setVisible( false );
        m_d->ui->steelGBox->setVisible( true );
    }
}

void TimberConnectionGUI::verify() {
    if( m_d->connection ){
        QString mes;

        if( m_d->connection->verify( m_d->FAxEd,
                                  m_d->FVEd,
                                  m_d->result,
                                  &mes) ) {
            m_d->ui->resultLEdit->setStyleSheet( "* {background-color: rgb(0, 255, 0)}" );
        } else {
            m_d->ui->resultLEdit->setStyleSheet( "* {background-color: rgb(255, 0, 0)}" );
        }
        m_d->ui->messagesTBrowser->clear();
        m_d->ui->messagesTBrowser->append( mes );
    }
}
