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
#include "soilsheetgui.h"
#include "ui_soilsheetgui.h"

#include "retainingwallqscombinationgui.h"
#include "retainingwalleqqscombinationgui.h"
#include "soilsheet.h"
#include "soilmodel.h"
#include "soil.h"

#include "point2dplus.h"
#include "doubleplus.h"
#include "qstringplus.h"
#include "uintplus.h"
#include "boolplus.h"

#include "varplusguiutility.h"

#include <QClipboard>
#include <QMimeData>

#include <cmath>

class SoilSheetGUIPrivate {
public:
    SoilSheetGUIPrivate( UnitMeasure * ump, SoilModel * sm ):
        unitMeasure( ump ),
        soilSheet( NULL ),
        soilModel( sm ),
        ui(new Ui::SoilSheetGUI) {

    }
    UnitMeasure * unitMeasure;
    SoilSheet * soilSheet;
    SoilModel * soilModel;
    Ui::SoilSheetGUI * ui;
};

SoilSheetGUI::SoilSheetGUI(UnitMeasure * ump, SoilModel * soilModel, QWidget *parent) :
    QWidget(parent) {
    m_d = new SoilSheetGUIPrivate(ump, soilModel );

    m_d->ui->setupUi(this);
}

SoilSheetGUI::~SoilSheetGUI() {
    delete m_d;
}

void SoilSheetGUI::setSoilSheet( SoilSheet * sheet ) {
    if( m_d->soilSheet != sheet ){
        if( m_d->soilSheet != NULL ){
            VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
            VarPlusGUI::disconnectVar( m_d->ui->soilDownZLabel, m_d->ui->soilDownZLEdit, m_d->ui->soilDownZUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->soilDownDeltaLabel, m_d->ui->soilDownDeltaLEdit, m_d->ui->soilDownDeltaUMLabel, m_d->ui->soilDownDeltaCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->soilUpILabel, m_d->ui->soilUpILEdit, m_d->ui->soilUpIUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->soilUpZLabel, m_d->ui->soilUpZLEdit, m_d->ui->soilUpZUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->soilUpDeltaLabel, m_d->ui->soilUpDeltaLEdit, m_d->ui->soilUpDeltaUMLabel, m_d->ui->soilUpDeltaCheckBox );
        }

        m_d->soilSheet = sheet;
//        m_d->A1C1CombGUI->setRetainingWall( wall );
//        m_d->A1C2CombGUI->setRetainingWall( wall );
//        m_d->A2CombGUI->setRetainingWall( wall );
//        m_d->EQA1C2CombGUI->setRetainingWall( wall );
//        m_d->EQA2CombGUI->setRetainingWall( wall );

        if( m_d->soilSheet != NULL ){
            VarPlusGUI::connectVar( m_d->soilSheet->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
            VarPlusGUI::connectVar( m_d->soilSheet->soilDownZ, m_d->ui->soilDownZLabel, m_d->ui->soilDownZLEdit, m_d->ui->soilDownZUMLabel );
            VarPlusGUI::connectVar( m_d->soilSheet->soilDownDelta, m_d->ui->soilDownDeltaLabel, m_d->ui->soilDownDeltaLEdit, m_d->ui->soilDownDeltaUMLabel, m_d->ui->soilDownDeltaCheckBox );
            VarPlusGUI::connectVar( m_d->soilSheet->soilUpZ, m_d->ui->soilUpZLabel, m_d->ui->soilUpZLEdit, m_d->ui->soilUpZUMLabel );
            VarPlusGUI::connectVar( m_d->soilSheet->soilUpDelta, m_d->ui->soilUpDeltaLabel, m_d->ui->soilUpDeltaLEdit, m_d->ui->soilUpDeltaUMLabel, m_d->ui->soilUpDeltaCheckBox );

            if( m_d->soilSheet->soilDown()  ){
                m_d->ui->soilDownNameComboBox->setCurrentIndex( m_d->ui->soilDownNameComboBox->findData( m_d->soilSheet->soilDown()->id->valueNormal() ) );
            }
            if( m_d->soilSheet->soilUp()  ){
                m_d->ui->soilUpNameComboBox->setCurrentIndex( m_d->ui->soilUpNameComboBox->findData( m_d->soilSheet->soilUp()->id->valueNormal() ) );
            }
        }
    }
}

void SoilSheetGUI::populateSoilComboBox() {
    m_d->ui->soilDownNameComboBox->clear();
    m_d->ui->soilUpNameComboBox->clear();
    for( int i=0; i < m_d->soilModel->soilCount(); i++){
        m_d->ui->soilDownNameComboBox->addItem( m_d->soilModel->soil(i)->name->valueStr(), QVariant(m_d->soilModel->soil(i)->id->valueNormal() ));
        m_d->ui->soilUpNameComboBox->addItem( m_d->soilModel->soil(i)->name->valueStr(), QVariant(m_d->soilModel->soil(i)->id->valueNormal() ));
    }
    if( m_d->soilSheet ){
        if( m_d->soilSheet->soilDown()  ){
            m_d->ui->soilDownNameComboBox->setCurrentIndex( m_d->ui->soilDownNameComboBox->findData( m_d->soilSheet->soilDown()->id->valueNormal() ) );
        }
        if( m_d->soilSheet->soilUp()  ){
            m_d->ui->soilUpNameComboBox->setCurrentIndex( m_d->ui->soilUpNameComboBox->findData( m_d->soilSheet->soilUp()->id->valueNormal() ) );
        }
    }
}

void SoilSheetGUI::showEvent ( QShowEvent * ){
    disconnect( m_d->ui->soilDownNameComboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(setSoilDown(int)));
    disconnect( m_d->ui->soilUpNameComboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(setSoilUp(int)));
    populateSoilComboBox();
    connect( m_d->ui->soilDownNameComboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(setSoilDown(int)));
    connect( m_d->ui->soilUpNameComboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(setSoilUp(int)));
}

void SoilSheetGUI::setSoilDown(int index ) {
    if( m_d->soilSheet ){
        m_d->soilSheet->setSoilDown( m_d->soilModel->soil(index));
    }
}

void SoilSheetGUI::setSoilUp(int index ) {
    if( m_d->soilSheet ){
        m_d->soilSheet->setSoilUp( m_d->soilModel->soil(index));
    }
}
