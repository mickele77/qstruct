#include "retainingwallguiprivate.h"

#include "retainingwallqscombinationgui.h"
#include "retainingwalleqqscombinationgui.h"

#include "retainingwall.h"
#include "soil.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "boolplus.h"

#include "ui_retainingwallgui.h"


RetainingWallGUIPrivate::RetainingWallGUIPrivate( UnitMeasure * ump, SoilModel * sm, RetainingWallGUI * parent ):
    QObject(),
    VarPlusContainer(ump, "", ""),
    retainingWall( NULL ),
    soilModel( sm ),
    ui(new Ui::RetainingWallGUI){
    initVar();

    A1C1CombGUI = new RetainingWallQSCombinationGUI( ump,
                                                     QObject::trUtf8("Approccio 1 - Combinazione 1") ,
                                                     Gk2, Qk,
                                                     gammaTanPhiPrimeM1, gammaCPrimeM1,
                                                     gammaG1MinA1, gammaG1MaxA1,
                                                     gammaG2MinA1, gammaG2MaxA1,
                                                     gammaQMinA1, gammaQMaxA1,
                                                     gammaSlidingR1, gammaQLimR1,
                                                     parent );

    A1C2CombGUI = new RetainingWallQSCombinationGUI( ump,
                                                     QObject::trUtf8("Approccio 1 - Combinazione 2"),
                                                     Gk2, Qk,
                                                     gammaTanPhiPrimeM2, gammaCPrimeM2,
                                                     gammaG1MinA2, gammaG1MaxA2,
                                                     gammaG2MinA2, gammaG2MaxA2,
                                                     gammaQMinA2, gammaQMaxA2,
                                                     gammaSlidingR2, gammaQLimR2,
                                                     parent );

    A2CombGUI = new RetainingWallQSCombinationGUI( ump,
                                                   QObject::trUtf8("Approccio 2"),
                                                   Gk2, Qk,
                                                   gammaTanPhiPrimeM1, gammaCPrimeM1,
                                                   gammaG1MinA1, gammaG1MaxA1,
                                                   gammaG2MinA1, gammaG2MaxA1,
                                                   gammaQMinA1, gammaQMaxA1,
                                                   gammaSlidingR3, gammaQLimR3,
                                                   parent );
    EQA1C2CombGUI = new RetainingWallEQQSCombinationGUI( ump,
                                                         QObject::trUtf8("Condizioni sismiche - Approccio 1 Combinazione 2"),
                                                         EQDirection,
                                                         EQkv,
                                                         EQkh,
                                                         EQtheta,
                                                         Gk2, Qk, psi2Q,
                                                         gammaTanPhiPrimeM2, gammaCPrimeM2,
                                                         gammaSlidingR2, gammaQLimR2,
                                                         parent );
    EQA2CombGUI = new RetainingWallEQQSCombinationGUI( ump,
                                                       QObject::trUtf8("Condizioni sismiche - Approccio 2"),
                                                       EQDirection,
                                                       EQkv,
                                                       EQkh,
                                                       EQtheta,
                                                       Gk2, Qk, psi2Q,
                                                       gammaTanPhiPrimeM1, gammaCPrimeM1,
                                                       gammaSlidingR3, gammaQLimR3,
                                                       parent );
}

RetainingWallGUIPrivate::~RetainingWallGUIPrivate(){
    delete ui;
}

void RetainingWallGUIPrivate::initVar() {
    gammaQLimR1 = new DoublePlus( 1.0, "gammaQLimR1", m_unitMeasure, UnitMeasure::noDimension);
    gammaQLimR1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">R,1,QLim</span>"));
    addVarToContainer( gammaQLimR1 );

    gammaSlidingR1 = new DoublePlus( 1.0, "gammaSlidingR1", m_unitMeasure, UnitMeasure::noDimension);
    gammaSlidingR1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">R,1,scorr</span>"));
    addVarToContainer( gammaSlidingR1 );

    gammaRdDownR1 = new DoublePlus( 1.0, "gammaKpR1", m_unitMeasure, UnitMeasure::noDimension);
    gammaRdDownR1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">R,1,Kp</span>"));
    addVarToContainer( gammaRdDownR1 );

    gammaQLimR2 = new DoublePlus( 1.0, "gammaQLimR2", m_unitMeasure, UnitMeasure::noDimension);
    gammaQLimR2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">R,2,QLim</span>"));
    addVarToContainer( gammaQLimR2 );

    gammaSlidingR2 = new DoublePlus( 1.0, "gammaSlidingR2", m_unitMeasure, UnitMeasure::noDimension);
    gammaSlidingR2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">R,2,scorr</span>"));
    addVarToContainer( gammaSlidingR2 );

    gammaRdDownR2 = new DoublePlus( 1.0, "gammaKpR2", m_unitMeasure, UnitMeasure::noDimension);
    gammaRdDownR2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">R,2,Kp</span>"));
    addVarToContainer( gammaRdDownR2 );

    gammaQLimR3 = new DoublePlus( 1.4, "gammaQLimR3", m_unitMeasure, UnitMeasure::noDimension);
    gammaQLimR3->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">R,3,QLim</span>"));
    addVarToContainer( gammaQLimR3 );

    gammaSlidingR3 = new DoublePlus( 1.1, "gammaSlidingR3", m_unitMeasure, UnitMeasure::noDimension);
    gammaSlidingR3->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">R,3,scorr</span>"));
    addVarToContainer( gammaSlidingR3 );

    gammaRdDownR3 = new DoublePlus( 1.4, "gammaKpR3", m_unitMeasure, UnitMeasure::noDimension);
    gammaRdDownR3->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">R,3,Kp</span>"));
    addVarToContainer( gammaRdDownR3 );

    gammaTanPhiPrimeM1  = new DoublePlus( 1.0, "gammaTanPhiPrimeM1", m_unitMeasure, UnitMeasure::noDimension);
    gammaTanPhiPrimeM1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">φ'</span>"));
    addVarToContainer( gammaTanPhiPrimeM1 );

    gammaCPrimeM1  = new DoublePlus( 1.0, "gammaCPrimeM1", m_unitMeasure, UnitMeasure::noDimension);
    gammaCPrimeM1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">c'</span>"));
    addVarToContainer( gammaCPrimeM1 );

    gammaCUM1  = new DoublePlus( 1.0, "gammaCUM1", m_unitMeasure, UnitMeasure::noDimension);
    gammaCUM1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">cu</span>"));
    addVarToContainer( gammaCUM1 );

    gammaGammaWM1  = new DoublePlus( 1.0, "gammaGammaWM1", m_unitMeasure, UnitMeasure::noDimension);
    gammaGammaWM1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">γ</span>"));
    addVarToContainer( gammaGammaWM1 );

    gammaTanPhiPrimeM2  = new DoublePlus( 1.25, "gammaTanPhiPrimeM2", m_unitMeasure, UnitMeasure::noDimension);
    gammaTanPhiPrimeM2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">φ'</span>"));
    addVarToContainer( gammaTanPhiPrimeM2 );

    gammaCPrimeM2  = new DoublePlus( 1.25, "gammaCPrimeM2", m_unitMeasure, UnitMeasure::noDimension);
    gammaCPrimeM2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">c'</span>"));
    addVarToContainer( gammaCPrimeM2 );

    gammaCUM2  = new DoublePlus( 1.4, "gammaCUM2", m_unitMeasure, UnitMeasure::noDimension);
    gammaCUM2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">cu</span>"));
    addVarToContainer( gammaCUM2 );

    gammaGammaWM2  = new DoublePlus( 1.0, "gammaGammaWM2", m_unitMeasure, UnitMeasure::noDimension);
    gammaGammaWM2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">γ</span>"));
    addVarToContainer( gammaGammaWM2 );

    gammaG1MinEQU  = new DoublePlus( 0.9, "gammaG1MinEQU", m_unitMeasure, UnitMeasure::noDimension);
    gammaG1MinEQU->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G1,min</span>"));
    addVarToContainer( gammaG1MinEQU );

    gammaG1MaxEQU  = new DoublePlus( 1.1, "gammaG1MaxEQU", m_unitMeasure, UnitMeasure::noDimension);
    gammaG1MaxEQU->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G1,max</span>"));
    addVarToContainer( gammaG1MaxEQU );

    gammaG2MinEQU  = new DoublePlus( 0.0, "gammaG2MinEQU", m_unitMeasure, UnitMeasure::noDimension);
    gammaG2MinEQU->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G2,min</span>"));
    addVarToContainer( gammaG2MinEQU );

    gammaG2MaxEQU  = new DoublePlus( 1.5, "gammaG2MaxEQU", m_unitMeasure, UnitMeasure::noDimension);
    gammaG2MaxEQU->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G2,max</span>"));
    addVarToContainer( gammaG2MaxEQU );

    gammaQMinEQU  = new DoublePlus( 0.0, "gammaQMinEQU", m_unitMeasure, UnitMeasure::noDimension);
    gammaQMinEQU->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">Q,min</span>"));
    addVarToContainer( gammaQMinEQU );

    gammaQMaxEQU  = new DoublePlus( 1.5, "gammaQMaxEQU", m_unitMeasure, UnitMeasure::noDimension);
    gammaQMaxEQU->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">Q,max</span>"));
    addVarToContainer( gammaQMaxEQU );

    gammaG1MinA1  = new DoublePlus( 1.0, "gammaG1MinA1", m_unitMeasure, UnitMeasure::noDimension);
    gammaG1MinA1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G1,min</span>"));
    addVarToContainer( gammaG1MinA1 );

    gammaG1MaxA1  = new DoublePlus( 1.3, "gammaG1MaxA1", m_unitMeasure, UnitMeasure::noDimension);
    gammaG1MaxA1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G1,max</span>"));
    addVarToContainer( gammaG1MaxA1 );

    gammaG2MinA1  = new DoublePlus( 0.0, "gammaG2MinA1", m_unitMeasure, UnitMeasure::noDimension);
    gammaG2MinA1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G2,min</span>"));
    addVarToContainer( gammaG2MinA1 );

    gammaG2MaxA1  = new DoublePlus( 1.5, "gammaG2MaxA1", m_unitMeasure, UnitMeasure::noDimension);
    gammaG2MaxA1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G2,max</span>"));
    addVarToContainer( gammaG2MaxA1 );

    gammaQMinA1  = new DoublePlus( 0.0, "gammaQMinA1", m_unitMeasure, UnitMeasure::noDimension);
    gammaQMinA1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">Q,min</span>"));
    addVarToContainer( gammaQMinA1 );

    gammaQMaxA1  = new DoublePlus( 1.5, "gammaQMaxA1", m_unitMeasure, UnitMeasure::noDimension);
    gammaQMaxA1->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">Q,max</span>"));
    addVarToContainer( gammaQMaxA1 );

    gammaG1MinA2  = new DoublePlus( 1.0, "gammaG1MinA2", m_unitMeasure, UnitMeasure::noDimension);
    gammaG1MinA2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G1,min</span>"));
    addVarToContainer( gammaG1MinA2 );

    gammaG1MaxA2  = new DoublePlus( 1.0, "gammaG1MaxA2", m_unitMeasure, UnitMeasure::noDimension);
    gammaG1MaxA2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G1,max</span>"));
    addVarToContainer( gammaG1MaxA2 );

    gammaG2MinA2  = new DoublePlus( 0.0, "gammaG2MinA2", m_unitMeasure, UnitMeasure::noDimension);
    gammaG2MinA2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G2,min</span>"));
    addVarToContainer( gammaG2MinA2 );

    gammaG2MaxA2  = new DoublePlus( 1.3, "gammaG2MaxA2", m_unitMeasure, UnitMeasure::noDimension);
    gammaG2MaxA2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">G2,max</span>"));
    addVarToContainer( gammaG2MaxA2 );

    gammaQMinA2  = new DoublePlus( 0.0, "gammaQMinA2", m_unitMeasure, UnitMeasure::noDimension);
    gammaQMinA2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">Q,min</span>"));
    addVarToContainer( gammaQMinA2 );

    gammaQMaxA2  = new DoublePlus( 1.3, "gammaQMaxA2", m_unitMeasure, UnitMeasure::noDimension);
    gammaQMaxA2->setRichName( QObject::trUtf8("γ<span style=\" vertical-align:sub;\">Q,max</span>"));
    addVarToContainer( gammaQMaxA2 );

    Gk2 = new DoublePlus( 0.0, "Gk2", m_unitMeasure, UnitMeasure::loadFArea );
    Gk2->setRichName( QObject::trUtf8("G<span style=\" vertical-align:sub;\">k,2</span>"));
    Gk2->setToolTip( QObject::trUtf8("Carichi permanenti non strutturali agenti verso monte"));
    addVarToContainer( Gk2 );

    Qk = new DoublePlus( 0.0, "Qk", m_unitMeasure, UnitMeasure::loadFArea );
    Qk->setRichName( QObject::trUtf8("Q<span style=\" vertical-align:sub;\">k</span>"));
    Qk->setToolTip( QObject::trUtf8("Carichi accidentali agenti verso monte"));
    addVarToContainer( Qk );

    psi2Q = new DoublePlus( 0.3, "psi2Q", m_unitMeasure, UnitMeasure::noDimension );
    psi2Q->setRichName( QObject::trUtf8("ψ<span style=\" vertical-align:sub;\">2,Q</span>"));
    psi2Q->setToolTip( QObject::trUtf8("Coefficiente di contemporaneità dei carichi accidentali"));
    addVarToContainer( psi2Q );

    overTurningSoilUpPhiPrimeD = new DoublePlus(0.0, "overTurningSoilUpPhiPrimeD", m_unitMeasure, UnitMeasure::angle );
    overTurningSoilUpPhiPrimeD->setRichName( QObject::trUtf8("φ'<span style=\" vertical-align:sub;\">monte,d</span>"));
    overTurningSoilUpPhiPrimeD->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Angolo di attrito di calcolo del terreno a monte"));
    overTurningSoilUpPhiPrimeD->setReadOnly( true );
    addVarToContainer( overTurningSoilUpPhiPrimeD );

    overTurningSoilDownPhiPrimeD = new DoublePlus(0.0, "overTurningSoilDownPhiPrimeD", m_unitMeasure, UnitMeasure::angle );
    overTurningSoilDownPhiPrimeD->setRichName( QObject::trUtf8("φ'<span style=\" vertical-align:sub;\">valle,d</span>"));
    overTurningSoilDownPhiPrimeD->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Angolo di attrito di calcolo del terreno a valle"));
    overTurningSoilDownPhiPrimeD->setReadOnly( true );
    addVarToContainer( overTurningSoilDownPhiPrimeD );

    overTurningSoilUpCPrimeD = new DoublePlus(0.0, "overTurningSoilUpCPrimeD", m_unitMeasure, UnitMeasure::tension );
    overTurningSoilUpCPrimeD->setRichName( QObject::trUtf8("c'<span style=\" vertical-align:sub;\">monte,d</span>"));
    overTurningSoilUpCPrimeD->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Coesione di calcolo del terreno a monte"));
    overTurningSoilUpCPrimeD->setReadOnly( true );
    addVarToContainer( overTurningSoilUpCPrimeD );

    overTurningSoilDownCPrimeD = new DoublePlus(0.0, "overTurningSoilDownCPrimeD", m_unitMeasure, UnitMeasure::tension );
    overTurningSoilDownCPrimeD->setRichName( QObject::trUtf8("c'<span style=\" vertical-align:sub;\">valle,d</span>"));
    overTurningSoilDownCPrimeD->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Coesione di calcolo del terreno a valle"));
    overTurningSoilDownCPrimeD->setReadOnly( true );
    addVarToContainer( overTurningSoilDownCPrimeD );

    overTurningSoilUpDeltaD = new DoublePlus(0.0, "overTurningSoilUpDeltaD", m_unitMeasure, UnitMeasure::angle );
    overTurningSoilUpDeltaD->setRichName( QObject::trUtf8("φ<span style=\" vertical-align:sub;\">monte,d</span>"));
    overTurningSoilUpDeltaD->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Angolo di attrito di calcolo del terreno a monte"));
    overTurningSoilUpDeltaD->setReadOnly( false );
    addVarToContainer( overTurningSoilUpDeltaD );

    overTurningSoilDownDeltaD = new DoublePlus(0.0, "overTurningSoilDownDeltaD", m_unitMeasure, UnitMeasure::angle );
    overTurningSoilDownDeltaD->setRichName( QObject::trUtf8("φ<span style=\" vertical-align:sub;\">valle,d</span>"));
    overTurningSoilDownDeltaD->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Angolo di attrito di calcolo del terreno a valle"));
    overTurningSoilDownDeltaD->setReadOnly( true );
    addVarToContainer( overTurningSoilDownDeltaD );

    overTurningKa = new DoublePlus(0.0, "overTurningKa", m_unitMeasure, UnitMeasure::noDimension );
    overTurningKa->setRichName( QObject::trUtf8("K<span style=\" vertical-align:sub;\">a</span>"));
    overTurningKa->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Coefficiente di spinta attiva"));
    overTurningKa->setReadOnly( true );
    addVarToContainer( overTurningKa );

    overTurningApplyKp = new BoolPlus( false, "overTurningApplyKp" );
    overTurningApplyKp->setToolTip( trUtf8("Verifica a ribaltamento - Se si tiene conto o meno del contributo della resistenza passiva nelle verifiche"));
    overTurningApplyKp->setRichName( trUtf8("Resistenza passiva contro fondazione"));
    overTurningApplyKp->setReadOnly( false );
    addVarToContainer( overTurningApplyKp );

    overTurningKp = new DoublePlus(0.0, "overTurningKp", m_unitMeasure, UnitMeasure::noDimension );
    overTurningKp->setRichName( QObject::trUtf8("K<span style=\" vertical-align:sub;\">p</span>"));
    overTurningKp->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Coefficiente di spinta passiva"));
    overTurningKp->setReadOnly( true );
    addVarToContainer( overTurningKp );

    overTurningChiKp = new DoublePlus(0.5, "overTurningChiKp", m_unitMeasure, UnitMeasure::noDimension );
    overTurningChiKp->setRichName( QObject::trUtf8("χ<span style=\" vertical-align:sub;\">kp</span>"));
    overTurningChiKp->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Fattore riduttivo del coefficiente di spinta passiva"));
    overTurningChiKp->setReadOnly( true );
    addVarToContainer( overTurningChiKp );

    overTurningKpRed = new DoublePlus(0.0, "overTurningKpRed", m_unitMeasure, UnitMeasure::noDimension );
    overTurningKpRed->setRichName( QObject::trUtf8("K<span style=\" vertical-align:sub;\">p,red</span>"));
    overTurningKpRed->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Coefficiente spinta passiva ridotto"));
    overTurningKpRed->setReadOnly( true );
    addVarToContainer( overTurningKpRed );

    overTurningC = new Point2DPlus("overTurningC", "y", 0.0, "z", 0.0, m_unitMeasure, UnitMeasure::length );
    overTurningC->y->setRichName( QObject::trUtf8("y<span style=\" vertical-align:sub;\">C</span>"));
    overTurningC->y->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Polo per calcolo momenti - y"));
    overTurningC->y->setReadOnly( true );
    overTurningC->z->setRichName( QObject::trUtf8("z<span style=\" vertical-align:sub;\">C</span>"));
    overTurningC->z->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Polo per calcolo momenti - z"));
    overTurningC->z->setReadOnly( true );
    addVarToContainer( overTurningC );

    overTurningMEd = new DoublePlus(0.0, "overTurningMEd", m_unitMeasure, UnitMeasure::loadM, true );
    overTurningMEd->setRichName( QObject::trUtf8("M<span style=\" vertical-align:sub;\">Ed</span>"));
    overTurningMEd->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Momento ribaltante"));
    overTurningMEd->setReadOnly( true );
    addVarToContainer( overTurningMEd );

    overTurningMRd = new DoublePlus(0.0, "overTurningMRd", m_unitMeasure, UnitMeasure::loadM, true );
    overTurningMRd->setRichName( QObject::trUtf8("M<span style=\" vertical-align:sub;\">Rd</span>"));
    overTurningMRd->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Momento resistente"));
    overTurningMRd->setReadOnly( true );
    addVarToContainer( overTurningMRd );

    EQag = new DoublePlus (0.0, "EQag", m_unitMeasure, UnitMeasure::noDimension );
    EQag->setRichName( QObject::trUtf8("a<span style=\" vertical-align:sub;\">g</span>/g"));
    EQag->setToolTip( QObject::trUtf8("Accelerazione orizzontale adimensionale massima attesa su sito di riferimento rigido"));
    addVarToContainer( EQag );

    EQF0 = new DoublePlus (0.0, "EQF0", m_unitMeasure, UnitMeasure::noDimension );
    EQF0->setRichName( QObject::trUtf8("F<span style=\" vertical-align:sub;\">0</span>"));
    EQF0->setToolTip( QObject::trUtf8("Parametro per la definizione dell'azione sismica"));
    addVarToContainer( EQF0 );

    EQSS = new DoublePlus (1.0, "EQSS", m_unitMeasure, UnitMeasure::noDimension );
    EQSS->setRichName( QObject::trUtf8("S<span style=\" vertical-align:sub;\">S</span>"));
    EQSS->setToolTip( QObject::trUtf8("Coefficiente di amplificazione stratigrafica"));
    EQSS->setReadOnly( true );
    addVarToContainer( EQSS );
    connect( EQag, SIGNAL(valueChanged(QString)), this, SLOT(setEQSS()));
    connect( EQF0, SIGNAL(valueChanged(QString)), this, SLOT(setEQSS()));
    connect( EQSS, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEQSS()));

    EQST = new DoublePlus (1.0, "EQST", m_unitMeasure, UnitMeasure::noDimension );
    EQST->setRichName( QObject::trUtf8("S<span style=\" vertical-align:sub;\">T</span>"));
    EQST->setToolTip( QObject::trUtf8("Coefficiente di amplificazione topografica"));
    addVarToContainer( EQST );

    EQS = new DoublePlus (1.0, "EQS", m_unitMeasure, UnitMeasure::noDimension );
    EQS->setRichName( QObject::trUtf8("S"));
    EQS->setToolTip( QObject::trUtf8("Coefficiente di amplificazione"));
    EQS->setReadOnly( true );
    addVarToContainer( EQS );
    connect( EQSS, SIGNAL(valueChanged(QString)), this, SLOT(setEQS()));
    connect( EQST, SIGNAL(valueChanged(QString)), this, SLOT(setEQS()));
    connect( EQS, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEQS()));

    EQaMax = new DoublePlus (0.0, "EQaMax", m_unitMeasure, UnitMeasure::noDimension );
    EQaMax->setRichName( QObject::trUtf8("a<span style=\" vertical-align:sub;\">max</span>/g"));
    EQaMax->setToolTip( QObject::trUtf8("Accelerazione orizzontale adimensionale massima attesa sul sito"));
    EQaMax->setReadOnly( true );
    addVarToContainer( EQaMax );

    connect( EQag, SIGNAL(valueChanged(QString)), this, SLOT(setEQaMax()));
    connect( EQS, SIGNAL(valueChanged(QString)), this, SLOT(setEQaMax()));
    connect( EQaMax, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEQaMax()));

    EQbetaM = new DoublePlus (0.0, "EQbetaM", m_unitMeasure, UnitMeasure::noDimension );
    EQbetaM->setRichName( QObject::trUtf8("β<span style=\" vertical-align:sub;\">m</span>"));
    EQbetaM->setToolTip( QObject::trUtf8("Coefficiente di correlazione tra l'accelerazione massima e la spinta inerziale orizzontale"));
    EQbetaM->setReadOnly( true );
    addVarToContainer( EQbetaM );
    connect( EQag, SIGNAL(valueChanged(QString)), this, SLOT(setEQbetaM()));
    connect( EQbetaM, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEQbetaM()));

    EQkh = new DoublePlus (0.0, "EQkh", m_unitMeasure, UnitMeasure::noDimension );
    EQkh->setRichName( QObject::trUtf8("k<span style=\" vertical-align:sub;\">h</span>"));
    EQkh->setToolTip( QObject::trUtf8("Coefficiente di spinta inerziale orizzontale"));
    EQkh->setReadOnly( true );
    addVarToContainer( EQkh );

    connect( EQbetaM, SIGNAL(valueChanged(QString)), this, SLOT(setEQkh()));
    connect( EQaMax, SIGNAL(valueChanged(QString)), this, SLOT(setEQkh()));
    connect( EQkh, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEQkh()));

    EQkv = new DoublePlus (0.0, "EQkv", m_unitMeasure, UnitMeasure::noDimension );
    EQkv->setRichName( QObject::trUtf8("k<span style=\" vertical-align:sub;\">v</span>"));
    EQkv->setToolTip( QObject::trUtf8("Coefficiente di spinta inerziale verticale"));
    EQkv->setReadOnly( true );
    addVarToContainer( EQkv );

    connect( EQkh, SIGNAL(valueChanged(QString)), this, SLOT(setEQkv()));
    connect( EQkv, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEQkv()));

    EQDirection = new RWEarthQuakeDirection( RetainingWall::EQdown, "EQDirection");
    EQDirection->setRichName( QObject::trUtf8("Direzione del sisma"));
    EQDirection->setToolTip( QObject::trUtf8("Direzione in cui agisce la componente inerziale"));
    addVarToContainer( EQDirection );

    EQtheta = new DoublePlus (0.0, "EQtheta", m_unitMeasure, UnitMeasure::angle );
    EQtheta->setRichName( QObject::trUtf8("θ"));
    EQtheta->setToolTip( QObject::trUtf8("Inclinazione carichi in condizioni sismiche"));
    EQtheta->setReadOnly( true );
    addVarToContainer( EQtheta );

    connect( EQkh, SIGNAL(valueChanged(QString)), this, SLOT(setEQtheta()));
    connect( EQkv, SIGNAL(valueChanged(QString)), this, SLOT(setEQtheta()));
    connect( EQDirection, SIGNAL(valueChanged(QString)), this, SLOT(setEQtheta()));
    connect( EQtheta, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEQtheta()));

    EQoverTurningSoilUpPhiPrimeD = new DoublePlus(0.0, "EQoverTurningSoilUpPhiPrimeD", m_unitMeasure, UnitMeasure::angle );
    EQoverTurningSoilUpPhiPrimeD->setRichName( QObject::trUtf8("φ'<span style=\" vertical-align:sub;\">monte,d</span>"));
    EQoverTurningSoilUpPhiPrimeD->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Angolo di attrito di calcolo del terreno a monte"));
    EQoverTurningSoilUpPhiPrimeD->setReadOnly( true );
    addVarToContainer( EQoverTurningSoilUpPhiPrimeD );

    EQoverTurningSoilUpCPrimeD = new DoublePlus(0.0, "EQoverTurningSoilUpCPrimeD", m_unitMeasure, UnitMeasure::tension );
    EQoverTurningSoilUpCPrimeD->setRichName( QObject::trUtf8("c'<span style=\" vertical-align:sub;\">monte,d</span>"));
    EQoverTurningSoilUpCPrimeD->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Coesione di calcolo del terreno a monte"));
    EQoverTurningSoilUpCPrimeD->setReadOnly( true );
    addVarToContainer( EQoverTurningSoilUpCPrimeD );

    EQoverTurningSoilDownPhiPrimeD = new DoublePlus(0.0, "EQoverTurningSoilDownPhiPrimeD", m_unitMeasure, UnitMeasure::angle );
    EQoverTurningSoilDownPhiPrimeD->setRichName( QObject::trUtf8("φ'<span style=\" vertical-align:sub;\">valle,d</span>"));
    EQoverTurningSoilDownPhiPrimeD->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Angolo di attrito di calcolo del terreno a valle"));
    EQoverTurningSoilDownPhiPrimeD->setReadOnly( true );
    addVarToContainer( EQoverTurningSoilDownPhiPrimeD );

    EQoverTurningSoilDownCPrimeD = new DoublePlus(0.0, "EQoverTurningSoilDownCPrimeD", m_unitMeasure, UnitMeasure::tension );
    EQoverTurningSoilDownCPrimeD->setRichName( QObject::trUtf8("c'<span style=\" vertical-align:sub;\">valle,d</span>"));
    EQoverTurningSoilDownCPrimeD->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Coesione di calcolo del terreno a valle"));
    EQoverTurningSoilDownCPrimeD->setReadOnly( true );
    addVarToContainer( EQoverTurningSoilDownCPrimeD );

    EQoverTurningSoilUpDeltaD = new DoublePlus(0.0, "EQoverTurningSoilUpDeltaD", m_unitMeasure, UnitMeasure::angle );
    EQoverTurningSoilUpDeltaD->setRichName( QObject::trUtf8("φ<span style=\" vertical-align:sub;\">monte,d</span>"));
    EQoverTurningSoilUpDeltaD->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Angolo di attrito di calcolo del terreno a monte"));
    EQoverTurningSoilUpDeltaD->setReadOnly( false );
    addVarToContainer( EQoverTurningSoilUpDeltaD );

    EQoverTurningSoilDownDeltaD = new DoublePlus(0.0, "EQoverTurningSoilDownDeltaD", m_unitMeasure, UnitMeasure::angle );
    EQoverTurningSoilDownDeltaD->setRichName( QObject::trUtf8("φ<span style=\" vertical-align:sub;\">valle,d</span>"));
    EQoverTurningSoilDownDeltaD->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Angolo di attrito di calcolo del terreno a valle"));
    EQoverTurningSoilDownDeltaD->setReadOnly( true );
    addVarToContainer( EQoverTurningSoilDownDeltaD );

    EQoverTurningKa = new DoublePlus(0.0, "EQoverTurningKa", m_unitMeasure, UnitMeasure::noDimension );
    EQoverTurningKa->setRichName( QObject::trUtf8("K<span style=\" vertical-align:sub;\">a</span>"));
    EQoverTurningKa->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Coefficiente di spinta attiva"));
    EQoverTurningKa->setReadOnly( true );
    addVarToContainer( EQoverTurningKa );

    EQoverTurningApplyKp = new BoolPlus( false, "EQoverTurningApplyKp" );
    EQoverTurningApplyKp->setToolTip( trUtf8("Verifica a ribaltamento in condizioni sismiche - Se si tiene conto o meno del contributo della resistenza passiva nelle verifiche"));
    EQoverTurningApplyKp->setRichName( trUtf8("Resistenza passiva contro fondazione"));
    EQoverTurningApplyKp->setReadOnly( false );
    addVarToContainer( EQoverTurningApplyKp );

    EQoverTurningKp = new DoublePlus(0.0, "EQoverTurningKp", m_unitMeasure, UnitMeasure::noDimension );
    EQoverTurningKp->setRichName( QObject::trUtf8("K<span style=\" vertical-align:sub;\">p</span>"));
    EQoverTurningKp->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Coefficiente di spinta passiva"));
    EQoverTurningKp->setReadOnly( true );
    addVarToContainer( EQoverTurningKp );

    EQoverTurningChiKp = new DoublePlus(0.5, "EQoverTurningChiKp", m_unitMeasure, UnitMeasure::noDimension );
    EQoverTurningChiKp->setRichName( QObject::trUtf8("χ<span style=\" vertical-align:sub;\">kp</span>"));
    EQoverTurningChiKp->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Fattore riduttivo del coefficiente di spinta passiva"));
    EQoverTurningChiKp->setReadOnly( true );
    addVarToContainer( EQoverTurningChiKp );

    EQoverTurningKpRed = new DoublePlus(0.0, "EQoverTurningKpRed", m_unitMeasure, UnitMeasure::noDimension );
    EQoverTurningKpRed->setRichName( QObject::trUtf8("K<span style=\" vertical-align:sub;\">p,red</span>"));
    EQoverTurningKpRed->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Coefficiente spinta passiva ridotto"));
    EQoverTurningKpRed->setReadOnly( true );
    addVarToContainer( EQoverTurningKpRed );

    EQoverTurningC = new Point2DPlus("EQoverTurningC", "y", 0.0, "z", 0.0, m_unitMeasure, UnitMeasure::length );
    EQoverTurningC->y->setRichName( QObject::trUtf8("y<span style=\" vertical-align:sub;\">C</span>"));
    EQoverTurningC->y->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Polo per calcolo momenti - y"));
    EQoverTurningC->y->setReadOnly( true );
    EQoverTurningC->z->setRichName( QObject::trUtf8("z<span style=\" vertical-align:sub;\">C</span>"));
    EQoverTurningC->z->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Polo per calcolo momenti - z"));
    EQoverTurningC->z->setReadOnly( true );
    addVarToContainer( EQoverTurningC );

    EQoverTurningMEd = new DoublePlus(0.0, "EQoverTurningMEd", m_unitMeasure, UnitMeasure::loadM, true );
    EQoverTurningMEd->setRichName( QObject::trUtf8("M<span style=\" vertical-align:sub;\">Ed</span>"));
    EQoverTurningMEd->setToolTip( QObject::trUtf8("Verifica a ribaltamento - Momento ribaltante"));
    EQoverTurningMEd->setReadOnly( true );
    addVarToContainer( EQoverTurningMEd );

    EQoverTurningMRd = new DoublePlus(0.0, "EQoverTurningMRd", m_unitMeasure, UnitMeasure::loadM, true );
    EQoverTurningMRd->setRichName( QObject::trUtf8("M<span style=\" vertical-align:sub;\">Rd</span>"));
    EQoverTurningMRd->setToolTip( QObject::trUtf8("Verifica a ribaltamento in condizioni sismiche - Momento resistente"));
    EQoverTurningMRd->setReadOnly( true );
    addVarToContainer( EQoverTurningMRd );
}

void RetainingWallGUIPrivate::setEQSS(){
    if( EQSS->readOnly() ){
        if( retainingWall ){
            if( retainingWall->soilDown() ){
                double v = 1.0;
                switch(retainingWall->soilDown()->soilEQCategory->valueNormal() ){
                case Soil::B: {
                    v =1.40 - 0.40 * EQF0->valueNormal() * EQag->valueNormal();
                    if( v < 1.00 )
                        v = 1.00;
                    else if( v > 1.40 )
                        v = 1.40;
                    break; }
                case Soil::C :{
                    v =1.70 - 0.60 * EQF0->valueNormal() * EQag->valueNormal();
                    if( v < 1.00 )
                        v = 1.00;
                    else if( v > 1.50 )
                        v = 1.50;
                    break;}
                case Soil::D:{
                    v =2.40 - 1.50 * EQF0->valueNormal() * EQag->valueNormal();
                    if( v < 0.90 )
                        v = 0.90;
                    else if( v > 1.80 )
                        v = 1.80;
                    break;}
                case Soil::E:{
                    v =2.0 - 1.1 * EQF0->valueNormal() * EQag->valueNormal();
                    if( v < 1.00 )
                        v = 1.00;
                    else if( v > 1.60 )
                        v = 1.60;
                    break;}
                default:{
                    v = 1.0;
                    break;}
                }
                EQSS->setValueNormal(v);
            }
        }
    }
}

void RetainingWallGUIPrivate::setEQbetaM(){
    if( EQbetaM->readOnly() ){
        if( retainingWall ){
            if( retainingWall->soilDown() ){
                if( retainingWall->soilDown()->soilEQCategory->valueNormal() == Soil::A ){
                    if( EQag->valueNormal() <= 0.1 ){
                        EQbetaM->setValueNormal( 0.20 );
                    } else if( EQag->valueNormal() <= 0.2 ){
                        EQbetaM->setValueNormal( 0.29 );
                    } else {
                        EQbetaM->setValueNormal( 0.31 );
                    }
                } else {
                    if( EQag->valueNormal() <= 0.1 ){
                        EQbetaM->setValueNormal( 0.18 );
                    } else if( EQag->valueNormal() <= 0.2 ){
                        EQbetaM->setValueNormal( 0.24 );
                    } else {
                        EQbetaM->setValueNormal( 0.31 );
                    }
                }
            }
        }
    }
}

void RetainingWallGUIPrivate::setEQS(){
    if( EQS->readOnly() ){
        EQS->setValueNormal( EQSS->valueNormal() * EQST->valueNormal() );
    }
}

void RetainingWallGUIPrivate::setEQaMax(){
    if( EQaMax->readOnly() ){
        EQaMax->setValueNormal( EQS->valueNormal() * EQag->valueNormal() );
    }
}

void RetainingWallGUIPrivate::setEQkh(){
    if( EQkh->readOnly() ){
        EQkh->setValueNormal( EQbetaM->valueNormal() * EQaMax->valueNormal() );
    }
}

void RetainingWallGUIPrivate::setEQkv(){
    if( EQkv->readOnly() ){
        EQkv->setValueNormal( 0.50 * EQkh->valueNormal() );
    }
}

void RetainingWallGUIPrivate::setEQtheta(){
    if( EQtheta->readOnly() ){
        if( retainingWall ){
            retainingWall->calculateEQTheta(EQtheta, EQkh, EQkv, EQDirection);
        }
    }
}
