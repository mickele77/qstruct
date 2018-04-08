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
#include "sectionthincontsteelgui.h"
#include "ui_sectionthincontsteelgui.h"

#include "sectiongeometricdatapanel.h"
#include "sectiontransformpanel.h"

#include "sectionthincontsteel.h"
#include "sectionthincontpointsmodel.h"
#include "multisectionlinemodel.h"
#include "multisectionarcmodel.h"
#include "sectionline.h"
#include "sectionarc.h"
#include "materialmodel.h"
#include "material.h"

#include "varplusguiutility.h"
#include "qstringplus.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "uintplus.h"

#include <QList>
#include <QGraphicsItemGroup>

class SectionThinContSteelGUIPrivate{
public:
    SectionThinContSteelGUIPrivate(){
        section = NULL;
        ui = new Ui::SectionThinContSteelGUI;
    }
    ~SectionThinContSteelGUIPrivate(){
        delete ui;
    }

    SectionThinContSteel * section;
    Ui::SectionThinContSteelGUI * ui;
    SectionGeometricDataPanel * geometricDataPanel;
    SectionTransformPanel * rotTransPanel;
};

SectionThinContSteelGUI::SectionThinContSteelGUI(UnitMeasure* ump, MaterialModel * matModel, QWidget *parent) :
    SectionGUI(ump, matModel, parent),
    m_d( new SectionThinContSteelGUIPrivate() ){

    m_d->ui->setupUi(this);

    m_d->geometricDataPanel = new SectionGeometricDataPanel( m_d->ui->geometricDataTab );
    m_d->ui->geometricDataScrollAreaContents->layout()->addWidget( m_d->geometricDataPanel );
    m_d->rotTransPanel = new SectionTransformPanel( m_unitMeasure, m_d->ui->rotTransTab );
    m_d->ui->rotTransTab->layout()->addWidget( m_d->rotTransPanel );

    m_d->ui->pointsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect( m_d->ui->addPointsPButton, SIGNAL(clicked()), this, SLOT(addPoints()));
    connect( m_d->ui->removePointsPButton, SIGNAL(clicked()), this, SLOT(removePoints()));

    m_materialComboBox = m_d->ui->materialComboBox;
    connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    refreshMaterialComboBox();

    m_d->ui->ULSMyDetailsGroupBox->setVisible( false );
    m_d->ui->ULSMzDetailsGroupBox->setVisible( false );
}

SectionThinContSteelGUI::~SectionThinContSteelGUI() {
    delete m_d;
}

void SectionThinContSteelGUI::setSection(Section * s){
    if( m_d->section ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->tLabel, m_d->ui->tLEdit, m_d->ui->tUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->isClosedLabel, m_d->ui->isClosedCheckBox );

        VarPlusGUI::disconnectVar( m_d->ui->ANMinusEffLabel, m_d->ui->ANMinusEffLEdit, m_d->ui->ANMinusEffUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->eGNMinusEffYLabel, m_d->ui->eGNMinusEffYLEdit, m_d->ui->eGNMinusEffYUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->eGNMinusEffZLabel, m_d->ui->eGNMinusEffZLEdit, m_d->ui->eGNMinusEffZUMLabel );

        VarPlusGUI::disconnectVar( m_d->ui->WEffYMinLabel, m_d->ui->WEffYMinLEdit, m_d->ui->WEffYMinUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->thetaCMyyMinEffLabel, m_d->ui->thetaCMyyMinEffLEdit, m_d->ui->thetaCMyyMinEffUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->WEffYPlusLabel, m_d->ui->WEffYPlusLEdit, m_d->ui->WEffYPlusUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->thetaCMyyPlusEffLabel, m_d->ui->thetaCMyyPlusEffLEdit, m_d->ui->thetaCMyyPlusEffUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->WEffYMinusLabel, m_d->ui->WEffYMinusLEdit, m_d->ui->WEffYMinusUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->thetaCMyyMinusEffLabel, m_d->ui->thetaCMyyMinusEffLEdit, m_d->ui->thetaCMyyMinusEffUMLabel );

        VarPlusGUI::disconnectVar( m_d->ui->WEffZMinLabel, m_d->ui->WEffZMinLEdit, m_d->ui->WEffZMinUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->thetaCMzzMinEffLabel, m_d->ui->thetaCMzzMinEffLEdit, m_d->ui->thetaCMzzMinEffUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->WEffZPlusLabel, m_d->ui->WEffZPlusLEdit, m_d->ui->WEffZPlusUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->thetaCMzzPlusEffLabel, m_d->ui->thetaCMzzPlusEffLEdit, m_d->ui->thetaCMzzPlusEffUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->WEffZMinusLabel, m_d->ui->WEffZMinusLEdit, m_d->ui->WEffZMinusUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->thetaCMzzMinusEffLabel, m_d->ui->thetaCMzzMinusEffLEdit, m_d->ui->thetaCMzzMinusEffUMLabel );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        m_materialComboBox->setCurrentIndex( -1 );
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }

    m_d->section = dynamic_cast<SectionThinContSteel *>(s);

    if( m_d->section ){
        VarPlusGUI::connectVar( m_d->section->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->section->pW, m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::connectVar( m_d->section->t, m_d->ui->tLabel, m_d->ui->tLEdit, m_d->ui->tUMLabel );
        VarPlusGUI::connectVar( m_d->section->isClosed, m_d->ui->isClosedLabel, m_d->ui->isClosedCheckBox );

        VarPlusGUI::connectVar( m_d->section->ANMinusEff, m_d->ui->ANMinusEffLabel, m_d->ui->ANMinusEffLEdit, m_d->ui->ANMinusEffUMLabel );
        VarPlusGUI::connectVar( m_d->section->eGNMinusEff->y, m_d->ui->eGNMinusEffYLabel, m_d->ui->eGNMinusEffYLEdit, m_d->ui->eGNMinusEffYUMLabel );
        VarPlusGUI::connectVar( m_d->section->eGNMinusEff->z, m_d->ui->eGNMinusEffZLabel, m_d->ui->eGNMinusEffZLEdit, m_d->ui->eGNMinusEffZUMLabel );

        VarPlusGUI::connectVar( m_d->section->WMyyMinEff, m_d->ui->WEffYMinLabel, m_d->ui->WEffYMinLEdit, m_d->ui->WEffYMinUMLabel );
        VarPlusGUI::connectVar( m_d->section->thetaCMyyMinEff, m_d->ui->thetaCMyyMinEffLabel, m_d->ui->thetaCMyyMinEffLEdit, m_d->ui->thetaCMyyMinEffUMLabel );
        VarPlusGUI::connectVar( m_d->section->WMyyPlusEff, m_d->ui->WEffYPlusLabel, m_d->ui->WEffYPlusLEdit, m_d->ui->WEffYPlusUMLabel );
        VarPlusGUI::connectVar( m_d->section->thetaCMyyPlusEff, m_d->ui->thetaCMyyPlusEffLabel, m_d->ui->thetaCMyyPlusEffLEdit, m_d->ui->thetaCMyyPlusEffUMLabel );
        VarPlusGUI::connectVar( m_d->section->WMyyMinusEff, m_d->ui->WEffYMinusLabel, m_d->ui->WEffYMinusLEdit, m_d->ui->WEffYMinusUMLabel );
        VarPlusGUI::connectVar( m_d->section->thetaCMyyMinusEff, m_d->ui->thetaCMyyMinusEffLabel, m_d->ui->thetaCMyyMinusEffLEdit, m_d->ui->thetaCMyyMinusEffUMLabel );

        VarPlusGUI::connectVar( m_d->section->WMzzMinEff, m_d->ui->WEffZMinLabel, m_d->ui->WEffZMinLEdit, m_d->ui->WEffZMinUMLabel );
        VarPlusGUI::connectVar( m_d->section->thetaCMzzMinEff, m_d->ui->thetaCMzzMinEffLabel, m_d->ui->thetaCMzzMinEffLEdit, m_d->ui->thetaCMzzMinEffUMLabel );
        VarPlusGUI::connectVar( m_d->section->WMzzPlusEff, m_d->ui->WEffZPlusLabel, m_d->ui->WEffZPlusLEdit, m_d->ui->WEffZPlusUMLabel );
        VarPlusGUI::connectVar( m_d->section->thetaCMzzPlusEff, m_d->ui->thetaCMzzPlusEffLabel, m_d->ui->thetaCMzzPlusEffLEdit, m_d->ui->thetaCMzzPlusEffUMLabel );
        VarPlusGUI::connectVar( m_d->section->WMzzMinusEff, m_d->ui->WEffZMinusLabel, m_d->ui->WEffZMinusLEdit, m_d->ui->WEffZMinusUMLabel );
        VarPlusGUI::connectVar( m_d->section->thetaCMzzMinusEff, m_d->ui->thetaCMzzMinusEffLabel, m_d->ui->thetaCMzzMinusEffLEdit, m_d->ui->thetaCMzzMinusEffUMLabel );

        m_d->ui->pointsTableView->setModel( m_d->section->pointsModel() );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        if( m_d->section->material() ){
            m_materialComboBox->setCurrentIndex( m_d->ui->materialComboBox->findData( QVariant(m_d->section->material()->id->valueNormal() ) ) );
        } else {
            m_materialComboBox->setCurrentIndex( -1 );
        }
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }

    m_d->geometricDataPanel->setSection( s );
    m_d->rotTransPanel->setSection( s );
}

void SectionThinContSteelGUI::addPoints(){
    QModelIndexList listRows = m_d->ui->pointsTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->pointsModel()->insertRows( listRows.last().row()+1, listRows.size() );
    } else {
        m_d->section->pointsModel()->insertRows( m_d->section->pointsModel()->rowCount(), 1 );
    }
}

void SectionThinContSteelGUI::removePoints(){
    QModelIndexList listRows = m_d->ui->pointsTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->pointsModel()->removeRows( listRows.first().row(), listRows.size());
    }
}

Material * SectionThinContSteelGUI::material(){
    if( m_d->section != NULL ){
        return m_d->section->material();
    }
    return NULL;
}

void SectionThinContSteelGUI::setMaterialFromComboBox(){
    if( m_d->section != NULL && m_materialModel != NULL && m_materialComboBox != NULL ){
        if( m_materialComboBox->currentIndex() == -1 ){
            m_d->section->setMaterial( NULL );
        } else {
            m_d->section->setMaterial( m_materialModel->materialId( m_materialComboBox->currentData().toUInt()) );
        }
    }
}
