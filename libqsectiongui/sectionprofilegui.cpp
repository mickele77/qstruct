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
#include "sectionprofilegui.h"
#include "ui_sectionprofilegui.h"

#include "sectiongeometricdatapanel.h"
#include "sectionprofiledatapanel.h"
#include "sectiontransformpanel.h"

#include "sectionprofile.h"
#include "materialmodel.h"
#include "material.h"

#include "varplusguiutility.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "uintplus.h"

class SectionProfileGUIPrivate {
public:
    SectionProfileGUIPrivate( ):
        section(NULL),
        ui(new Ui::SectionProfileGUI) {
    }
    ~SectionProfileGUIPrivate( ){
        delete ui;
    }

    SectionProfile * section;
    Ui::SectionProfileGUI *ui;
    SectionGeometricDataPanel * geometricDataPanel;
    SectionProfileDataPanel * profileDataPanel;
    SectionTransformPanel * transformPanel;
};

SectionProfileGUI::SectionProfileGUI(UnitMeasure* ump, MaterialModel * matModel, QWidget *parent) :
    SectionGUI(ump, matModel, parent),
    m_d( new SectionProfileGUIPrivate() ) {
    m_d->ui->setupUi(this);

    m_d->geometricDataPanel = new SectionGeometricDataPanel( m_d->ui->geometricDataScrollContents );
    m_d->ui->geometricDataScrollContents->layout()->addWidget( m_d->geometricDataPanel );
    m_d->profileDataPanel = new SectionProfileDataPanel( m_d->ui->profileDataScrollContents );
    m_d->ui->profileDataScrollContents->layout()->addWidget( m_d->profileDataPanel );
    m_d->transformPanel = new SectionTransformPanel( m_unitMeasure, m_d->ui->transformationTab );
    m_d->ui->transformationTab->layout()->addWidget( m_d->transformPanel );

    m_materialComboBox = m_d->ui->materialComboBox;
    connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    refreshMaterialComboBox();
}

SectionProfileGUI::~SectionProfileGUI() {
    delete m_d;
}

void SectionProfileGUI::setSection(Section *s){
    if( m_d->section != NULL ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        m_materialComboBox->setCurrentIndex(-1);
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }

    m_d->section = dynamic_cast<SectionProfile *>(s);

    if( m_d->section != NULL ){
        VarPlusGUI::connectVar( m_d->section->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->section->pW, m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        if( m_d->section->material() ){
            m_d->ui->materialComboBox->setCurrentIndex( m_d->ui->materialComboBox->findData( QVariant(m_d->section->material()->id->valueNormal())));
        } else {
            m_d->ui->materialComboBox->setCurrentIndex( -1 );
        }
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }
    m_d->geometricDataPanel->setSection( m_d->section );
    m_d->profileDataPanel->setSection( m_d->section );
    m_d->transformPanel->setSection( m_d->section );
}

Material * SectionProfileGUI::material(){
    if( m_d->section != NULL ){
        return m_d->section->material();
    }
    return NULL;
}

void SectionProfileGUI::setMaterialFromComboBox(){
    if( m_d->section != NULL && m_materialModel != NULL && m_materialComboBox != NULL ){
        if(  m_materialComboBox->currentIndex() == -1 ){
            m_d->section->setMaterial( NULL );
        } else {
            m_d->section->setMaterial( m_materialModel->materialId( m_materialComboBox->currentData().toUInt()) );
        }
    }
}
