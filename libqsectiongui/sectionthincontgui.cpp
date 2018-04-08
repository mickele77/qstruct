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
#include "sectionthincontgui.h"
#include "ui_sectionthincontgui.h"

#include "sectiongeometricdatapanel.h"
#include "sectiontransformpanel.h"

#include "multisectionlinemodel.h"
#include "multisectionarcmodel.h"
#include "sectionthincont.h"
#include "sectionthincontpointsmodel.h"
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

class SectionThinContGUIPrivate{
public:
    SectionThinContGUIPrivate(){
        section = NULL;
        ui = new Ui::SectionThinContGUI;
    }
    ~SectionThinContGUIPrivate(){
        delete ui;
    }

    SectionThinCont * section;
    Ui::SectionThinContGUI * ui;
    SectionGeometricDataPanel * geometricDataPanel;
    SectionTransformPanel * rotTransPanel;
};

SectionThinContGUI::SectionThinContGUI(UnitMeasure* ump, MaterialModel * matModel, QWidget *parent) :
    SectionGUI(ump, matModel, parent),
    m_d( new SectionThinContGUIPrivate() ){

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
}

SectionThinContGUI::~SectionThinContGUI() {
    delete m_d;
}

void SectionThinContGUI::setSection(Section * s){
    if( m_d->section ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->tLabel, m_d->ui->tLEdit, m_d->ui->tUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->isClosedLabel, m_d->ui->isClosedCheckBox );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        m_materialComboBox->setCurrentIndex( -1 );
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }

    m_d->section = dynamic_cast<SectionThinCont *>(s);

    if( m_d->section ){
        VarPlusGUI::connectVar( m_d->section->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->section->pW, m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::connectVar( m_d->section->t, m_d->ui->tLabel, m_d->ui->tLEdit, m_d->ui->tUMLabel );
        VarPlusGUI::connectVar( m_d->section->isClosed, m_d->ui->isClosedLabel, m_d->ui->isClosedCheckBox );

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

void SectionThinContGUI::addPoints(){
    QModelIndexList listRows = m_d->ui->pointsTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->pointsModel()->insertRows( listRows.last().row()+1, listRows.size() );
    } else {
        m_d->section->pointsModel()->insertRows( m_d->section->pointsModel()->rowCount(), 1 );
    }
}

void SectionThinContGUI::removePoints(){
    QModelIndexList listRows = m_d->ui->pointsTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->pointsModel()->removeRows( listRows.first().row(), listRows.size());
    }
}

Material * SectionThinContGUI::material(){
    if( m_d->section != NULL ){
        return m_d->section->material();
    }
    return NULL;
}

void SectionThinContGUI::setMaterialFromComboBox(){
    if( m_d->section != NULL && m_materialModel != NULL && m_materialComboBox != NULL ){
        if( m_materialComboBox->currentIndex() == -1 ){
            m_d->section->setMaterial( NULL );
        } else {
            m_d->section->setMaterial( m_materialModel->materialId( m_materialComboBox->currentData().toUInt()) );
        }
    }
}
