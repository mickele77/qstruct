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
#include "sectiondefaultgui.h"
#include "ui_sectiondefaultgui.h"

#include "sectiongeometricdatapanel.h"
#include "sectiontransformpanel.h"

#include "section.h"
#include "materialmodel.h"
#include "material.h"
#include "varplusguiutility.h"

#include "qstringplus.h"
#include "doubleplus.h"
#include "uintplus.h"

class SectionDefaultGUIPrivate {
public:
    SectionDefaultGUIPrivate( ):
        section(NULL),
        ui(new Ui::SectionDefaultGUI) {
    };

    Section * section;
    Ui::SectionDefaultGUI *ui;
    SectionGeometricDataPanel * geometricDataPanel;
    SectionTransformPanel * transformPanel;
};

SectionDefaultGUI::SectionDefaultGUI(UnitMeasure* ump, MaterialModel * matModel, QWidget *parent) :
    SectionGUI(ump, matModel, parent),
    m_d( new SectionDefaultGUIPrivate() ) {
    m_d->ui->setupUi(this);

    m_d->geometricDataPanel = new SectionGeometricDataPanel( m_d->ui->geometrcDataScrollAreaContents );
    m_d->ui->geometrcDataScrollAreaContents->layout()->addWidget( m_d->geometricDataPanel );
    m_d->transformPanel = new SectionTransformPanel( m_unitMeasure, m_d->ui->transformationTab );
    m_d->ui->transformationTab->layout()->addWidget(m_d->transformPanel);

    m_materialComboBox = m_d->ui->materialComboBox;
    connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    refreshMaterialComboBox();
}

SectionDefaultGUI::~SectionDefaultGUI() {
    delete m_d->ui;
}

void SectionDefaultGUI::setSection(Section * s){
    if( m_d->section != NULL ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        m_materialComboBox->setCurrentIndex(-1);
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }

    m_d->section = s;

    if( m_d->section != NULL ){
        VarPlusGUI::connectVar( m_d->section->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->section->pW, m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        if( m_d->section->material() ){
            m_materialComboBox->setCurrentIndex( m_d->ui->materialComboBox->findData( QVariant(m_d->section->material()->id->valueNormal())));
        } else {
            m_materialComboBox->setCurrentIndex( -1 );
        }
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }
    m_d->geometricDataPanel->setSection( s );
    m_d->transformPanel->setSection( s );
}

Material * SectionDefaultGUI::material(){
    if( m_d->section != NULL ){
        return m_d->section->material();
    }
    return NULL;
}

void SectionDefaultGUI::setMaterialFromComboBox(){
    if( m_d->section != NULL && m_materialModel != NULL && m_materialComboBox != NULL ){
        if( m_materialComboBox->currentIndex() == -1 ){
            m_d->section->setMaterial( NULL );
        } else {
            m_d->section->setMaterial( m_materialModel->materialId( m_materialComboBox->currentData().toUInt()) );
        }
    }
}
