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
#include "sectionthingui.h"
#include "ui_sectionthingui.h"

#include "sectiongeometricdatapanel.h"
#include "sectiontransformpanel.h"

#include "multisectionlinemodel.h"
#include "multisectionarcmodel.h"
#include "sectionthin.h"
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

class SectionThinGUIPrivate{
public:
    SectionThinGUIPrivate(){
        section = NULL;
        ui = new Ui::SectionThinGUI;
    }
    ~SectionThinGUIPrivate(){
        delete ui;
    }

    SectionThin * section;
    Ui::SectionThinGUI * ui;
    SectionGeometricDataPanel * geometricDataPanel;
    SectionTransformPanel * rotTransPanel;
};

SectionThinGUI::SectionThinGUI(UnitMeasure* ump, MaterialModel * matModel, QWidget *parent) :
    SectionGUI(ump, matModel, parent),
    m_d( new SectionThinGUIPrivate() ){

    m_d->ui->setupUi(this);

    m_d->geometricDataPanel = new SectionGeometricDataPanel( m_d->ui->geometricDataTab );
    m_d->ui->geometricDataScrollAreaWidgetContents->layout()->addWidget( m_d->geometricDataPanel );
    m_d->rotTransPanel = new SectionTransformPanel( m_unitMeasure, m_d->ui->transformationTab );
    m_d->ui->transformationScrollAreaWidgetContents->layout()->addWidget( m_d->rotTransPanel );

    m_d->ui->sectionLineTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect( m_d->ui->addSectionLinePButton, SIGNAL(clicked()), this, SLOT(addSectionLine()));
    connect( m_d->ui->removeSectionLinePButton, SIGNAL(clicked()), this, SLOT(removeSectionLine()));

    m_d->ui->sectionArcTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect( m_d->ui->addSectionArcPButton, SIGNAL(clicked()), this, SLOT(addSectionArc()));
    connect( m_d->ui->removeSectionArcPButton, SIGNAL(clicked()), this, SLOT(removeSectionArc()));

    m_materialComboBox = m_d->ui->materialComboBox;
    connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    refreshMaterialComboBox();
}

SectionThinGUI::~SectionThinGUI() {
    delete m_d;
}

void SectionThinGUI::setSection(Section * s){
    if( m_d->section ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->tLabel, m_d->ui->tLEdit, m_d->ui->tUMLabel );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        m_materialComboBox->setCurrentIndex( -1 );
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }

    m_d->section = dynamic_cast<SectionThin *>(s);

    if( m_d->section ){
        VarPlusGUI::connectVar( m_d->section->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->section->pW, m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::connectVar( m_d->section->t, m_d->ui->tLabel, m_d->ui->tLEdit, m_d->ui->tUMLabel );

        m_d->ui->sectionLineTableView->setModel( m_d->section->sectionLineModel() );
        m_d->ui->sectionArcTableView->setModel( m_d->section->sectionArcModel() );

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

void SectionThinGUI::addSectionLine(){
    QModelIndexList listRows = m_d->ui->sectionLineTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionLineModel()->insertRows( listRows.last().row()+1, listRows.size() );
    } else {
        int row = m_d->ui->sectionLineTableView->currentIndex().row();
        if( row < 0 || row > m_d->section->sectionLineModel()->count() )
            row = m_d->section->sectionLineModel()->count() - 1 ;
        m_d->section->sectionLineModel()->insertRows( row + 1 );
    }
}

void SectionThinGUI::removeSectionLine(){
    QModelIndexList listRows = m_d->ui->sectionLineTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionLineModel()->removeRows( listRows.first().row(), listRows.size());
    }
}

void SectionThinGUI::addSectionArc(){
    QModelIndexList listRows = m_d->ui->sectionArcTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionArcModel()->insertRows( listRows.last().row()+1, listRows.size() );
    } else {
        int row = m_d->ui->sectionArcTableView->currentIndex().row();
        if( row < 0 || row > m_d->section->sectionArcModel()->count() )
            row = m_d->section->sectionArcModel()->count() - 1 ;
        m_d->section->sectionArcModel()->insertRows( row + 1 );
    }
}

void SectionThinGUI::removeSectionArc(){
    QModelIndexList listRows = m_d->ui->sectionArcTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionArcModel()->removeRows( listRows.first().row(), listRows.size());
    }
}

Material * SectionThinGUI::material(){
    if( m_d->section != NULL ){
        return m_d->section->material();
    }
    return NULL;
}

void SectionThinGUI::setMaterialFromComboBox(){
    if( m_d->section != NULL && m_materialModel != NULL && m_materialComboBox != NULL ){
        if( m_materialComboBox->currentIndex() == -1 ){
            m_d->section->setMaterial( NULL );
        } else {
            m_d->section->setMaterial( m_materialModel->materialId( m_materialComboBox->currentData().toUInt()) );
        }
    }
}
