#include "bondedanchorgui.h"
#include "ui_bondedanchorgui.h"

#include "varplusguiutility.h"
#include "qcomboboxenum.h"
#include "doubleplus.h"
#include "uintplus.h"

#include "bondedanchor.h"

class BondedAnchorGUIPrivate {
public:
    BondedAnchorGUIPrivate( UnitMeasure * ump ):
        ui( new Ui::BondedAnchorGUI ),
        anchor( new BondedAnchor(ump)) {
    }
    ~BondedAnchorGUIPrivate(){
        delete ui;
        delete anchor;
    }
    Ui::BondedAnchorGUI *ui;
    BondedAnchor * anchor;
};

BondedAnchorGUI::BondedAnchorGUI(UnitMeasure * ump, QWidget *parent) :
    QWidget(parent),
    m_d( new BondedAnchorGUIPrivate(ump) ){
    m_d->ui->setupUi(this);
    connectVars();
}

BondedAnchorGUI::~BondedAnchorGUI() {
    delete m_d;
}

void BondedAnchorGUI::connectVars() {
    VarPlusGUI::connectVar( m_d->anchor->lVEd, m_d->ui->lVEdLabel, m_d->ui->lVEdLEdit, m_d->ui->lVEdUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->VEdX, m_d->ui->VEdXLabel, m_d->ui->VEdXLEdit, m_d->ui->VEdXUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->VEdY, m_d->ui->VEdYLabel, m_d->ui->VEdYLEdit, m_d->ui->VEdYUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->TEd, m_d->ui->TEdLabel, m_d->ui->TEdLEdit, m_d->ui->TEdUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->eNEdX, m_d->ui->eNEdXLabel, m_d->ui->eNEdXLEdit, m_d->ui->eNEdXUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->eNEdY, m_d->ui->eNEdYLabel, m_d->ui->eNEdYLEdit, m_d->ui->eNEdYUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->NEd, m_d->ui->NEdLabel, m_d->ui->NEdLEdit, m_d->ui->NEdUMLabel );

    VarPlusGUI::connectVar( m_d->anchor->d, m_d->ui->dLabel, m_d->ui->dLEdit, m_d->ui->dUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->d0, m_d->ui->d0Label, m_d->ui->d0LEdit, m_d->ui->d0UMLabel );
    VarPlusGUI::connectVar( m_d->anchor->As, m_d->ui->AsLabel, m_d->ui->AsLEdit, m_d->ui->AsUMLabel );

    VarPlusGUI::connectVar( m_d->anchor->nX, m_d->ui->nHorLabel, m_d->ui->nHorLEdit, m_d->ui->nHorUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->nY, m_d->ui->nVerLabel, m_d->ui->nVerLEdit, m_d->ui->nVerUMLabel );

    VarPlusGUI::connectVar( m_d->anchor->cXMinus, m_d->ui->cHorLeftLabel, m_d->ui->cHorLeftLEdit, m_d->ui->cHorLeftUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->sX, m_d->ui->sHorLabel, m_d->ui->sHorLEdit, m_d->ui->sHorUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->cXPlus, m_d->ui->cHorRightLabel, m_d->ui->cHorRightLEdit, m_d->ui->cHorRightUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->cYMinus, m_d->ui->cVerDownLabel, m_d->ui->cVerDownLEdit, m_d->ui->cVerDownUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->sY, m_d->ui->sVerLabel, m_d->ui->sVerLEdit, m_d->ui->sVerUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->cYPlus, m_d->ui->cVerUpLabel, m_d->ui->cVerUpLEdit, m_d->ui->cVerUpUMLabel );

    VarPlusGUI::connectVar( m_d->anchor->h, m_d->ui->hLabel, m_d->ui->hLEdit, m_d->ui->hUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->hEff, m_d->ui->hEfLabel, m_d->ui->hEfLEdit, m_d->ui->hEfUMLabel );

    VarPlusGUI::connectEnumVar( m_d->anchor->concreteCrack, m_d->ui->concreteCrackLabel, m_d->ui->concreteCrackCBox );
    VarPlusGUI::connectEnumVar( m_d->anchor->internalReinforcement, m_d->ui->internalReinforcementLabel, m_d->ui->internalReinforcementCBox );
    VarPlusGUI::connectEnumVar( m_d->anchor->edgeReinforcement, m_d->ui->edgeReinforcementLabel, m_d->ui->edgeReinforcementCBox );

    VarPlusGUI::connectVar( m_d->anchor->fyks, m_d->ui->fykLabel, m_d->ui->fykLEdit, m_d->ui->fykUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->fuks, m_d->ui->fukLabel, m_d->ui->fukLEdit, m_d->ui->fukUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->fckCube, m_d->ui->fckCubeLabel, m_d->ui->fckCubeLEdit, m_d->ui->fckCubeUMLabel );

    VarPlusGUI::connectVar( m_d->anchor->sMin, m_d->ui->sMinLabel, m_d->ui->sMinLEdit, m_d->ui->sMinUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->cMin, m_d->ui->cMinLabel, m_d->ui->cMinLEdit, m_d->ui->cMinUMLabel );

    VarPlusGUI::connectVar( m_d->anchor->hEffMin, m_d->ui->hEfMinLabel, m_d->ui->hEfMinLEdit, m_d->ui->hEfMinUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->hEffMax, m_d->ui->hEfMaxLabel, m_d->ui->hEfMaxLEdit, m_d->ui->hEfMaxUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->hMin, m_d->ui->hMinLabel, m_d->ui->hMinLEdit, m_d->ui->hMinUMLabel );

    VarPlusGUI::connectVar( m_d->anchor->tauRkCr, m_d->ui->tauRkCRLabel, m_d->ui->tauRkCRLEdit, m_d->ui->tauRkCRUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->tauRkUCr, m_d->ui->tauRkUCRLabel, m_d->ui->tauRkUCRLEdit, m_d->ui->tauRkUCRUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->tauRkUCr25, m_d->ui->tauRkUCR25Label, m_d->ui->tauRkUCR25LEdit, m_d->ui->tauRkUCR25UMLabel );

    VarPlusGUI::connectVar( m_d->anchor->ratioCCrSpOvEff, m_d->ui->ratioCCrSpOvHEffLabel, m_d->ui->ratioCCrSpOvHEffLEdit, m_d->ui->ratioCCrSpOvHEffUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->ratioSCrSpOvEff, m_d->ui->ratioSCrSpOvHEffLabel, m_d->ui->ratioSCrSpOvHEffLEdit, m_d->ui->ratioSCrSpOvHEffUMLabel );

    VarPlusGUI::connectVar( m_d->anchor->kCp, m_d->ui->kCpLabel, m_d->ui->kCpLEdit, m_d->ui->kCpUMLabel, m_d->ui->kCpCheckBox );

    VarPlusGUI::connectVar( m_d->anchor->NRks, m_d->ui->NRksLabel, m_d->ui->NRksLEdit, m_d->ui->NRksUMLabel, m_d->ui->NRksCBox );
    VarPlusGUI::connectVar( m_d->anchor->gammaMsN, m_d->ui->gammaMsNLabel, m_d->ui->gammaMsNLEdit, m_d->ui->gammaMsNUMLabel, m_d->ui->gammaMsNCBox );
    VarPlusGUI::connectVar( m_d->anchor->NhRds, m_d->ui->NhRdsLabel, m_d->ui->NhRdsLEdit, m_d->ui->NhRdsUMLabel, m_d->ui->NhRdsCBox );

    VarPlusGUI::connectVar( m_d->anchor->sCrNp, m_d->ui->sCrNpLabel, m_d->ui->sCrNpLEdit, m_d->ui->sCrNpUMLabel, m_d->ui->sCrNpCBox );
    VarPlusGUI::connectVar( m_d->anchor->cCrNp, m_d->ui->cCrNpLabel, m_d->ui->cCrNpLEdit, m_d->ui->cCrNpUMLabel, m_d->ui->cCrNpCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiSNp, m_d->ui->psiSNpLabel, m_d->ui->psiSNpLEdit, m_d->ui->psiSNpUMLabel, m_d->ui->psiSNpCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiGNp, m_d->ui->psiGNpLabel, m_d->ui->psiGNpLEdit, m_d->ui->psiGNpUMLabel, m_d->ui->psiGNpCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiEcNpX, m_d->ui->psiEcNpXLabel, m_d->ui->psiEcNpXLEdit, m_d->ui->psiEcNpXUMLabel, m_d->ui->psiEcNpXCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiEcNpY, m_d->ui->psiEcNpYLabel, m_d->ui->psiEcNpYLEdit, m_d->ui->psiEcNpYUMLabel, m_d->ui->psiEcNpYCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiReNp, m_d->ui->psiReNpLabel, m_d->ui->psiReNpLEdit, m_d->ui->psiReNpUMLabel, m_d->ui->psiReNpCBox );
    VarPlusGUI::connectVar( m_d->anchor->N0Rkp, m_d->ui->N0RkpLabel, m_d->ui->N0RkpLEdit, m_d->ui->N0RkpUMLabel, m_d->ui->N0RkpCBox );
    VarPlusGUI::connectVar( m_d->anchor->NRkp, m_d->ui->NRkpLabel, m_d->ui->NRkpLEdit, m_d->ui->NRkpUMLabel, m_d->ui->NRkpCBox );
    VarPlusGUI::connectVar( m_d->anchor->gammaMpN, m_d->ui->gammaMpNLabel, m_d->ui->gammaMpNLEdit, m_d->ui->gammaMpNUMLabel, m_d->ui->gammaMpNCBox );
    VarPlusGUI::connectVar( m_d->anchor->NgRdp, m_d->ui->NgRdpLabel, m_d->ui->NgRdpLEdit, m_d->ui->NgRdpUMLabel, m_d->ui->NgRdpCBox );

    VarPlusGUI::connectVar( m_d->anchor->sCrN, m_d->ui->sCrNLabel, m_d->ui->sCrNLEdit, m_d->ui->sCrNUMLabel, m_d->ui->sCrNCBox );
    VarPlusGUI::connectVar( m_d->anchor->cCrN, m_d->ui->cCrNLabel, m_d->ui->cCrNLEdit, m_d->ui->cCrNUMLabel, m_d->ui->cCrNCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiSN, m_d->ui->psiSNLabel, m_d->ui->psiSNLEdit, m_d->ui->psiSNUMLabel, m_d->ui->psiSNCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiEcNX, m_d->ui->psiEcNXLabel, m_d->ui->psiEcNXLEdit, m_d->ui->psiEcNXUMLabel, m_d->ui->psiEcNXCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiEcNY, m_d->ui->psiEcNYLabel, m_d->ui->psiEcNYLEdit, m_d->ui->psiEcNYUMLabel, m_d->ui->psiEcNYCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiReN, m_d->ui->psiReNLabel, m_d->ui->psiReNLEdit, m_d->ui->psiReNUMLabel, m_d->ui->psiReNCBox );
    VarPlusGUI::connectVar( m_d->anchor->N0Rkc, m_d->ui->N0RkcLabel, m_d->ui->N0RkcLEdit, m_d->ui->N0RkcUMLabel, m_d->ui->N0RkcCBox );
    VarPlusGUI::connectVar( m_d->anchor->NRkc, m_d->ui->NRkcLabel, m_d->ui->NRkcLEdit, m_d->ui->NRkcUMLabel, m_d->ui->NRkcCBox );
    VarPlusGUI::connectVar( m_d->anchor->gammaMcN, m_d->ui->gammaMcNLabel, m_d->ui->gammaMcNLEdit, m_d->ui->gammaMcNUMLabel, m_d->ui->gammaMcNCBox );
    VarPlusGUI::connectVar( m_d->anchor->NgRdc, m_d->ui->NgRdcLabel, m_d->ui->NgRdcLEdit, m_d->ui->NgRdcUMLabel, m_d->ui->NgRdcCBox );

    VarPlusGUI::connectVar( m_d->anchor->sCrSp, m_d->ui->sCrSpLabel, m_d->ui->sCrSpLEdit, m_d->ui->sCrSpUMLabel, m_d->ui->sCrSpCBox );
    VarPlusGUI::connectVar( m_d->anchor->cCrSp, m_d->ui->cCrSpLabel, m_d->ui->cCrSpLEdit, m_d->ui->cCrSpUMLabel, m_d->ui->cCrSpCBox );
    VarPlusGUI::connectVar( m_d->anchor->psiHSp, m_d->ui->psiHSpLabel, m_d->ui->psiHSpLEdit, m_d->ui->psiHSpUMLabel, m_d->ui->psiHSpCBox );
    VarPlusGUI::connectVar( m_d->anchor->NRkSp, m_d->ui->NRkSpLabel, m_d->ui->NRkSpLEdit, m_d->ui->NRkSpUMLabel, m_d->ui->NRkSpCBox );
    VarPlusGUI::connectVar( m_d->anchor->gammaMSpN, m_d->ui->gammaMspNLabel, m_d->ui->gammaMspNLEdit, m_d->ui->gammaMspNUMLabel, m_d->ui->gammaMspNCBox );
    VarPlusGUI::connectVar( m_d->anchor->NgRdSp, m_d->ui->NgRdSpLabel, m_d->ui->NgRdSpLEdit, m_d->ui->NgRdSpUMLabel, m_d->ui->NgRdSpCBox );

    VarPlusGUI::connectVar( m_d->anchor->alphaM, m_d->ui->alphaMLabel, m_d->ui->alphaMLEdit, m_d->ui->alphaMUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->M0Rks, m_d->ui->M0RksLabel, m_d->ui->M0RksLEdit, m_d->ui->M0RksUMLabel );
    VarPlusGUI::connectVar( m_d->anchor->MRks, m_d->ui->MRksLabel, m_d->ui->MRksLEdit, m_d->ui->MRksUMLabel, m_d->ui->MRksCBox );
    VarPlusGUI::connectVar( m_d->anchor->VRks, m_d->ui->VRksLabel, m_d->ui->VRksLEdit, m_d->ui->VRksUMLabel, m_d->ui->VRksCBox );
    VarPlusGUI::connectVar( m_d->anchor->gammaMsV, m_d->ui->gammaMsVLabel, m_d->ui->gammaMsVLEdit, m_d->ui->gammaMsVUMLabel, m_d->ui->gammaMsVCBox );
    VarPlusGUI::connectVar( m_d->anchor->VhRds, m_d->ui->VhRdsLabel, m_d->ui->VhRdsLEdit, m_d->ui->VhRdsUMLabel, m_d->ui->VhRdsCBox );

    VarPlusGUI::connectVar( m_d->anchor->VRkCp, m_d->ui->VRkCpLabel, m_d->ui->VRkCpLEdit, m_d->ui->VRkCpUMLabel, m_d->ui->VRkCpCBox );
    VarPlusGUI::connectVar( m_d->anchor->gammaMcV, m_d->ui->gammaMpVLabel, m_d->ui->gammaMpVLEdit, m_d->ui->gammaMpVUMLabel, m_d->ui->gammaMpVCBox );
    VarPlusGUI::connectVar( m_d->anchor->VgRdCp, m_d->ui->VgRdCpLabel, m_d->ui->VgRdCpLEdit, m_d->ui->VgRdCpUMLabel, m_d->ui->VgRdCpCBox );

    VarPlusGUI::connectVar( m_d->anchor->k1C, m_d->ui->k1CLabel, m_d->ui->k1CLEdit, m_d->ui->k1CUMLabel, m_d->ui->k1CCBox );
    VarPlusGUI::connectVar( m_d->anchor->gammaMcV, m_d->ui->gammaMcVLabel, m_d->ui->gammaMcVLEdit, m_d->ui->gammaMcVUMLabel, m_d->ui->gammaMcVCBox );

    VarPlusGUI::connectVar( m_d->anchor->A0CVXPlus, m_d->ui->A0CVXPlusLabel, m_d->ui->A0CVXPlusLEdit, m_d->ui->A0CVXPlusUMLabel, m_d->ui->A0CVXPlusCBox );
    VarPlusGUI::connectVar( m_d->anchor->ACVXPlus, m_d->ui->ACVXPlusLabel, m_d->ui->ACVXPlusLEdit, m_d->ui->ACVXPlusUMLabel, m_d->ui->ACVXPlusCBox );
    VarPlusGUI::connectVar( m_d->anchor->V0RkCXPlus, m_d->ui->V0RkCXPlusLabel, m_d->ui->V0RkCXPlusLEdit, m_d->ui->V0RkCXPlusUMLabel, m_d->ui->V0RkCXPlusCBox );
    VarPlusGUI::connectVar( m_d->anchor->VRkCXPlus, m_d->ui->VRkCXPlusLabel, m_d->ui->VRkCXPlusLEdit, m_d->ui->VRkCXPlusUMLabel, m_d->ui->VRkCXPlusCBox );
    VarPlusGUI::connectVar( m_d->anchor->VgRdCXPlus, m_d->ui->VgRdCXPlusLabel, m_d->ui->VgRdCXPlusLEdit, m_d->ui->VgRdCXPlusUMLabel, m_d->ui->VgRdCXPlusCBox );

    VarPlusGUI::connectVar( m_d->anchor->A0CVXMinus, m_d->ui->A0CVXMinusLabel, m_d->ui->A0CVXMinusLEdit, m_d->ui->A0CVXMinusUMLabel, m_d->ui->A0CVXMinusCBox );
    VarPlusGUI::connectVar( m_d->anchor->ACVXMinus, m_d->ui->ACVXMinusLabel, m_d->ui->ACVXMinusLEdit, m_d->ui->ACVXMinusUMLabel, m_d->ui->ACVXMinusCBox );
    VarPlusGUI::connectVar( m_d->anchor->V0RkCXMinus, m_d->ui->V0RkCXMinusLabel, m_d->ui->V0RkCXMinusLEdit, m_d->ui->V0RkCXMinusUMLabel, m_d->ui->V0RkCXMinusCBox );
    VarPlusGUI::connectVar( m_d->anchor->VRkCXMinus, m_d->ui->VRkCXMinusLabel, m_d->ui->VRkCXMinusLEdit, m_d->ui->VRkCXMinusUMLabel, m_d->ui->VRkCXMinusCBox );
    VarPlusGUI::connectVar( m_d->anchor->VgRdCXMinus, m_d->ui->VgRdCXMinusLabel, m_d->ui->VgRdCXMinusLEdit, m_d->ui->VgRdCXMinusUMLabel, m_d->ui->VgRdCXMinusCBox );

    VarPlusGUI::connectVar( m_d->anchor->A0CVYPlus, m_d->ui->A0CVYPlusLabel, m_d->ui->A0CVYPlusLEdit, m_d->ui->A0CVYPlusUMLabel, m_d->ui->A0CVYPlusCBox );
    VarPlusGUI::connectVar( m_d->anchor->ACVYPlus, m_d->ui->ACVYPlusLabel, m_d->ui->ACVYPlusLEdit, m_d->ui->ACVYPlusUMLabel, m_d->ui->ACVYPlusCBox );
    VarPlusGUI::connectVar( m_d->anchor->V0RkCYPlus, m_d->ui->V0RkCYPlusLabel, m_d->ui->V0RkCYPlusLEdit, m_d->ui->V0RkCYPlusUMLabel, m_d->ui->V0RkCYPlusCBox );
    VarPlusGUI::connectVar( m_d->anchor->VRkCYPlus, m_d->ui->VRkCYPlusLabel, m_d->ui->VRkCYPlusLEdit, m_d->ui->VRkCYPlusUMLabel, m_d->ui->VRkCYPlusCBox );
    VarPlusGUI::connectVar( m_d->anchor->VgRdCYPlus, m_d->ui->VgRdCYPlusLabel, m_d->ui->VgRdCYPlusLEdit, m_d->ui->VgRdCYPlusUMLabel, m_d->ui->VgRdCYPlusCBox );

    VarPlusGUI::connectVar( m_d->anchor->A0CVYMinus, m_d->ui->A0CVYMinusLabel, m_d->ui->A0CVYMinusLEdit, m_d->ui->A0CVYMinusUMLabel, m_d->ui->A0CVYMinusCBox );
    VarPlusGUI::connectVar( m_d->anchor->ACVYMinus, m_d->ui->ACVYMinusLabel, m_d->ui->ACVYMinusLEdit, m_d->ui->ACVYMinusUMLabel, m_d->ui->ACVYMinusCBox );
    VarPlusGUI::connectVar( m_d->anchor->V0RkCYMinus, m_d->ui->V0RkCYMinusLabel, m_d->ui->V0RkCYMinusLEdit, m_d->ui->V0RkCYMinusUMLabel, m_d->ui->V0RkCYMinusCBox );
    VarPlusGUI::connectVar( m_d->anchor->VRkCYMinus, m_d->ui->VRkCYMinusLabel, m_d->ui->VRkCYMinusLEdit, m_d->ui->VRkCYMinusUMLabel, m_d->ui->VRkCYMinusCBox );
    VarPlusGUI::connectVar( m_d->anchor->VgRdCYMinus, m_d->ui->VgRdCYMinusLabel, m_d->ui->VgRdCYMinusLEdit, m_d->ui->VgRdCYMinusUMLabel, m_d->ui->VgRdCYMinusCBox );
}
