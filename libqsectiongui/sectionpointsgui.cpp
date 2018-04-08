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
#include "sectionpointsgui.h"
#include "ui_sectionpointsgui.h"

#include "materialmodel.h"
#include "sectiongeometricdatapanel.h"
#include "sectiontransformpanel.h"
#include "sectionpoints.h"
#include "material.h"

#include "varplusguiutility.h"
#include "point2dplusmodel.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include <QPair>

class SectionPointsGUIPrivate{
public:
    SectionPointsGUIPrivate() {
        section = NULL;
        geometricDataPanel = NULL;
        transformPanel = NULL;
        ui = new Ui::SectionPointsGUI;
    }
    ~SectionPointsGUIPrivate(){
        delete ui;
    }

    void setupUI( UnitMeasure * unitMeasure ){
        geometricDataPanel = new SectionGeometricDataPanel( ui->geometricDataScrollContents );
        QGridLayout * gdlay = dynamic_cast<QGridLayout *>(ui->geometricDataScrollContents->layout());
        if( gdlay ){
            gdlay->addWidget( geometricDataPanel, 0, 0, 1, 1);
            QSpacerItem * verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
            gdlay->addItem(verticalSpacer, 1, 0, 1, 1);
            QSpacerItem * horSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
            gdlay->addItem(horSpacer, 0, 1, 1, 1);
        }

        transformPanel = new SectionTransformPanel( unitMeasure, ui->transformationTab );
        ui->transformationTab->layout()->addWidget(transformPanel);

        ui->pointsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }

    SectionPoints * section;
    Ui::SectionPointsGUI * ui;
    SectionGeometricDataPanel * geometricDataPanel;
    SectionTransformPanel * transformPanel;
};

SectionPointsGUI::SectionPointsGUI(UnitMeasure* ump, MaterialModel * matModel, QWidget *parent) :
    SectionGUI(ump, matModel, parent),
    m_d(new SectionPointsGUIPrivate()) {

    m_d->ui->setupUi(this);

    m_d->setupUI( m_unitMeasure );

    connect( m_d->ui->addPointPButton, SIGNAL(clicked()), this, SLOT(insertPoint()));
    connect( m_d->ui->removePointPButton, SIGNAL(clicked()), this, SLOT(removePoint()));

    m_materialComboBox = m_d->ui->materialComboBox;
    connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    refreshMaterialComboBox();
}

SectionPointsGUI::~SectionPointsGUI() {
    delete m_d;
}

void SectionPointsGUI::setSection(Section * s){
    if( m_d->section != NULL ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        m_materialComboBox->setCurrentIndex(-1);
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );

        m_d->ui->pointsTableView->setModel( NULL );
    }

    m_d->section = dynamic_cast<SectionPoints *>(s);

    if( m_d->section != NULL ){
        VarPlusGUI::connectVar( m_d->section->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->section->pW, m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        if( m_d->section->material() ){
            m_materialComboBox->setCurrentIndex( m_materialComboBox->findData( QVariant( m_d->section->material()->id->valueNormal())) );
        } else {
            m_materialComboBox->setCurrentIndex( -1 );
        }
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );

        m_d->ui->pointsTableView->setModel( m_d->section->pointsModel() );
    }

    m_d->geometricDataPanel->setSection( s );
    m_d->transformPanel->setSection( s );
}

void SectionPointsGUI::insertPoint(){
    QModelIndexList listRows = m_d->ui->pointsTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->pointsModel()->insertRows( listRows.last().row()+1, listRows.size() );
    } else {
        int row = m_d->ui->pointsTableView->currentIndex().row();
        if( row < 0 || row > m_d->section->pointsModel()->pCount() )
            row = m_d->section->pointsModel()->pCount() - 1 ;
        m_d->section->pointsModel()->insertRows( row + 1 );
    }
}

void SectionPointsGUI::removePoint(){
    QModelIndexList listRows = m_d->ui->pointsTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        if( m_d->section != NULL ){
            m_d->section->pointsModel()->removeRows( listRows.first().row(), listRows.size());
        }
    }
}

Material * SectionPointsGUI::material(){
    if( m_d->section != NULL ) {
        return m_d->section->material();
    }
    return NULL;
}

void SectionPointsGUI::setMaterialFromComboBox(){
    if( m_d->section != NULL && m_materialModel != NULL && m_materialComboBox != NULL ){
        if( m_materialComboBox->currentIndex() == -1 ){
            m_d->section->setMaterial( NULL );
        } else {
            m_d->section->setMaterial( m_materialModel->materialId( m_materialComboBox->currentData().toUInt()) );
        }
    }
}
