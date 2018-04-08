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
#include "frpgui.h"
#include "ui_frpgui.h"

#include "qstringplus.h"
#include "doubleplus.h"
#include "varplusguiutility.h"
#include "frp.h"

class FRPGUIPrivate {
public:
    FRPGUIPrivate(const QString & sFile):
        settingsFile(sFile),
        ui(new Ui::FRPGUI),
        frp(NULL) {
    }

    ~FRPGUIPrivate(){
        delete ui;
    }

    QString settingsFile;
    Ui::FRPGUI * ui;
    FRP * frp;
};

FRPGUI::FRPGUI(UnitMeasure * um, const QString &settingsFile, QWidget *parent) :
    MaterialGUI( um, parent),
    m_d(new FRPGUIPrivate(settingsFile)){
    m_d->ui->setupUi(this);
}

FRPGUI::~FRPGUI() {
    delete m_d;
}

void FRPGUI::setMaterial( Material * frp ){
    if( m_d->frp != NULL ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLineEdit );
        VarPlusGUI::disconnectVar( m_d->ui->EfLabel, m_d->ui->EfLineEdit, m_d->ui->EfUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->ffkLabel, m_d->ui->ffkLineEdit, m_d->ui->ffkUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->epsfkLabel, m_d->ui->epsfkLineEdit, m_d->ui->epsfkUMLabel, m_d->ui->epsfkCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->gammafLabel, m_d->ui->gammafLineEdit, m_d->ui->gammafUMLabel, m_d->ui->gammafCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->gammafdLabel, m_d->ui->gammafdLineEdit, m_d->ui->gammafdUMLabel, m_d->ui->gammafdCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->etaaLabel, m_d->ui->etaaLineEdit, m_d->ui->etaaUMLabel, m_d->ui->etaaCheckBox );
        VarPlusGUI::disconnectEnumVar( m_d->ui->applicationTypeLabel, m_d->ui->applicationTypeComboBox );
        VarPlusGUI::disconnectEnumVar( m_d->ui->fiberTypeLabel, m_d->ui->fiberTypeComboBox );
        VarPlusGUI::disconnectEnumVar( m_d->ui->expositionLabel, m_d->ui->expositionComboBox );
    }

    m_d->frp = dynamic_cast<FRP *>(frp);

    if( m_d->frp != NULL ){
        VarPlusGUI::connectVar( m_d->frp->name, m_d->ui->nameLabel, m_d->ui->nameLineEdit );
        VarPlusGUI::connectVar( m_d->frp->E, m_d->ui->EfLabel, m_d->ui->EfLineEdit, m_d->ui->EfUMLabel );
        VarPlusGUI::connectVar( m_d->frp->ffk, m_d->ui->ffkLabel, m_d->ui->ffkLineEdit, m_d->ui->ffkUMLabel );
        VarPlusGUI::connectVar( m_d->frp->epsfk, m_d->ui->epsfkLabel, m_d->ui->epsfkLineEdit, m_d->ui->epsfkUMLabel, m_d->ui->epsfkCheckBox );
        VarPlusGUI::connectVar( m_d->frp->gammaf, m_d->ui->gammafLabel, m_d->ui->gammafLineEdit, m_d->ui->gammafUMLabel, m_d->ui->gammafCheckBox );
        VarPlusGUI::connectVar( m_d->frp->gammafd, m_d->ui->gammafdLabel, m_d->ui->gammafdLineEdit, m_d->ui->gammafdUMLabel, m_d->ui->gammafdCheckBox );
        VarPlusGUI::connectVar( m_d->frp->etaa, m_d->ui->etaaLabel, m_d->ui->etaaLineEdit, m_d->ui->etaaUMLabel, m_d->ui->etaaCheckBox );
        VarPlusGUI::connectEnumVar( m_d->frp->applicationType, m_d->ui->applicationTypeLabel, m_d->ui->applicationTypeComboBox );
        VarPlusGUI::connectEnumVar( m_d->frp->fiberType, m_d->ui->fiberTypeLabel, m_d->ui->fiberTypeComboBox );
        VarPlusGUI::connectEnumVar( m_d->frp->exposition, m_d->ui->expositionLabel, m_d->ui->expositionComboBox );
    }
}

