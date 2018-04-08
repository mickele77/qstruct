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
#include "sectiongeometricdatapanel.h"
#include "ui_sectiongeometricdatapanel.h"

#include "section.h"
#include "doubleplus.h"
#include "point2dplus.h"

#include "varplusguiutility.h"

class SectionGeometricDataPanelPrivate{
public:
    SectionGeometricDataPanelPrivate():
        section( NULL ),
        ui(new Ui::SectionGeometricDataPanel ){
    }
    ~SectionGeometricDataPanelPrivate(){
        delete ui;
    }

    Section * section;
    Ui::SectionGeometricDataPanel * ui;
};

SectionGeometricDataPanel::SectionGeometricDataPanel( QWidget *parent) :
    QWidget( parent),
    m_d( new SectionGeometricDataPanelPrivate() ){
    m_d->ui->setupUi( this );
}

SectionGeometricDataPanel::~SectionGeometricDataPanel(){
    delete m_d;
}

Section * SectionGeometricDataPanel::section(){
    return m_d->section;
}

void SectionGeometricDataPanel::setSection(Section * s){
    if( m_d->section != 0 ){
        VarPlusGUI::disconnectVar( m_d->ui->ALabel, m_d->ui->ALEdit, m_d->ui->AUMLabel, m_d->ui->ACheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->SyLabel, m_d->ui->SyLEdit, m_d->ui->SyUMLabel, m_d->ui->SyCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->SzLabel, m_d->ui->SzLEdit, m_d->ui->SzUMLabel, m_d->ui->SzCheckBox);
        VarPlusGUI::disconnectVar(  m_d->ui->IyLabel, m_d->ui->IyLEdit, m_d->ui->IyUMLabel, m_d->ui->IyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->IzLabel, m_d->ui->IzLEdit, m_d->ui->IzUMLabel, m_d->ui->IzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->IyzLabel, m_d->ui->IyzLEdit, m_d->ui->IyzUMLabel, m_d->ui->IyzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->IxLabel, m_d->ui->IxLEdit, m_d->ui->IxUMLabel, m_d->ui->IxCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->Sy3DyDzLabel, m_d->ui->Sy3DyDzLEdit, m_d->ui->Sy3DyDzUMLabel, m_d->ui->Sy3DyDzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->Sz3DyDzLabel, m_d->ui->Sz3DyDzLEdit, m_d->ui->Sz3DyDzUMLabel, m_d->ui->Sz3DyDzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->Sy2zDyDzLabel, m_d->ui->Sy2zDyDzLEdit, m_d->ui->Sy2zDyDzUMLabel, m_d->ui->Sy2zDyDzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->Syz2DyDzLabel, m_d->ui->Syz2DyDzLEdit, m_d->ui->Syz2DyDzUMLabel, m_d->ui->Syz2DyDzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->GyLabel, m_d->ui->GyLEdit, m_d->ui->GyUMLabel, m_d->ui->GyCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->GzLabel, m_d->ui->GzLEdit, m_d->ui->GzUMLabel, m_d->ui->GzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->IGyLabel, m_d->ui->IGyLEdit, m_d->ui->IGyUMLabel, m_d->ui->IGyCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->IGzLabel, m_d->ui->IGzLEdit, m_d->ui->IGzUMLabel, m_d->ui->IGzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->IGyzLabel, m_d->ui->IGyzLEdit, m_d->ui->IGyzUMLabel, m_d->ui->IGyzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->IGxLabel, m_d->ui->IGxLEdit, m_d->ui->IGxUMLabel, m_d->ui->IGxCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->ICyLabel, m_d->ui->ICyLEdit, m_d->ui->ICyUMLabel, m_d->ui->ICyCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->ICzLabel, m_d->ui->ICzLEdit, m_d->ui->ICzUMLabel, m_d->ui->ICzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->ICyzLabel, m_d->ui->ICyzLEdit, m_d->ui->ICyzUMLabel, m_d->ui->ICyzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->alphaCLabel, m_d->ui->alphaCLEdit, m_d->ui->alphaCUMLabel, m_d->ui->alphaCCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->tyLabel, m_d->ui->tyLEdit, m_d->ui->tyUMLabel, m_d->ui->tyCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->tzLabel, m_d->ui->tzLEdit, m_d->ui->tzUMLabel, m_d->ui->tzCheckBox);
        VarPlusGUI::disconnectVar( m_d->ui->tyzLabel, m_d->ui->tyzLEdit, m_d->ui->tyzUMLabel, m_d->ui->tyzCheckBox);
    }

    m_d->section = s;

    if( m_d->section != 0 ){
        VarPlusGUI::connectVar( m_d->section->A, m_d->ui->ALabel, m_d->ui->ALEdit, m_d->ui->AUMLabel, m_d->ui->ACheckBox);
        VarPlusGUI::connectVar( m_d->section->Sy, m_d->ui->SyLabel, m_d->ui->SyLEdit, m_d->ui->SyUMLabel, m_d->ui->SyCheckBox);
        VarPlusGUI::connectVar( m_d->section->Sz, m_d->ui->SzLabel, m_d->ui->SzLEdit, m_d->ui->SzUMLabel, m_d->ui->SzCheckBox);
        VarPlusGUI::connectVar( m_d->section->Iyy, m_d->ui->IyLabel, m_d->ui->IyLEdit, m_d->ui->IyUMLabel, m_d->ui->IyCheckBox );
        VarPlusGUI::connectVar( m_d->section->Izz, m_d->ui->IzLabel, m_d->ui->IzLEdit, m_d->ui->IzUMLabel, m_d->ui->IzCheckBox);
        VarPlusGUI::connectVar( m_d->section->Iyz, m_d->ui->IyzLabel, m_d->ui->IyzLEdit, m_d->ui->IyzUMLabel, m_d->ui->IyzCheckBox);
        VarPlusGUI::connectVar( m_d->section->Ixx, m_d->ui->IxLabel, m_d->ui->IxLEdit, m_d->ui->IxUMLabel, m_d->ui->IxCheckBox);
        VarPlusGUI::connectVar( m_d->section->Sy3DyDz, m_d->ui->Sy3DyDzLabel, m_d->ui->Sy3DyDzLEdit, m_d->ui->Sy3DyDzUMLabel, m_d->ui->Sy3DyDzCheckBox);
        VarPlusGUI::connectVar( m_d->section->Sz3DyDz, m_d->ui->Sz3DyDzLabel, m_d->ui->Sz3DyDzLEdit, m_d->ui->Sz3DyDzUMLabel, m_d->ui->Sz3DyDzCheckBox);
        VarPlusGUI::connectVar( m_d->section->Sy2zDyDz, m_d->ui->Sy2zDyDzLabel, m_d->ui->Sy2zDyDzLEdit, m_d->ui->Sy2zDyDzUMLabel, m_d->ui->Sy2zDyDzCheckBox);
        VarPlusGUI::connectVar( m_d->section->Syz2DyDz, m_d->ui->Syz2DyDzLabel, m_d->ui->Syz2DyDzLEdit, m_d->ui->Syz2DyDzUMLabel, m_d->ui->Syz2DyDzCheckBox);
        VarPlusGUI::connectVar( m_d->section->G->y, m_d->ui->GyLabel, m_d->ui->GyLEdit, m_d->ui->GyUMLabel, m_d->ui->GyCheckBox);
        VarPlusGUI::connectVar( m_d->section->G->z, m_d->ui->GzLabel, m_d->ui->GzLEdit, m_d->ui->GzUMLabel, m_d->ui->GzCheckBox);
        VarPlusGUI::connectVar( m_d->section->IGyy, m_d->ui->IGyLabel, m_d->ui->IGyLEdit, m_d->ui->IGyUMLabel, m_d->ui->IGyCheckBox);
        VarPlusGUI::connectVar( m_d->section->IGzz, m_d->ui->IGzLabel, m_d->ui->IGzLEdit, m_d->ui->IGzUMLabel, m_d->ui->IGzCheckBox);
        VarPlusGUI::connectVar( m_d->section->IGyz, m_d->ui->IGyzLabel, m_d->ui->IGyzLEdit, m_d->ui->IGyzUMLabel, m_d->ui->IGyzCheckBox);
        VarPlusGUI::connectVar( m_d->section->IGxx, m_d->ui->IGxLabel, m_d->ui->IGxLEdit, m_d->ui->IGxUMLabel, m_d->ui->IGxCheckBox);
        VarPlusGUI::connectVar( m_d->section->ICyy, m_d->ui->ICyLabel, m_d->ui->ICyLEdit, m_d->ui->ICyUMLabel, m_d->ui->ICyCheckBox);
        VarPlusGUI::connectVar( m_d->section->ICzz, m_d->ui->ICzLabel, m_d->ui->ICzLEdit, m_d->ui->ICzUMLabel, m_d->ui->ICzCheckBox);
        VarPlusGUI::connectVar( m_d->section->ICyz, m_d->ui->ICyzLabel, m_d->ui->ICyzLEdit, m_d->ui->ICyzUMLabel, m_d->ui->ICyzCheckBox);
        VarPlusGUI::connectVar( m_d->section->thetaC, m_d->ui->alphaCLabel, m_d->ui->alphaCLEdit, m_d->ui->alphaCUMLabel, m_d->ui->alphaCCheckBox);
        VarPlusGUI::connectVar( m_d->section->ty, m_d->ui->tyLabel, m_d->ui->tyLEdit, m_d->ui->tyUMLabel, m_d->ui->tyCheckBox);
        VarPlusGUI::connectVar( m_d->section->tz, m_d->ui->tzLabel, m_d->ui->tzLEdit, m_d->ui->tzUMLabel, m_d->ui->tzCheckBox);
        VarPlusGUI::connectVar( m_d->section->tyz, m_d->ui->tyzLabel, m_d->ui->tyzLEdit, m_d->ui->tyzUMLabel, m_d->ui->tyzCheckBox);
    }
}
