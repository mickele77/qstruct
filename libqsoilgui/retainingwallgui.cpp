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
#include "retainingwallgui.h"
#include "retainingwallguiprivate.h"

#include "retainingwallqscombinationgui.h"
#include "retainingwalleqqscombinationgui.h"
#include "retainingwall.h"
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

RetainingWallGUI::RetainingWallGUI(UnitMeasure * ump, SoilModel * soilModel, QWidget *parent) :
    QWidget(parent) {
    m_d = new RetainingWallGUIPrivate(ump, soilModel, this );

    m_d->ui->setupUi(this);

    m_d->ui->A1C1GBox->layout()->addWidget( m_d->A1C1CombGUI );
    m_d->ui->A1C2GBox->layout()->addWidget( m_d->A1C2CombGUI );
    m_d->ui->A2GBox->layout()->addWidget( m_d->A2CombGUI );
    m_d->ui->EQQSA1C2GBox->layout()->addWidget( m_d->EQA1C2CombGUI );
    m_d->ui->EQQSA2GBox->layout()->addWidget( m_d->EQA2CombGUI );

    setVar();

    connect( m_d->ui->overturningPushButton, SIGNAL(clicked()), this, SLOT(calculateOverTurning()));
    connect( m_d->ui->EQoverturningPushButton, SIGNAL(clicked()), this, SLOT(calculateEQOverTurning()));
    connect( m_d->ui->A1PushButton, SIGNAL(clicked()), this, SLOT(calculateA1()));

    connect( m_d->ui->overTurningCopyClipPButton, SIGNAL(clicked()), this, SLOT(overTurningCopyClip()));
    connect( m_d->ui->EQoverturningCopyClipPButton, SIGNAL(clicked()), this, SLOT(EQOverTurningCopyClip()));
}

RetainingWallGUI::~RetainingWallGUI() {
    delete m_d;
}

void RetainingWallGUI::setVar(){
    VarPlusGUI::connectVar( m_d->gammaQLimR1, m_d->ui->gammaQLimR1Label, m_d->ui->gammaQLimR1LEdit, m_d->ui->gammaQLimR1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaSlidingR1, m_d->ui->gammaSlidingR1Label, m_d->ui->gammaSlidingR1LEdit, m_d->ui->gammaSlidingR1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaRdDownR1, m_d->ui->gammaKpR1Label, m_d->ui->gammaKpR1LEdit, m_d->ui->gammaKpR1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaQLimR2, m_d->ui->gammaQLimR2Label, m_d->ui->gammaQLimR2LEdit, m_d->ui->gammaQLimR2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaSlidingR2, m_d->ui->gammaSlidingR2Label, m_d->ui->gammaSlidingR2LEdit, m_d->ui->gammaSlidingR2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaRdDownR2, m_d->ui->gammaKpR2Label, m_d->ui->gammaKpR2LEdit, m_d->ui->gammaKpR2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaQLimR3, m_d->ui->gammaQLimR3Label, m_d->ui->gammaQLimR3LEdit, m_d->ui->gammaQLimR3UMLabel );
    VarPlusGUI::connectVar( m_d->gammaSlidingR3, m_d->ui->gammaSlidingR3Label, m_d->ui->gammaSlidingR3LEdit, m_d->ui->gammaSlidingR3UMLabel );
    VarPlusGUI::connectVar( m_d->gammaRdDownR3, m_d->ui->gammaKpR3Label, m_d->ui->gammaKpR3LEdit, m_d->ui->gammaKpR3UMLabel );

    VarPlusGUI::connectVar( m_d->gammaTanPhiPrimeM1, m_d->ui->gammaTanPhiPrimeM1Label, m_d->ui->gammaTanPhiPrimeM1LEdit, m_d->ui->gammaTanPhiPrimeM1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaCPrimeM1, m_d->ui->gammaCPrimeM1Label, m_d->ui->gammaCPrimeM1LEdit, m_d->ui->gammaCPrimeM1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaCUM1, m_d->ui->gammaCUM1Label, m_d->ui->gammaCUM1LEdit, m_d->ui->gammaCUM1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaGammaWM1, m_d->ui->gammaGammaWM1Label, m_d->ui->gammaGammaWM1LEdit, m_d->ui->gammaGammaWM1UMLabel );

    VarPlusGUI::connectVar( m_d->gammaTanPhiPrimeM2, m_d->ui->gammaTanPhiPrimeM2Label, m_d->ui->gammaTanPhiPrimeM2LEdit, m_d->ui->gammaTanPhiPrimeM2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaCPrimeM2, m_d->ui->gammaCPrimeM2Label, m_d->ui->gammaCPrimeM2LEdit, m_d->ui->gammaCPrimeM2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaCUM2, m_d->ui->gammaCUM2Label, m_d->ui->gammaCUM2LEdit, m_d->ui->gammaCUM2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaGammaWM2, m_d->ui->gammaGammaWM2Label, m_d->ui->gammaGammaWM2LEdit, m_d->ui->gammaGammaWM2UMLabel );

    VarPlusGUI::connectVar( m_d->gammaG1MinEQU, m_d->ui->gammaG1MinEQULabel, m_d->ui->gammaG1MinEQULEdit, m_d->ui->gammaG1MinEQUUMLabel );
    VarPlusGUI::connectVar( m_d->gammaG1MaxEQU, m_d->ui->gammaG1MaxEQULabel, m_d->ui->gammaG1MaxEQULEdit, m_d->ui->gammaG1MaxEQUUMLabel );
    VarPlusGUI::connectVar( m_d->gammaG2MinEQU, m_d->ui->gammaG2MinEQULabel, m_d->ui->gammaG2MinEQULEdit, m_d->ui->gammaG2MinEQUUMLabel );
    VarPlusGUI::connectVar( m_d->gammaG2MaxEQU, m_d->ui->gammaG2MaxEQULabel, m_d->ui->gammaG2MaxEQULEdit, m_d->ui->gammaG2MaxEQUUMLabel );
    VarPlusGUI::connectVar( m_d->gammaQMinEQU, m_d->ui->gammaQMinEQULabel, m_d->ui->gammaQMinEQULEdit, m_d->ui->gammaQMinEQUUMLabel );
    VarPlusGUI::connectVar( m_d->gammaQMaxEQU, m_d->ui->gammaQMaxEQULabel, m_d->ui->gammaQMaxEQULEdit, m_d->ui->gammaQMaxEQUUMLabel );

    VarPlusGUI::connectVar( m_d->gammaG1MinA1, m_d->ui->gammaG1MinA1Label, m_d->ui->gammaG1MinA1LEdit, m_d->ui->gammaG1MinA1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaG1MaxA1, m_d->ui->gammaG1MaxA1Label, m_d->ui->gammaG1MaxA1LEdit, m_d->ui->gammaG1MaxA1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaG2MinA1, m_d->ui->gammaG2MinA1Label, m_d->ui->gammaG2MinA1LEdit, m_d->ui->gammaG2MinA1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaG2MaxA1, m_d->ui->gammaG2MaxA1Label, m_d->ui->gammaG2MaxA1LEdit, m_d->ui->gammaG2MaxA1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaQMinA1, m_d->ui->gammaQMinA1Label, m_d->ui->gammaQMinA1LEdit, m_d->ui->gammaQMinA1UMLabel );
    VarPlusGUI::connectVar( m_d->gammaQMaxA1, m_d->ui->gammaQMaxA1Label, m_d->ui->gammaQMaxA1LEdit, m_d->ui->gammaQMaxA1UMLabel );

    VarPlusGUI::connectVar( m_d->gammaG1MinA2, m_d->ui->gammaG1MinA2Label, m_d->ui->gammaG1MinA2LEdit, m_d->ui->gammaG1MinA2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaG1MaxA2, m_d->ui->gammaG1MaxA2Label, m_d->ui->gammaG1MaxA2LEdit, m_d->ui->gammaG1MaxA2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaG2MinA2, m_d->ui->gammaG2MinA2Label, m_d->ui->gammaG2MinA2LEdit, m_d->ui->gammaG2MinA2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaG2MaxA2, m_d->ui->gammaG2MaxA2Label, m_d->ui->gammaG2MaxA2LEdit, m_d->ui->gammaG2MaxA2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaQMinA2, m_d->ui->gammaQMinA2Label, m_d->ui->gammaQMinA2LEdit, m_d->ui->gammaQMinA2UMLabel );
    VarPlusGUI::connectVar( m_d->gammaQMaxA2, m_d->ui->gammaQMaxA2Label, m_d->ui->gammaQMaxA2LEdit, m_d->ui->gammaQMaxA2UMLabel );

    VarPlusGUI::connectVar( m_d->Gk2, m_d->ui->Gk2Label, m_d->ui->Gk2LEdit, m_d->ui->Gk2UMLabel );
    VarPlusGUI::connectVar( m_d->Qk, m_d->ui->QkLabel, m_d->ui->QkLEdit, m_d->ui->QkUMLabel );
    VarPlusGUI::connectVar( m_d->psi2Q, m_d->ui->psi2QLabel, m_d->ui->psi2QLEdit, m_d->ui->psi2QUMLabel );

    VarPlusGUI::connectVar( m_d->overTurningSoilUpPhiPrimeD, m_d->ui->overturningSoilUpPhiPrimeDLabel, m_d->ui->overturningSoilUpPhiPrimeDLEdit, m_d->ui->overturningSoilUpPhiPrimeDUMLabel, m_d->ui->overturningSoilUpPhiPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningSoilUpDeltaD, m_d->ui->overturningSoilUpDeltaDLabel, m_d->ui->overturningSoilUpDeltaDLEdit, m_d->ui->overturningSoilUpDeltaDUMLabel, m_d->ui->overturningSoilUpDeltaDCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningSoilUpCPrimeD, m_d->ui->overturningSoilUpCPrimeDLabel, m_d->ui->overturningSoilUpCPrimeDLEdit, m_d->ui->overturningSoilUpCPrimeDUMLabel, m_d->ui->overturningSoilUpCPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningSoilDownPhiPrimeD, m_d->ui->overturningSoilDownPhiPrimeDLabel, m_d->ui->overturningSoilDownPhiPrimeDLEdit, m_d->ui->overturningSoilDownPhiPrimeDUMLabel, m_d->ui->overturningSoilDownPhiPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningSoilDownCPrimeD, m_d->ui->overturningSoilDownCPrimeDLabel, m_d->ui->overturningSoilDownCPrimeDLEdit, m_d->ui->overturningSoilDownCPrimeDUMLabel, m_d->ui->overturningSoilDownCPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningSoilDownDeltaD, m_d->ui->overturningSoilDownDeltaDLabel, m_d->ui->overturningSoilDownDeltaDLEdit, m_d->ui->overturningSoilDownDeltaDUMLabel, m_d->ui->overturningSoilDownDeltaDCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningKa, m_d->ui->overturningKaLabel, m_d->ui->overturningKaLEdit, m_d->ui->overturningKaUMLabel, m_d->ui->overturningKaCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningChiKp, m_d->ui->overturningChiKpLabel, m_d->ui->overturningChiKpLEdit, m_d->ui->overturningChiKpUMLabel);
    VarPlusGUI::connectVar( m_d->overTurningKp, m_d->ui->overturningKpLabel, m_d->ui->overturningKpLEdit, m_d->ui->overturningKpUMLabel, m_d->ui->overturningKpCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningKpRed, m_d->ui->overturningKpRedLabel, m_d->ui->overturningKpRedLEdit, m_d->ui->overturningKpRedUMLabel, m_d->ui->overturningKpRedCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningC->y, m_d->ui->overturningYCLabel, m_d->ui->overturningYCLEdit, m_d->ui->overturningYCUMLabel, m_d->ui->overturningYCCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningC->z, m_d->ui->overturningZCLabel, m_d->ui->overturningZCLEdit, m_d->ui->overturningZCUMLabel, m_d->ui->overturningZCCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningMEd, m_d->ui->overturningMEdLabel, m_d->ui->overturningMEdLEdit, m_d->ui->overturningMEdUMLabel);
    VarPlusGUI::connectVar( m_d->overTurningApplyKp, m_d->ui->overturningApplyKpLabel, 0, 0, m_d->ui->overturningApplyKpCheckBox );
    VarPlusGUI::connectVar( m_d->overTurningMRd, m_d->ui->overturningMRdLabel, m_d->ui->overturningMRdLEdit, m_d->ui->overturningMRdUMLabel);

    VarPlusGUI::connectVar( m_d->EQag, m_d->ui->EQagLabel, m_d->ui->EQagLEdit, m_d->ui->EQagUMLabel );
    VarPlusGUI::connectVar( m_d->EQF0, m_d->ui->EQF0Label, m_d->ui->EQF0LEdit, m_d->ui->EQF0UMLabel );
    VarPlusGUI::connectVar( m_d->EQSS, m_d->ui->EQSSLabel, m_d->ui->EQSSLEdit, m_d->ui->EQSSUMLabel, m_d->ui->EQSSCheckBox );
    VarPlusGUI::connectVar( m_d->EQST, m_d->ui->EQSTLabel, m_d->ui->EQSTLEdit, m_d->ui->EQSTUMLabel );
    VarPlusGUI::connectVar( m_d->EQS, m_d->ui->EQSLabel, m_d->ui->EQSLEdit, m_d->ui->EQSUMLabel, m_d->ui->EQSCheckBox );
    VarPlusGUI::connectVar( m_d->EQaMax, m_d->ui->EQaMaxLabel, m_d->ui->EQaMaxLEdit, m_d->ui->EQaMaxUMLabel, m_d->ui->EQaMaxCheckBox );
    VarPlusGUI::connectVar( m_d->EQbetaM, m_d->ui->EQbetaMLabel, m_d->ui->EQbetaMLEdit, m_d->ui->EQbetaMUMLabel, m_d->ui->EQbetaMCheckBox );
    VarPlusGUI::connectVar( m_d->EQkh, m_d->ui->EQkhLabel, m_d->ui->EQkhLEdit, m_d->ui->EQkhUMLabel, m_d->ui->EQkhCheckBox );
    VarPlusGUI::connectVar( m_d->EQkv, m_d->ui->EQkvLabel, m_d->ui->EQkvLEdit, m_d->ui->EQkvUMLabel, m_d->ui->EQkvCheckBox );
    VarPlusGUI::connectEnumVar( m_d->EQDirection, m_d->ui->EQDirectionLabel, m_d->ui->EQDirectionComboBox );
    VarPlusGUI::connectVar( m_d->EQtheta, m_d->ui->EQthetaLabel, m_d->ui->EQthetaLEdit, m_d->ui->EQthetaUMLabel, m_d->ui->EQthetaCheckBox );

    VarPlusGUI::connectVar( m_d->EQoverTurningSoilUpPhiPrimeD, m_d->ui->EQoverturningSoilUpPhiPrimeDLabel, m_d->ui->EQoverturningSoilUpPhiPrimeDLEdit, m_d->ui->EQoverturningSoilUpPhiPrimeDUMLabel, m_d->ui->EQoverturningSoilUpPhiPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningSoilUpCPrimeD, m_d->ui->EQoverturningSoilUpCPrimeDLabel, m_d->ui->EQoverturningSoilUpCPrimeDLEdit, m_d->ui->EQoverturningSoilUpCPrimeDUMLabel, m_d->ui->EQoverturningSoilUpCPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningSoilUpDeltaD, m_d->ui->EQoverturningSoilUpDeltaDLabel, m_d->ui->EQoverturningSoilUpDeltaDLEdit, m_d->ui->EQoverturningSoilUpDeltaDUMLabel, m_d->ui->EQoverturningSoilUpDeltaDCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningSoilDownPhiPrimeD, m_d->ui->EQoverturningSoilDownPhiPrimeDLabel, m_d->ui->EQoverturningSoilDownPhiPrimeDLEdit, m_d->ui->EQoverturningSoilDownPhiPrimeDUMLabel, m_d->ui->EQoverturningSoilDownPhiPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningSoilDownCPrimeD, m_d->ui->EQoverturningSoilDownCPrimeDLabel, m_d->ui->EQoverturningSoilDownCPrimeDLEdit, m_d->ui->EQoverturningSoilDownCPrimeDUMLabel, m_d->ui->EQoverturningSoilDownCPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningSoilDownDeltaD, m_d->ui->EQoverturningSoilDownDeltaDLabel, m_d->ui->EQoverturningSoilDownDeltaDLEdit, m_d->ui->EQoverturningSoilDownDeltaDUMLabel, m_d->ui->EQoverturningSoilDownDeltaDCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningKa, m_d->ui->EQoverturningKaLabel, m_d->ui->EQoverturningKaLEdit, m_d->ui->EQoverturningKaUMLabel, m_d->ui->EQoverturningKaCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningKp, m_d->ui->EQoverturningKpLabel, m_d->ui->EQoverturningKpLEdit, m_d->ui->EQoverturningKpUMLabel, m_d->ui->EQoverturningKpCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningChiKp, m_d->ui->EQoverturningChiKpLabel, m_d->ui->EQoverturningChiKpLEdit, m_d->ui->EQoverturningChiKpUMLabel);
    VarPlusGUI::connectVar( m_d->EQoverTurningKpRed, m_d->ui->EQoverturningKpRedLabel, m_d->ui->EQoverturningKpRedLEdit, m_d->ui->EQoverturningKpRedUMLabel, m_d->ui->EQoverturningKpRedCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningC->y, m_d->ui->EQoverturningYCLabel, m_d->ui->EQoverturningYCLEdit, m_d->ui->EQoverturningYCUMLabel, m_d->ui->EQoverturningYCCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningC->z, m_d->ui->EQoverturningZCLabel, m_d->ui->EQoverturningZCLEdit, m_d->ui->EQoverturningZCUMLabel, m_d->ui->EQoverturningZCCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningApplyKp, m_d->ui->EQoverturningApplyKpLabel, 0, 0, m_d->ui->EQoverturningApplyKpCheckBox );
    VarPlusGUI::connectVar( m_d->EQoverTurningMRd, m_d->ui->EQoverturningMRdLabel, m_d->ui->EQoverturningMRdLEdit, m_d->ui->EQoverturningMRdUMLabel);
    VarPlusGUI::connectVar( m_d->EQoverTurningMEd, m_d->ui->EQoverturningMEdLabel, m_d->ui->EQoverturningMEdLEdit, m_d->ui->EQoverturningMEdUMLabel);
}

void RetainingWallGUI::setRetainingWall( RetainingWall * wall ) {
    if( m_d->retainingWall != wall ){
        if( m_d->retainingWall != NULL ){
            VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
            VarPlusGUI::disconnectVar( m_d->ui->soilDownZLabel, m_d->ui->soilDownZLEdit, m_d->ui->soilDownZUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->soilDownDeltaLabel, m_d->ui->soilDownDeltaLEdit, m_d->ui->soilDownDeltaUMLabel, m_d->ui->soilDownDeltaCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->soilUpILabel, m_d->ui->soilUpILEdit, m_d->ui->soilUpIUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->soilUpZLabel, m_d->ui->soilUpZLEdit, m_d->ui->soilUpZUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->soilUpDeltaLabel, m_d->ui->soilUpDeltaLEdit, m_d->ui->soilUpDeltaUMLabel, m_d->ui->soilUpDeltaCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->baseAlphaLabel, m_d->ui->baseAlphaLEdit, m_d->ui->baseAlphaUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->toeGammaWLabel, m_d->ui->toeGammaWLEdit, m_d->ui->toeGammaWUMLabel, m_d->ui->toeGammaWCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->toeHLeftLabel, m_d->ui->toeHLeftLEdit, m_d->ui->toeHLeftUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->toeHRightLabel, m_d->ui->toeHRightLEdit, m_d->ui->toeHRightUMLabel, m_d->ui->toeHRightCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->toeBLabel, m_d->ui->toeBLEdit, m_d->ui->toeBUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->toeDLabel, m_d->ui->toeDLEdit, m_d->ui->toeDUMLabel, m_d->ui->toeDCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->heelGammaWLabel, m_d->ui->heelGammaWLEdit, m_d->ui->heelGammaWUMLabel, m_d->ui->heelGammaWCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->heelHLeftLabel, m_d->ui->heelHLeftLEdit, m_d->ui->heelHLeftUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->heelHRightLabel, m_d->ui->heelHRightLEdit, m_d->ui->heelHRightUMLabel, m_d->ui->heelHRightCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->heelBLabel, m_d->ui->heelBLEdit, m_d->ui->heelBUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->heelDLabel, m_d->ui->heelDLEdit, m_d->ui->heelDUMLabel, m_d->ui->heelDCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->keyGammaWLabel, m_d->ui->keyGammaWLEdit, m_d->ui->keyGammaWUMLabel, m_d->ui->keyGammaWCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->keyYLabel, m_d->ui->keyYLEdit, m_d->ui->keyYUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->keyHLabel, m_d->ui->keyHLEdit, m_d->ui->keyHUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->keyBLabel, m_d->ui->keyBLEdit, m_d->ui->keyBUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->keyDLabel, m_d->ui->keyDLEdit, m_d->ui->keyDUMLabel, m_d->ui->keyDCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->stemGammaWLabel, m_d->ui->stemGammaWLEdit, m_d->ui->stemGammaWUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->stemBetaLabel, m_d->ui->stemBetaLEdit, m_d->ui->stemBetaUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->stemTBottomLabel, m_d->ui->stemTBottomLEdit, m_d->ui->stemTBottomUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->stemTTopLabel, m_d->ui->stemTTopLEdit, m_d->ui->stemTTopUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->stemHLabel, m_d->ui->stemHLEdit, m_d->ui->stemHUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->stemDLabel, m_d->ui->stemDLEdit, m_d->ui->stemDUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->counterfortGammaWLabel, m_d->ui->counterfortGammaWLEdit, m_d->ui->counterfortGammaWUMLabel, m_d->ui->counterfortGammaWCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->counterfortBBottomLabel, m_d->ui->counterfortBBottomLEdit, m_d->ui->counterfortBBottomUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->counterfortBTopLabel, m_d->ui->counterfortBTopLEdit, m_d->ui->counterfortBTopUMLabel );
            VarPlusGUI::disconnectVar( m_d->ui->counterfortHLabel, m_d->ui->counterfortHLEdit, m_d->ui->counterfortHUMLabel, m_d->ui->counterfortHCheckBox );
            VarPlusGUI::disconnectVar( m_d->ui->counterfortTLabel, m_d->ui->counterfortTLEdit, m_d->ui->counterfortTUMLabel );

            disconnect( m_d->retainingWall, SIGNAL(soilDownChanged(Soil*, Soil*)), this, SLOT(connectSoilDown(Soil*, Soil*)));
            if( m_d->retainingWall->soilDown()  ){
                disconnect( m_d->retainingWall->soilDown()->soilEQCategory, SIGNAL(valueChanged(QString)), m_d, SLOT(setEQSS()));
                disconnect( m_d->retainingWall->soilDown()->soilEQCategory, SIGNAL(valueChanged(QString)), m_d, SLOT(setEQbetaM()));
            }
        }

        m_d->retainingWall = wall;
        m_d->A1C1CombGUI->setRetainingWall( wall );
        m_d->A1C2CombGUI->setRetainingWall( wall );
        m_d->A2CombGUI->setRetainingWall( wall );
        m_d->EQA1C2CombGUI->setRetainingWall( wall );
        m_d->EQA2CombGUI->setRetainingWall( wall );

        if( m_d->retainingWall != NULL ){
            VarPlusGUI::connectVar( m_d->retainingWall->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
            VarPlusGUI::connectVar( m_d->retainingWall->soilDownZ, m_d->ui->soilDownZLabel, m_d->ui->soilDownZLEdit, m_d->ui->soilDownZUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->soilDownDelta, m_d->ui->soilDownDeltaLabel, m_d->ui->soilDownDeltaLEdit, m_d->ui->soilDownDeltaUMLabel, m_d->ui->soilDownDeltaCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->soilUpI, m_d->ui->soilUpILabel, m_d->ui->soilUpILEdit, m_d->ui->soilUpIUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->soilUpZ, m_d->ui->soilUpZLabel, m_d->ui->soilUpZLEdit, m_d->ui->soilUpZUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->soilUpDelta, m_d->ui->soilUpDeltaLabel, m_d->ui->soilUpDeltaLEdit, m_d->ui->soilUpDeltaUMLabel, m_d->ui->soilUpDeltaCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->baseAlpha, m_d->ui->baseAlphaLabel, m_d->ui->baseAlphaLEdit, m_d->ui->baseAlphaUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->toeGammaW, m_d->ui->toeGammaWLabel, m_d->ui->toeGammaWLEdit, m_d->ui->toeGammaWUMLabel, m_d->ui->toeGammaWCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->toeHLeft, m_d->ui->toeHLeftLabel, m_d->ui->toeHLeftLEdit, m_d->ui->toeHLeftUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->toeHRight, m_d->ui->toeHRightLabel, m_d->ui->toeHRightLEdit, m_d->ui->toeHRightUMLabel, m_d->ui->toeHRightCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->toeB, m_d->ui->toeBLabel, m_d->ui->toeBLEdit, m_d->ui->toeBUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->toeD, m_d->ui->toeDLabel, m_d->ui->toeDLEdit, m_d->ui->toeDUMLabel, m_d->ui->toeDCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->heelGammaW, m_d->ui->heelGammaWLabel, m_d->ui->heelGammaWLEdit, m_d->ui->heelGammaWUMLabel, m_d->ui->heelGammaWCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->heelHLeft, m_d->ui->heelHLeftLabel, m_d->ui->heelHLeftLEdit, m_d->ui->heelHLeftUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->heelHRight, m_d->ui->heelHRightLabel, m_d->ui->heelHRightLEdit, m_d->ui->heelHRightUMLabel, m_d->ui->heelHRightCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->heelB, m_d->ui->heelBLabel, m_d->ui->heelBLEdit, m_d->ui->heelBUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->heelD, m_d->ui->heelDLabel, m_d->ui->heelDLEdit, m_d->ui->heelDUMLabel, m_d->ui->heelDCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->keyGammaW, m_d->ui->keyGammaWLabel, m_d->ui->keyGammaWLEdit, m_d->ui->keyGammaWUMLabel, m_d->ui->keyGammaWCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->keyY, m_d->ui->keyYLabel, m_d->ui->keyYLEdit, m_d->ui->keyYUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->keyH, m_d->ui->keyHLabel, m_d->ui->keyHLEdit, m_d->ui->keyHUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->keyB, m_d->ui->keyBLabel, m_d->ui->keyBLEdit, m_d->ui->keyBUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->keyD, m_d->ui->keyDLabel, m_d->ui->keyDLEdit, m_d->ui->keyDUMLabel, m_d->ui->keyDCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->stemGammaW, m_d->ui->stemGammaWLabel, m_d->ui->stemGammaWLEdit, m_d->ui->stemGammaWUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->stemBeta, m_d->ui->stemBetaLabel, m_d->ui->stemBetaLEdit, m_d->ui->stemBetaUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->stemTBottom, m_d->ui->stemTBottomLabel, m_d->ui->stemTBottomLEdit, m_d->ui->stemTBottomUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->stemTTop, m_d->ui->stemTTopLabel, m_d->ui->stemTTopLEdit, m_d->ui->stemTTopUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->stemH, m_d->ui->stemHLabel, m_d->ui->stemHLEdit, m_d->ui->stemHUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->stemD, m_d->ui->stemDLabel, m_d->ui->stemDLEdit, m_d->ui->stemDUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->counterfortGammaW, m_d->ui->counterfortGammaWLabel, m_d->ui->counterfortGammaWLEdit, m_d->ui->counterfortGammaWUMLabel, m_d->ui->counterfortGammaWCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->counterfortBBottom, m_d->ui->counterfortBBottomLabel, m_d->ui->counterfortBBottomLEdit, m_d->ui->counterfortBBottomUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->counterfortBTop, m_d->ui->counterfortBTopLabel, m_d->ui->counterfortBTopLEdit, m_d->ui->counterfortBTopUMLabel );
            VarPlusGUI::connectVar( m_d->retainingWall->counterfortH, m_d->ui->counterfortHLabel, m_d->ui->counterfortHLEdit, m_d->ui->counterfortHUMLabel, m_d->ui->counterfortHCheckBox );
            VarPlusGUI::connectVar( m_d->retainingWall->counterfortT, m_d->ui->counterfortTLabel, m_d->ui->counterfortTLEdit, m_d->ui->counterfortTUMLabel );

            connect( m_d->retainingWall, SIGNAL(soilDownChanged(Soil*, Soil*)), this, SLOT(connectSoilDown(Soil*, Soil*)));
            if( m_d->retainingWall->soilDown()  ){
                m_d->ui->soilDownNameComboBox->setCurrentIndex( m_d->ui->soilDownNameComboBox->findData( m_d->retainingWall->soilDown()->id->valueNormal() ) );
                connect( m_d->retainingWall->soilDown()->soilEQCategory, SIGNAL(valueChanged(QString)), m_d, SLOT(setEQSS()));
                connect( m_d->retainingWall->soilDown()->soilEQCategory, SIGNAL(valueChanged(QString)), m_d, SLOT(setEQbetaM()));
            }
            if( m_d->retainingWall->soilUp()  ){
                m_d->ui->soilUpNameComboBox->setCurrentIndex( m_d->ui->soilUpNameComboBox->findData( m_d->retainingWall->soilUp()->id->valueNormal() ) );
            }
        }
    }
}

void RetainingWallGUI::connectSoilDown(Soil * oldSoil, Soil * newSoil){
    if( oldSoil ){
        disconnect( oldSoil->soilEQCategory, SIGNAL(valueChanged(QString)), m_d, SLOT(setEQSS()));
        disconnect( oldSoil->soilEQCategory, SIGNAL(valueChanged(QString)), m_d, SLOT(setEQbetaM()));
    }
    if( newSoil ){
        connect( newSoil->soilEQCategory, SIGNAL(valueChanged(QString)), m_d, SLOT(setEQSS()));
        connect( newSoil->soilEQCategory, SIGNAL(valueChanged(QString)), m_d, SLOT(setEQbetaM()));
    }
}

void RetainingWallGUI::populateSoilComboBox() {
    m_d->ui->soilDownNameComboBox->clear();
    m_d->ui->soilUpNameComboBox->clear();
    for( int i=0; i < m_d->soilModel->soilCount(); i++){
        m_d->ui->soilDownNameComboBox->addItem( m_d->soilModel->soil(i)->name->valueStr(), QVariant(m_d->soilModel->soil(i)->id->valueNormal() ));
        m_d->ui->soilUpNameComboBox->addItem( m_d->soilModel->soil(i)->name->valueStr(), QVariant(m_d->soilModel->soil(i)->id->valueNormal() ));
    }
    if( m_d->retainingWall ){
        if( m_d->retainingWall->soilDown()  ){
            m_d->ui->soilDownNameComboBox->setCurrentIndex( m_d->ui->soilDownNameComboBox->findData( m_d->retainingWall->soilDown()->id->valueNormal() ) );
        }
        if( m_d->retainingWall->soilUp()  ){
            m_d->ui->soilUpNameComboBox->setCurrentIndex( m_d->ui->soilUpNameComboBox->findData( m_d->retainingWall->soilUp()->id->valueNormal() ) );
        }
    }
}

void RetainingWallGUI::showEvent ( QShowEvent * ){
    disconnect( m_d->ui->soilDownNameComboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(setSoilDown(int)));
    disconnect( m_d->ui->soilUpNameComboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(setSoilUp(int)));
    populateSoilComboBox();
    connect( m_d->ui->soilDownNameComboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(setSoilDown(int)));
    connect( m_d->ui->soilUpNameComboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(setSoilUp(int)));
}

void RetainingWallGUI::setSoilDown(int index ) {
    if( m_d->retainingWall ){
        m_d->retainingWall->setSoilDown( m_d->soilModel->soil(index));
    }
}

void RetainingWallGUI::setSoilUp(int index ) {
    if( m_d->retainingWall ){
        m_d->retainingWall->setSoilUp( m_d->soilModel->soil(index));
    }
}

void RetainingWallGUI::calculateOverTurning(){
    if( m_d->retainingWall ){
        m_d->retainingWall->calculatePhiPrimeD( m_d->overTurningSoilDownPhiPrimeD, m_d->overTurningSoilUpPhiPrimeD, m_d->gammaTanPhiPrimeM2 );
        m_d->retainingWall->calculateCPrimeD( m_d->overTurningSoilDownCPrimeD, m_d->overTurningSoilUpCPrimeD, m_d->gammaCPrimeM2 );
        m_d->retainingWall->calculateDeltaD( m_d->overTurningSoilDownDeltaD, m_d->overTurningSoilUpDeltaD, m_d->gammaTanPhiPrimeM2);
        if( m_d->overTurningSoilDownDeltaD->valueNormal() > m_d->overTurningSoilDownPhiPrimeD->valueNormal() ){
            m_d->overTurningSoilDownDeltaD->setValueNormal( m_d->overTurningSoilDownPhiPrimeD->valueNormal() );
        }
        m_d->retainingWall->calculateK( m_d->overTurningSoilUpPhiPrimeD, m_d->overTurningSoilUpDeltaD, m_d->overTurningKa, m_d->overTurningSoilDownPhiPrimeD, m_d->overTurningSoilDownDeltaD, m_d->overTurningKp, m_d->overTurningChiKp, m_d->overTurningKpRed);
        m_d->retainingWall->calculateOverTurning( m_d->overTurningC, m_d->overTurningMEd, m_d->overTurningMRd, m_d->overTurningSoilDownCPrimeD, m_d->overTurningSoilUpCPrimeD, m_d->overTurningKa, m_d->overTurningApplyKp, m_d->overTurningKpRed, m_d->Gk2, m_d->Qk, m_d->gammaG1MinEQU, m_d->gammaG1MaxEQU, m_d->gammaG2MinEQU, m_d->gammaG1MaxEQU, m_d->gammaQMinEQU, m_d->gammaQMaxEQU);
    }
}

void RetainingWallGUI::calculateEQOverTurning(){
    if( m_d->retainingWall ){
        m_d->retainingWall->calculatePhiPrimeD( m_d->EQoverTurningSoilDownPhiPrimeD, m_d->EQoverTurningSoilUpPhiPrimeD, m_d->gammaTanPhiPrimeM2 );
        m_d->retainingWall->calculateCPrimeD( m_d->EQoverTurningSoilDownCPrimeD, m_d->EQoverTurningSoilUpCPrimeD, m_d->gammaCPrimeM2);
        m_d->retainingWall->calculateDeltaD( m_d->EQoverTurningSoilDownDeltaD, m_d->EQoverTurningSoilUpDeltaD, m_d->gammaTanPhiPrimeM2);
        if( m_d->EQoverTurningSoilDownDeltaD->valueNormal() > m_d->EQoverTurningSoilDownPhiPrimeD->valueNormal() ){
            m_d->EQoverTurningSoilDownDeltaD->setValueNormal( m_d->EQoverTurningSoilDownPhiPrimeD->valueNormal() );
        }
        m_d->retainingWall->calculateEQK( m_d->EQtheta, m_d->EQoverTurningSoilUpPhiPrimeD, m_d->EQoverTurningSoilUpDeltaD, m_d->EQoverTurningKa, m_d->EQoverTurningSoilDownPhiPrimeD, m_d->EQoverTurningSoilDownDeltaD, m_d->EQoverTurningKp, m_d->EQoverTurningChiKp, m_d->EQoverTurningKpRed);
        m_d->retainingWall->calculateEQOverTurning( m_d->EQoverTurningC, m_d->EQoverTurningMEd, m_d->EQoverTurningMRd, m_d->EQoverTurningSoilDownCPrimeD, m_d->EQoverTurningSoilUpCPrimeD, m_d->EQoverTurningKa, m_d->EQoverTurningApplyKp, m_d->EQoverTurningKpRed, m_d->EQDirection, m_d->EQkv, m_d->EQkh, m_d->Gk2, m_d->Qk, m_d->psi2Q );
    }
}

void RetainingWallGUI::calculateA1() {
    if( m_d->retainingWall ){
        m_d->A1C1CombGUI->calculateQSCombination();
        m_d->A1C2CombGUI->calculateQSCombination();
    }
}

void RetainingWallGUI::overTurningCopyClip(){
    QList<VarPlus *> v;

    // v << trUtf8("Verifica a ribaltamento");

    // v << trUtf8("Parametri geotecnici");
    v << m_d->gammaTanPhiPrimeM2 << m_d->overTurningSoilDownPhiPrimeD << m_d->overTurningSoilUpPhiPrimeD;
    v << m_d->gammaCPrimeM2 << m_d->overTurningSoilDownCPrimeD << m_d->overTurningSoilUpCPrimeD;
    v << m_d->gammaTanPhiPrimeM2 << m_d->overTurningSoilDownDeltaD << m_d->overTurningSoilUpDeltaD;
    v << m_d->overTurningKa << m_d->overTurningKp << m_d->overTurningChiKp << m_d->overTurningKpRed;

    // v << trUtf8("Verifica a ribaltamento");
    v << m_d->overTurningC->y << m_d->overTurningC->z << m_d->overTurningMEd << m_d->overTurningMRd;

    QClipboard * clip = QApplication::clipboard();
    QMimeData * data = new QMimeData;
    data->setHtml( printData(v) );
    clip->setMimeData( data  );
}

void RetainingWallGUI::EQOverTurningCopyClip(){
    QList<VarPlus *> v;

    // v << trUtf8("Verifica a ribaltamento in condizioni sismiche");

    // v << trUtf8("Parametri geotecnici");
    v << m_d->EQDirection << m_d->EQkv << m_d->EQkh << m_d->EQtheta;
    v << m_d->gammaTanPhiPrimeM2 << m_d->EQoverTurningSoilDownPhiPrimeD << m_d->EQoverTurningSoilDownPhiPrimeD;
    v << m_d->gammaCPrimeM2 << m_d->EQoverTurningSoilDownCPrimeD << m_d->EQoverTurningSoilUpCPrimeD;
    v << m_d->gammaTanPhiPrimeM2 << m_d->EQoverTurningSoilDownDeltaD << m_d->EQoverTurningSoilUpDeltaD;
    v << m_d->EQoverTurningKa << m_d->EQoverTurningKp << m_d->EQoverTurningChiKp << m_d->EQoverTurningKpRed;

    // v << trUtf8("Verifica a ribaltamento");
    v << m_d->EQoverTurningC->y << m_d->EQoverTurningC->z << m_d->EQoverTurningMEd << m_d->EQoverTurningMRd;

    QClipboard * clip = QApplication::clipboard();
    QMimeData * data = new QMimeData;
    data->setHtml( printData(v) );
    clip->setMimeData( data  );
}

QString RetainingWallGUI::printData( QList<VarPlus *> & v ){
    QString text;
    for( int i=0; i < v.size(); i++ ){
        text +=  v.at(i)->richName() + " ";
        if( v.at(i)->unitMeasurePointer() != NULL )
            text += "[" + v.at(i)->unitMeasurePointer()->string(  v.at(i)->unitMeasureValue() ) + "] ";
        text += "= " + v.at(i)->valueStr() + "\n";
    }
    return text;
}
