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
#include "sectionprofiledatapanel.h"
#include "ui_sectionprofiledatapanel.h"

#include "sectionprofile.h"

#include "varplusguiutility.h"
#include "point2dplus.h"
#include "doubleplus.h"

class SectionProfileDataPanelPrivate{
public:
    SectionProfileDataPanelPrivate():
    section(NULL),
    ui(new Ui::SectionProfileDataPanel ){

    };
    ~SectionProfileDataPanelPrivate(){
        delete ui;
    };
    SectionProfile * section;
    Ui::SectionProfileDataPanel * ui;
};


SectionProfileDataPanel::SectionProfileDataPanel(QWidget *parent) :
    QWidget(parent),
    m_d( new SectionProfileDataPanelPrivate ){
    m_d->ui->setupUi( this );
}

SectionProfileDataPanel::~SectionProfileDataPanel() {
    delete m_d;
}

SectionProfile *SectionProfileDataPanel::section(){
    return m_d->section;
}

void SectionProfileDataPanel::setSection(SectionProfile * s) {
    if( m_d->section != NULL ){
        VarPlusGUI::disconnectVar( m_d->ui->yDLabel, m_d->ui->yDLEdit, m_d->ui->yDUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->zDLabel, m_d->ui->zDLEdit, m_d->ui->zDUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->thetaDLabel, m_d->ui->thetaDLEdit, m_d->ui->thetaDUMLabel);

        VarPlusGUI::disconnectVar( m_d->ui->ALabel, m_d->ui->ALEdit, m_d->ui->AUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->SDyLabel, m_d->ui->SDyLEdit, m_d->ui->SDyUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->SDzLabel, m_d->ui->SDzLEdit, m_d->ui->SDzUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->IDyLabel, m_d->ui->IDyLEdit, m_d->ui->IDyUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->IDzLabel, m_d->ui->IDzLEdit, m_d->ui->IDzUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->IDyzLabel, m_d->ui->IDyzLEdit, m_d->ui->IDyzUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->SDy3DyDzLabel, m_d->ui->SDy3DyDzLEdit, m_d->ui->SDy3DyDzUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->SDz3DyDzLabel, m_d->ui->SDz3DyDzLEdit, m_d->ui->SDz3DyDzUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->SDy2zDyDzLabel, m_d->ui->SDy2zDyDzLEdit, m_d->ui->SDy2zDyDzUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->SDyz2DyDzLabel, m_d->ui->SDyz2DyDzLEdit, m_d->ui->SDyz2DyDzUMLabel);
    }

    m_d->section = dynamic_cast<SectionProfile *>(s);

    if( m_d->section != NULL ){
        VarPlusGUI::connectVar( m_d->section->D->y, m_d->ui->yDLabel, m_d->ui->yDLEdit, m_d->ui->yDUMLabel);
        VarPlusGUI::connectVar( m_d->section->D->z, m_d->ui->zDLabel, m_d->ui->zDLEdit, m_d->ui->zDUMLabel);
        VarPlusGUI::connectVar( m_d->section->thetaD, m_d->ui->thetaDLabel, m_d->ui->thetaDLEdit, m_d->ui->thetaDUMLabel);

        VarPlusGUI::connectVar( m_d->section->A, m_d->ui->ALabel, m_d->ui->ALEdit, m_d->ui->AUMLabel);
        VarPlusGUI::connectVar( m_d->section->SDy, m_d->ui->SDyLabel, m_d->ui->SDyLEdit, m_d->ui->SDyUMLabel);
        VarPlusGUI::connectVar( m_d->section->SDz, m_d->ui->SDzLabel, m_d->ui->SDzLEdit, m_d->ui->SDzUMLabel);
        VarPlusGUI::connectVar( m_d->section->IDyy, m_d->ui->IDyLabel, m_d->ui->IDyLEdit, m_d->ui->IDyUMLabel);
        VarPlusGUI::connectVar( m_d->section->IDzz, m_d->ui->IDzLabel, m_d->ui->IDzLEdit, m_d->ui->IDzUMLabel);
        VarPlusGUI::connectVar( m_d->section->IDyz, m_d->ui->IDyzLabel, m_d->ui->IDyzLEdit, m_d->ui->IDyzUMLabel);
        VarPlusGUI::connectVar( m_d->section->SDy3DyDz, m_d->ui->SDy3DyDzLabel, m_d->ui->SDy3DyDzLEdit, m_d->ui->SDy3DyDzUMLabel);
        VarPlusGUI::connectVar( m_d->section->SDz3DyDz, m_d->ui->SDz3DyDzLabel, m_d->ui->SDz3DyDzLEdit, m_d->ui->SDz3DyDzUMLabel);
        VarPlusGUI::connectVar( m_d->section->SDy2zDyDz, m_d->ui->SDy2zDyDzLabel, m_d->ui->SDy2zDyDzLEdit, m_d->ui->SDy2zDyDzUMLabel);
        VarPlusGUI::connectVar( m_d->section->SDyz2DyDz, m_d->ui->SDyz2DyDzLabel, m_d->ui->SDyz2DyDzLEdit, m_d->ui->SDyz2DyDzUMLabel);
    }
}
