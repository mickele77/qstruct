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
#include "materialdefaultgui.h"
#include "ui_materialdefaultgui.h"

#include "varplusguiutility.h"

#include "material.h"
#include "qstringplus.h"
#include "doubleplus.h"

MaterialDefaultGUI::MaterialDefaultGUI(UnitMeasure * ump, QWidget *parent) :
    MaterialGUI(ump, parent),
    m_material(NULL),
    m_ui(new Ui::MaterialDefaultGUI){
    m_ui->setupUi(this);
}

MaterialDefaultGUI::~MaterialDefaultGUI() {
    delete m_ui;
}

void MaterialDefaultGUI::setMaterial( Material * mat ){
    if( m_material != NULL ){
        VarPlusGUI::disconnectVar( m_ui->nameLabel, m_ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_ui->gammaWLabel, m_ui->gammaWLEdit, m_ui->gammaWUMLabel );
        VarPlusGUI::disconnectVar( m_ui->ELabel, m_ui->ELEdit, m_ui->EUMLabel );
        VarPlusGUI::disconnectVar( m_ui->GLabel, m_ui->GLEdit, m_ui->GUMLabel, m_ui->GCheckBox );
        VarPlusGUI::disconnectVar( m_ui->nuLabel, m_ui->nuLEdit, m_ui->nuUMLabel );
    }

    m_material = mat;

    if( m_material != NULL ){
        VarPlusGUI::connectVar( m_material->name, m_ui->nameLabel, m_ui->nameLEdit );
        VarPlusGUI::connectVar( m_material->gammaW, m_ui->gammaWLabel, m_ui->gammaWLEdit, m_ui->gammaWUMLabel );
        VarPlusGUI::connectVar( m_material->E, m_ui->ELabel, m_ui->ELEdit, m_ui->EUMLabel );
        VarPlusGUI::connectVar( m_material->G, m_ui->GLabel, m_ui->GLEdit, m_ui->GUMLabel, m_ui->GCheckBox );
        VarPlusGUI::connectVar( m_material->nu, m_ui->nuLabel, m_ui->nuLEdit, m_ui->nuUMLabel );
    }
}
