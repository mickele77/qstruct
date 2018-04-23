#include "bondedanchor.h"

#include "doubleplus.h"
#include "uintplus.h"
#include "qstringplus.h"

#include <cmath>

BondedAnchor::BondedAnchor( UnitMeasure * ump ):
    VarPlusContainerPlus(ump, "BondedAnchor", ""){
    initVar();
}

void BondedAnchor::initVar() {
    lVEd = new DoublePlus( 0.0, "lVEd", m_unitMeasure, UnitMeasure::sectL, false );
    lVEd->setToolTip(trUtf8("Braccio di applicazione della Componente trasversale agente sull'ancoraggio"));
    lVEd->setRichName( trUtf8("l<span style=\"vertical-align:sub;\">VEd</span>") );
    addVarToContainer( lVEd );

    VEdX = new DoublePlus( 0.0, "VEdX", m_unitMeasure, UnitMeasure::loadF, false );
    VEdX->setToolTip(trUtf8("Componente trasversale agente lungo X sull'ancoraggio"));
    VEdX->setRichName( trUtf8("V<span style=\"vertical-align:sub;\">Ed,x</span>") );
    addVarToContainer( VEdX );

    VEdY = new DoublePlus( 0.0, "VEdX", m_unitMeasure, UnitMeasure::loadF, false );
    VEdY->setToolTip(trUtf8("Componente trasversale agente lungo Y sull'ancoraggio"));
    VEdY->setRichName( trUtf8("V<span style=\"vertical-align:sub;\">Ed,y</span>") );
    addVarToContainer( VEdY );

    TEd = new DoublePlus( 0.0, "TEd", m_unitMeasure, UnitMeasure::loadM, false );
    TEd->setToolTip(trUtf8("Momento indotto dalla componente trasversale"));
    TEd->setRichName( trUtf8("T<span style=\"vertical-align:sub;\">Ed</span>") );
    addVarToContainer( TEd );

    eNEdX = new DoublePlus( 0.0, "eNEdX", m_unitMeasure, UnitMeasure::sectL, false );
    eNEdX->setToolTip(trUtf8("Eccentricità della componente di trazione lungo X"));
    eNEdX->setRichName( trUtf8("e<span style=\"vertical-align:sub;\">NEd,x</span>") );
    addVarToContainer( eNEdX );

    eNEdY = new DoublePlus( 0.0, "eNEdY", m_unitMeasure, UnitMeasure::sectL, false );
    eNEdY->setToolTip(trUtf8("Eccentricità della componente di trazione lungo Y"));
    eNEdY->setRichName( trUtf8("e<span style=\"vertical-align:sub;\">NEd,y</span>") );
    addVarToContainer( eNEdY );

    NEd = new DoublePlus( 0.0, "NEd", m_unitMeasure, UnitMeasure::loadF, false );
    NEd->setToolTip(trUtf8("Componente di trazione"));
    NEd->setRichName( trUtf8("N<span style=\"vertical-align:sub;\">Ed</span>") );
    addVarToContainer( NEd );

    d = new DoublePlus( 1.6e-2, "d", m_unitMeasure, UnitMeasure::sectL, false );
    d->setToolTip(trUtf8("Diametro barra acciaio"));
    d->setRichName( trUtf8("d") );
    addVarToContainer( d );

    d0 = new DoublePlus( 2.0e-2, "d0", m_unitMeasure, UnitMeasure::sectL, false );
    d0->setToolTip(trUtf8("Diametro foro"));
    d0->setRichName( trUtf8("d<span style=\"vertical-align:sub;\">0</span>") );
    addVarToContainer( d0 );

    As = new DoublePlus( 157.0e-6, "As", m_unitMeasure, UnitMeasure::sectL2, false );
    As->setToolTip(trUtf8("Area resistente barra acciaio"));
    As->setRichName( trUtf8("A<span style=\"vertical-align:sub;\">s</span>") );
    addVarToContainer( As );
    As->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setAs) );
    As->addConnectedVars( 1, d );

    fyks = new DoublePlus( 6.40e+8, "fyks", m_unitMeasure, UnitMeasure::tension, false );
    fyks->setToolTip(trUtf8("Valore caratteristico della tensione di snervamento dell'acciaio"));
    fyks->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">yk,s</span>") );
    addVarToContainer( fyks );

    fuks = new DoublePlus( 8.0e+8, "fuks", m_unitMeasure, UnitMeasure::tension, false );
    fuks->setToolTip(trUtf8("Valore caratteristico della tensione di rottura dell'acciaio"));
    fuks->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">uk,s</span>") );
    addVarToContainer( fuks );

    fckCube = new DoublePlus( 2.5e+7, "fckCube", m_unitMeasure, UnitMeasure::tension, false );
    fckCube->setToolTip(trUtf8("Valore caratteristico della rottura a compressione su provino cubico del calcestruzzo"));
    fckCube->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">ck,cube</span>") );
    addVarToContainer( fckCube );

    nX = new UIntPlus( 1, "nHor", false );
    nX->setRichName( trUtf8("n<span style=\"vertical-align:sub;\">or</span>") );
    nX->setToolTip( trUtf8("Numero di ancoraggi presenti orizzontalmente" ) );
    addVarToContainer( nX );

    nY = new UIntPlus( 1, "nVer", false );
    nY->setRichName( trUtf8("n<span style=\"vertical-align:sub;\">ver</span>") );
    nY->setToolTip( trUtf8("Numero di ancoraggi presenti verticalmente" ) );
    addVarToContainer( nY );

    nTot = new UIntPlus( 1, "nTot", true );
    nTot->setRichName( trUtf8("n<span style=\"vertical-align:sub;\">tot</span>") );
    nTot->setToolTip( trUtf8("Numero di ancoraggi presenti nel gruppo" ) );
    addVarToContainer( nTot );
    nTot->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setNTot ) );
    nTot->addConnectedVars( 2, nX, nY );

    cXMinus = new DoublePlus( 0.1, "cHorLeft", m_unitMeasure, UnitMeasure::sectL, false );
    cXMinus->setRichName( trUtf8("c<span style=\"vertical-align:sub;\">or,←</span>") );
    cXMinus->setToolTip( trUtf8("Distanza orizzontale sinistra dal bordo" ) );
    addVarToContainer( cXMinus );

    sX = new DoublePlus( 2.0e-1, "sHor", m_unitMeasure, UnitMeasure::sectL, false );
    sX->setRichName( trUtf8("s<span style=\"vertical-align:sub;\">or</span>") );
    sX->setToolTip( trUtf8("Interasse orizzontale tra gli ancoraggi") );
    addVarToContainer( sX );

    cXPlus = new DoublePlus( 0.1, "cHorRight", m_unitMeasure, UnitMeasure::sectL, false );
    cXPlus->setRichName( trUtf8("c<span style=\"vertical-align:sub;\">or,→</span>") );
    cXPlus->setToolTip( trUtf8("Distanza orizzontale destra dal bordo" ) );
    addVarToContainer( cXPlus );

    cYMinus = new DoublePlus( 0.1, "cVerDown", m_unitMeasure, UnitMeasure::sectL, false );
    cYMinus->setRichName( trUtf8("c<span style=\"vertical-align:sub;\">ver,↓</span>") );
    cYMinus->setToolTip( trUtf8("Distanza verticale inferiore dal bordo" ) );
    addVarToContainer( cYMinus );

    sY = new DoublePlus( 0.15, "sVer", m_unitMeasure, UnitMeasure::sectL, false );
    sY->setRichName( trUtf8("s<span style=\"vertical-align:sub;\">ver</span>") );
    sY->setToolTip( trUtf8("Interasse verticale tra gli ancoraggi" ) );
    addVarToContainer( sY );

    cYPlus = new DoublePlus( 0.1, "sVerUp", m_unitMeasure, UnitMeasure::sectL, false );
    cYPlus->setRichName( trUtf8("c<span style=\"vertical-align:sub;\">ver,↑</span>") );
    cYPlus->setToolTip( trUtf8("Distanza verticale superiore dal bordo" ) );
    addVarToContainer( cYPlus );

    h = new DoublePlus( 3.0e-1, "h", m_unitMeasure, UnitMeasure::sectL, false );
    h->setToolTip(trUtf8("Spessore del supporto in calcestruzzo"));
    h->setRichName( trUtf8("h") );
    addVarToContainer( h );

    hEff = new DoublePlus( 1.0e-1, "hEff", m_unitMeasure, UnitMeasure::sectL, false );
    hEff->setToolTip(trUtf8("Effettiva profondità di ancoraggio"));
    hEff->setRichName( trUtf8("h<span style=\"vertical-align:sub;\">ef</span>") );
    addVarToContainer( hEff );

    hEffMin = new DoublePlus( 0.8e-1, "hEffMin", m_unitMeasure, UnitMeasure::sectL, false );
    hEffMin->setToolTip(trUtf8("Profondità di ancoraggio minima"));
    hEffMin->setRichName( trUtf8("h<span style=\"vertical-align:sub;\">ef,min</span>") );
    addVarToContainer( hEffMin );

    hEffMax = new DoublePlus( 2.0e-1, "hEffMax", m_unitMeasure, UnitMeasure::sectL, false );
    hEffMax->setToolTip(trUtf8("Profondità di ancoraggio massima"));
    hEffMax->setRichName( trUtf8("h<span style=\"vertical-align:sub;\">ef,max</span>") );
    addVarToContainer( hEffMax );

    hMin = new DoublePlus( 0.8e-1, "hMin", m_unitMeasure, UnitMeasure::sectL, false );
    hMin->setToolTip(trUtf8("Spessore minimo del supporto"));
    hMin->setRichName( trUtf8("h<span style=\"vertical-align:sub;\">min</span>") );
    addVarToContainer( hMin );

    sMin = new DoublePlus( 1.0e-1, "sMin", m_unitMeasure, UnitMeasure::sectL, false );
    sMin->setToolTip(trUtf8("Interasse minimo ancoraggi"));
    sMin->setRichName( trUtf8("s<span style=\"vertical-align:sub;\">min</span>") );
    addVarToContainer( sMin );

    cMin = new DoublePlus( 1.0e-1, "cMin", m_unitMeasure, UnitMeasure::sectL, false );
    cMin->setToolTip(trUtf8("Distanza minima dal bordo degli ancoraggi"));
    cMin->setRichName( trUtf8("c<span style=\"vertical-align:sub;\">min</span>") );
    addVarToContainer( cMin );

    tauRkCr = new DoublePlus( 0.0, "tauRkCr", m_unitMeasure, UnitMeasure::tension, false );
    tauRkCr->setToolTip(trUtf8("Tensione caratteristica calcestruzzo fessurato"));
    tauRkCr->setRichName( trUtf8("τ<span style=\"vertical-align:sub;\">Rk,cr</span>") );
    addVarToContainer( tauRkCr );

    tauRkUCr = new DoublePlus( 0.0, "tauRkUCr", m_unitMeasure, UnitMeasure::tension, false );
    tauRkUCr->setToolTip(trUtf8("Tensione caratteristica calcestruzzo non fessurato"));
    tauRkUCr->setRichName( trUtf8("τ<span style=\"vertical-align:sub;\">Rk,ucr</span>") );
    addVarToContainer( tauRkUCr );

    tauRkUCr25 = new DoublePlus( 0.0, "tauRkUCr25", m_unitMeasure, UnitMeasure::tension, false );
    tauRkUCr25->setToolTip(trUtf8("Tensione caratteristica calcestruzzo C20/25 non fessurato"));
    tauRkUCr25->setRichName( trUtf8("τ<span style=\"vertical-align:sub;\">Rk,ucr,C20/25</span>") );
    addVarToContainer( tauRkUCr25 );

    concreteCrack = new BAnchorConcrete( BondedAnchor::Cracked, "concreteCrack", false );
    concreteCrack->setToolTip(trUtf8("Condizioni di fessurazione del supporto in calcestruzzo"));
    concreteCrack->setRichName( trUtf8("Fessurazione") );
    addVarToContainer( concreteCrack );

    internalReinforcement = new BAnchorInternalReinforcement( BondedAnchor::Rare, "internalReinforcement", false );
    internalReinforcement->setToolTip(trUtf8("Presenza di armatura rada (Ø>10mm e spaziatura a≥150mm o Ø≤10mm e spaziatura a≥100mm) o fitta"));
    internalReinforcement->setRichName( trUtf8("Armatura interna") );
    addVarToContainer( internalReinforcement );

    edgeReinforcement = new BAnchorEdgeReinforcement( BondedAnchor::EdgeNotReinforced, "edgeReinforcement", false );
    edgeReinforcement->setToolTip(trUtf8("Armatura nello spigolo (armatura assente, armatura Ø≥12mm presente, armatura Ø≥12mm presente con staffatura  fitta a≤100mm )"));
    edgeReinforcement->setRichName( trUtf8("Armatura spigolo") );
    addVarToContainer( edgeReinforcement );

    NRks = new DoublePlus( 0.0, "NRks", m_unitMeasure, UnitMeasure::loadF, false );
    NRks->setToolTip(trUtf8("Valore caratteristico della resistenza a trazione dell'ancoraggio in caso di rottura dell'acciaio"));
    NRks->setRichName( trUtf8("N<span style=\"vertical-align:sub;\">Rk,s</span>") );
    addVarToContainer( NRks );
    NRks->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setNRks) );
    NRks->addConnectedVars( 2, As, fuks );

    gammaMsN = new DoublePlus( 2.0, "gammaMsN", m_unitMeasure, UnitMeasure::noDimension, true );
    gammaMsN->setToolTip(trUtf8("Coefficiente sicurezza rottura acciaio - Trazione"));
    gammaMsN->setRichName( trUtf8("γ<span style=\"vertical-align:sub;\">Ms,N</span>") );
    addVarToContainer( gammaMsN );
    gammaMsN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setGammaMsN ) );
    gammaMsN->addConnectedVars( 2, fyks, fuks );

    NhRds = new DoublePlus( 0.0, "NhRds", m_unitMeasure, UnitMeasure::loadF, true );
    NhRds->setToolTip(trUtf8("Valore di progetto della resistenza a trazione del singolo ancoraggio in caso di rottura dell'acciaio"));
    NhRds->setRichName( trUtf8("N<span style=\"vertical-align:super;\">h</span><span style=\"vertical-align:sub;\">Rd,s</span>") );
    addVarToContainer( NhRds );
    NhRds->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setNhRds) );
    NhRds->addConnectedVars( 2, NRks, gammaMsN );

    sCrNp = new DoublePlus( 1.0e-1, "sCrNp", m_unitMeasure, UnitMeasure::sectL, true );
    sCrNp->setToolTip(trUtf8("Interasse"));
    sCrNp->setRichName( trUtf8("s<span style=\"vertical-align:sub;\">cr,Np</span>") );
    addVarToContainer( sCrNp );
    sCrNp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setSCrNp) );
    sCrNp->addConnectedVars( 3, hEff, tauRkUCr25, d );

    cCrNp = new DoublePlus( 1.0e-1, "cCrNp", m_unitMeasure, UnitMeasure::sectL, true );
    cCrNp->setToolTip(trUtf8("Distanza dal bordo"));
    cCrNp->setRichName( trUtf8("c<span style=\"vertical-align:sub;\">cr,Np</span>") );
    addVarToContainer( cCrNp );
    cCrNp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setCCrNp) );
    cCrNp->addConnectedVars( 3, hEff, tauRkUCr25, d );

    ApN = new DoublePlus( 0.0, "ApN", m_unitMeasure, UnitMeasure::sectL2, true );
    ApN->setToolTip(trUtf8("Area di influenza effettiva"));
    ApN->setRichName( trUtf8("A<span style=\"vertical-align:sub;\">p,N</span>") );
    addVarToContainer( ApN );
    ApN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setApN ) );
    ApN->addConnectedVars( 8, sCrNp, cCrNp, cXMinus, sX, cXPlus, cYMinus, sY, cYPlus );

    A0pN = new DoublePlus( 0.0, "A0pN", m_unitMeasure, UnitMeasure::sectL2, true );
    A0pN->setToolTip(trUtf8("Area di influenza massima ancoraggio singolo"));
    A0pN->setRichName( trUtf8("A<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">p,N</span>") );
    addVarToContainer( A0pN );
    A0pN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setA0pN ) );
    A0pN->addConnectedVars( 6, sCrNp, cCrNp, cXMinus, cXPlus, cYMinus, cYPlus );

    psiSNp = new DoublePlus( 0.0, "psiSNp", m_unitMeasure, UnitMeasure::noDimension, true );
    psiSNp->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">s,Np</span>") );
    addVarToContainer( psiSNp );
    psiSNp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiSNp ) );
    psiSNp->addConnectedVars( 5, cXMinus, cXPlus, cYMinus, cYPlus, cCrNp );

    psiGNp = new DoublePlus( 0.0, "psiGNp", m_unitMeasure, UnitMeasure::noDimension, true );
    psiGNp->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">g,Np</span>") );
    addVarToContainer( psiGNp );
    psiGNp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiGNp ) );
    psiGNp->addConnectedVars( 10, nTot, tauRkUCr, tauRkCr, concreteCrack, d, hEff, fckCube, sX, sY, sCrNp );

    psiEcNpX = new DoublePlus( 1.0, "psiEcNpX", m_unitMeasure, UnitMeasure::noDimension, true );
    psiEcNpX->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">ec,Np,x</span>") );
    addVarToContainer( psiEcNpX );
    psiEcNpX->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiEcNpX ) );
    psiEcNpX->addConnectedVars( 2, eNEdX, sCrNp );

    psiEcNpY = new DoublePlus( 1.0, "psiEcNpY", m_unitMeasure, UnitMeasure::noDimension, true );
    psiEcNpY->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">ec,Np,y</span>") );
    addVarToContainer( psiEcNpY );
    psiEcNpY->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiEcNpY ) );
    psiEcNpY->addConnectedVars( 2, eNEdY, sCrNp );

    psiReNp = new DoublePlus( 0.0, "psiReNp", m_unitMeasure, UnitMeasure::noDimension, true );
    psiReNp->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">re,Np</span>") );
    addVarToContainer( psiReNp );
    psiReNp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiReNp ) );
    psiReNp->addConnectedVars( 2, internalReinforcement, hEff );

    N0Rkp = new DoublePlus( 0.0, "N0Rkp", m_unitMeasure, UnitMeasure::loadF, true );
    N0Rkp->setToolTip(trUtf8("Valore caratteristico massimo della resistenza in caso di rottura composta per pull-out e cono di calcestruzzo, riferito ad ancoraggio singolo - Trazione"));
    N0Rkp->setRichName( trUtf8("N<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">Rk,p</span>") );
    addVarToContainer( N0Rkp );
    N0Rkp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setN0Rkp ) );
    N0Rkp->addConnectedVars( 10, d, hEff, concreteCrack, tauRkUCr, tauRkCr, cCrNp, cXMinus, cXPlus, cYMinus, cYPlus );

    NRkp = new DoublePlus( 0.0, "NRkp", m_unitMeasure, UnitMeasure::loadF, true );
    NRkp->setToolTip(trUtf8("Valore caratteristico della resistenza dell'ancoraggio in caso di rottura composta per pull-out e cono di calcestruzzo - Trazione"));
    NRkp->setRichName( trUtf8("N<span style=\"vertical-align:sub;\">Rk,p</span>") );
    addVarToContainer( NRkp );
    NRkp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setNRkp ) );
    NRkp->addConnectedVars( 8, N0Rkp, ApN, A0pN, psiSNp, psiGNp, psiEcNpX, psiEcNpY, psiReNp );

    gammaC = new DoublePlus( 1.5, "gammaC", m_unitMeasure, UnitMeasure::noDimension, false );
    gammaC->setToolTip(trUtf8("Coefficiente sicurezza rottura calcestruzzo"));
    gammaC->setRichName( trUtf8("γ<span style=\"vertical-align:sub;\">c</span>") );
    addVarToContainer( gammaC );

    gamma2 = new DoublePlus( 1.2, "gamma2", m_unitMeasure, UnitMeasure::noDimension, false );
    gamma2->setToolTip(trUtf8("Coefficiente sicurezza installazione"));
    gamma2->setRichName( trUtf8("γ<span style=\"vertical-align:sub;\">2</span>") );
    addVarToContainer( gamma2 );

    gammaMpN = new DoublePlus( 1.0, "gammaMpN", m_unitMeasure, UnitMeasure::noDimension, true );
    gammaMpN->setToolTip(trUtf8("Coefficiente sicurezza rottura per pull-out e cono di cls - Trazione"));
    gammaMpN->setRichName( trUtf8("γ<span style=\"vertical-align:sub;\">Mp,N</span>") );
    addVarToContainer( gammaMpN );
    gammaMpN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setGammaMpN ) );
    gammaMpN->addConnectedVars( 2, gammaC, gamma2 );

    NgRdp = new DoublePlus( 0.0, "NgRdp", m_unitMeasure, UnitMeasure::loadF, true );
    NgRdp->setToolTip(trUtf8("Valore di progetto della resistenza del gruppo di ancoraggi in caso di rottura composta per pull-out e cono di calcestruzzo - Trazione"));
    NgRdp->setRichName( trUtf8("N<span style=\"vertical-align:super;\">g</span><span style=\"vertical-align:sub;\">Rd,p</span>") );
    addVarToContainer( NgRdp );
    NgRdp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setNgRdp) );
    NgRdp->addConnectedVars( 2, NRkp, gammaMpN );

    sCrN = new DoublePlus( 1.0e-1, "sCrN", m_unitMeasure, UnitMeasure::sectL, true );
    sCrN->setToolTip(trUtf8("Interasse - Trazione"));
    sCrN->setRichName( trUtf8("s<span style=\"vertical-align:sub;\">cr,N</span>") );
    addVarToContainer( sCrN );
    sCrN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setSCrN) );
    sCrN->addConnectedVars( 1, hEff );

    cCrN = new DoublePlus( 1.0e-1, "cCrN", m_unitMeasure, UnitMeasure::sectL, true );
    cCrN->setToolTip(trUtf8("Distanza dal bordo - Trazione"));
    cCrN->setRichName( trUtf8("c<span style=\"vertical-align:sub;\">cr,N</span>") );
    addVarToContainer( cCrN );
    cCrN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setCCrN) );
    cCrN->addConnectedVars( 1, hEff );

    AcN = new DoublePlus( 0.0, "AcN", m_unitMeasure, UnitMeasure::sectL2, true );
    AcN->setToolTip(trUtf8("Area di influenza effettiva - Trazione"));
    AcN->setRichName( trUtf8("A<span style=\"vertical-align:sub;\">c,N</span>") );
    addVarToContainer( AcN );
    AcN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setAcN ) );
    AcN->addConnectedVars( 8, sCrN, cCrN, cXMinus, sX, cXPlus, cYMinus, sY, cYPlus );

    A0cN = new DoublePlus( 0.0, "A0cN", m_unitMeasure, UnitMeasure::sectL2, true );
    A0cN->setToolTip(trUtf8("Area di influenza massima ancoraggio singolo - Trazione"));
    A0cN->setRichName( trUtf8("A<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">c,N</span>") );
    addVarToContainer( A0cN );
    A0cN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setA0cN ) );
    A0cN->addConnectedVars( 6, sCrN, cCrN, cXMinus, cXPlus, cYMinus, cYPlus );

    psiSN = new DoublePlus( 0.0, "psiSN", m_unitMeasure, UnitMeasure::noDimension, true );
    psiSN->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">s,N</span>") );
    addVarToContainer( psiSN );
    psiSN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiSN) );
    psiSN->addConnectedVars( 5, cXMinus, cXPlus, cYMinus, cYPlus, cCrN );

    psiEcNX = new DoublePlus( 1.0, "psiEcNX", m_unitMeasure, UnitMeasure::noDimension, true );
    psiEcNX->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">ec,N,x</span>") );
    addVarToContainer( psiEcNX );
    psiEcNX->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiEcNX ) );
    psiEcNX->addConnectedVars( 2, eNEdX, sCrN );

    psiEcNY = new DoublePlus( 1.0, "psiEcNY", m_unitMeasure, UnitMeasure::noDimension, true );
    psiEcNY->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">ec,N,y</span>") );
    addVarToContainer( psiEcNY );
    psiEcNY->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiEcNY ) );
    psiEcNY->addConnectedVars( 2, eNEdY, sCrN );

    psiReN = new DoublePlus( 0.0, "psiReN", m_unitMeasure, UnitMeasure::noDimension, true );
    psiReN->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">re,N</span>") );
    addVarToContainer( psiReN );
    psiReN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiReN ) );
    psiReN->addConnectedVars( 2, internalReinforcement, hEff );

    N0Rkc = new DoublePlus( 0.0, "N0Rkc", m_unitMeasure, UnitMeasure::loadF, true );
    N0Rkc->setToolTip(trUtf8("Valore caratteristico massimo della resistenza in caso di rottura del cono di calcestruzzo riferito ad ancoraggio singolo - Trazione"));
    N0Rkc->setRichName( trUtf8("N<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">Rk,c</span>") );
    addVarToContainer( N0Rkc );
    N0Rkc->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setN0Rkc ) );
    N0Rkc->addConnectedVars( 8, hEff, concreteCrack, fckCube, cXMinus, cCrN, cXPlus, cYMinus, cYPlus  );

    NRkc = new DoublePlus( 0.0, "NRkc", m_unitMeasure, UnitMeasure::loadF, true );
    NRkc->setToolTip(trUtf8("Valore caratteristico della resistenza dell'ancoraggio in caso di rottura del cono di calcestruzzo - Trazione"));
    NRkc->setRichName( trUtf8("N<span style=\"vertical-align:sub;\">Rk,c</span>") );
    addVarToContainer( NRkc );
    NRkc->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setNRkc ) );
    NRkc->addConnectedVars( 7, N0Rkc, AcN, A0cN, psiSN, psiEcNX, psiEcNY, psiReN );

    gammaMcN = new DoublePlus( 1.0, "gammaMcN", m_unitMeasure, UnitMeasure::noDimension, true );
    gammaMcN->setToolTip(trUtf8("Coefficiente sicurezza rottura cono di cls - Carico assiale"));
    gammaMcN->setRichName( trUtf8("γ<span style=\"vertical-align:sub;\">Mc,N</span>") );
    addVarToContainer( gammaMcN );
    gammaMcN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setGammaMcN ) );
    gammaMcN->addConnectedVars( 2, gammaC, gamma2 );

    NgRdc = new DoublePlus( 0.0, "NgRdc", m_unitMeasure, UnitMeasure::loadF, true );
    NgRdc->setToolTip(trUtf8("Valore di progetto della resistenza del gruppo di ancoraggi in caso di rottura del cono di calcestruzzo - Trazione"));
    NgRdc->setRichName( trUtf8("N<span style=\"vertical-align:super;\">g</span><span style=\"vertical-align:sub;\">Rd,c</span>") );
    addVarToContainer( NgRdc );
    NgRdc->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setNgRdc) );
    NgRdc->addConnectedVars( 2, NRkc, gammaMcN );

    ratioCCrSpOvEff = new DoublePlus( 0.6, "ratioCCrSpOvEff", m_unitMeasure, UnitMeasure::noDimension, false );
    ratioCCrSpOvEff->setToolTip(trUtf8("Rapporto tra cCrSp e l'altezza dell'ancoraggio"));
    ratioCCrSpOvEff->setRichName( trUtf8("c<span style=\"vertical-align:sub;\">cr,sp</span>/h<span style=\"vertical-align:sub;\">ef</span>") );
    addVarToContainer( ratioCCrSpOvEff );

    ratioSCrSpOvEff = new DoublePlus( 1.2, "ratioSCrSpOvEff", m_unitMeasure, UnitMeasure::noDimension, false );
    ratioSCrSpOvEff->setToolTip(trUtf8("Rapporto tra sCrSp e l'altezza dell'ancoraggio"));
    ratioSCrSpOvEff->setRichName( trUtf8("s<span style=\"vertical-align:sub;\">cr,sp</span>/h<span style=\"vertical-align:sub;\">ef</span>") );
    addVarToContainer( ratioSCrSpOvEff );

    sCrSp = new DoublePlus( 1.0e-1, "sCrSp", m_unitMeasure, UnitMeasure::sectL, false );
    sCrSp->setToolTip(trUtf8("Interasse - Trazione"));
    sCrSp->setRichName( trUtf8("s<span style=\"vertical-align:sub;\">cr,sp</span>") );
    addVarToContainer( sCrSp );
    sCrSp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setSCrSp ) );
    sCrSp->addConnectedVars( 2, hEff, ratioSCrSpOvEff );

    cCrSp = new DoublePlus( 1.0e-1, "cCrSp", m_unitMeasure, UnitMeasure::sectL, false );
    cCrSp->setToolTip(trUtf8("Distanza dal bordo - Trazione"));
    cCrSp->setRichName( trUtf8("c<span style=\"vertical-align:sub;\">cr,sp</span>") );
    addVarToContainer( cCrSp );
    cCrSp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setCCrSp ) );
    cCrSp->addConnectedVars( 2, hEff, ratioCCrSpOvEff );

    psiHSp = new DoublePlus( 0.0, "psiHSp", m_unitMeasure, UnitMeasure::noDimension, true );
    psiHSp->setRichName( trUtf8("ψ<span style=\"vertical-align:sub;\">h,sp</span>") );
    addVarToContainer( psiHSp );
    psiHSp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setPsiHSp ) );
    psiHSp->addConnectedVars( 3, h, hEff, hMin );

    NRkSp = new DoublePlus( 0.0, "NRkp", m_unitMeasure, UnitMeasure::loadF, true );
    NRkSp->setToolTip(trUtf8("Valore caratteristico della resistenza dell'ancoraggio in caso di rottura per splitting - Trazione"));
    NRkSp->setRichName( trUtf8("N<span style=\"vertical-align:sub;\">Rk,sp</span>") );
    addVarToContainer( NRkSp );
    NRkSp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setNRkSp ) );
    NRkSp->addConnectedVars( 8, N0Rkp, ApN, A0pN, psiSNp, psiEcNpX, psiEcNpY, psiReNp, psiHSp );

    gammaMSpN = new DoublePlus( 1.0, "gammaMSpN", m_unitMeasure, UnitMeasure::noDimension, true );
    gammaMSpN->setToolTip(trUtf8("Coefficiente sicurezza rottura per splittings - Carico assiale"));
    gammaMSpN->setRichName( trUtf8("γ<span style=\"vertical-align:sub;\">Msp,N</span>") );
    addVarToContainer( gammaMSpN );
    gammaMSpN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setGammaMSpN ) );
    gammaMSpN->addConnectedVars( 2, gammaC, gamma2 );

    NgRdSp = new DoublePlus( 0.0, "NgRdSp", m_unitMeasure, UnitMeasure::loadF, true );
    NgRdSp->setToolTip(trUtf8("Valore di progetto della resistenza del gruppo di ancoraggi in caso di rottura composta per pull-out e cono di calcestruzzo - Trazione"));
    NgRdSp->setRichName( trUtf8("N<span style=\"vertical-align:super;\">g</span><span style=\"vertical-align:sub;\">Rd,sp</span>") );
    addVarToContainer( NgRdSp );
    NgRdSp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setNgRdSp) );
    NgRdSp->addConnectedVars( 2, NRkSp, gammaMSpN );

    M0Rks = new DoublePlus( 0.0, "M0Rks", m_unitMeasure, UnitMeasure::loadM, false );
    M0Rks->setToolTip(trUtf8("Momento resistente dell'ancoraggio in acciaio - Taglio"));
    M0Rks->setRichName( trUtf8("M<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">Rk,s</span>") );
    addVarToContainer( M0Rks );

    MRks = new DoublePlus( 0.0, "MRks", m_unitMeasure, UnitMeasure::loadM, true );
    MRks->setToolTip(trUtf8("Momento resistente dell'ancoraggio in acciaio - Taglio"));
    MRks->setRichName( trUtf8("M<span style=\"vertical-align:sub;\">Rk,s</span>") );
    addVarToContainer( MRks );
    MRks->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setMRks ) );
    MRks->addConnectedVars( 3, M0Rks, NEd, NhRds );

    alphaM = new DoublePlus( 1.0, "alphaM", m_unitMeasure, UnitMeasure::noDimension, false );
    alphaM->setToolTip(trUtf8("Coefficiente per tener conto del grado di incastro dell'ancoraggio in caso di rottura dell'acciaio - Taglio"));
    alphaM->setRichName( trUtf8("α<span style=\"vertical-align:sub;\">M</span>") );
    addVarToContainer( alphaM );

    VRks = new DoublePlus( 0.0, "VRks", m_unitMeasure, UnitMeasure::loadF, false );
    VRks->setToolTip(trUtf8("Valore caratteristico della resistenza a taglio dell'ancoraggio in caso di rottura dell'acciaio - Taglio"));
    VRks->setRichName( trUtf8("V<span style=\"vertical-align:sub;\">Rk,s</span>") );
    addVarToContainer( VRks );
    VRks->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVRks ) );
    VRks->addConnectedVars( 6, nTot, As, fuks, alphaM, MRks, lVEd );

    gammaMsV = new DoublePlus( 1.0, "gammaMsV", m_unitMeasure, UnitMeasure::noDimension, true );
    gammaMsV->setToolTip(trUtf8("Coefficiente sicurezza rottura acciaio - Taglio"));
    gammaMsV->setRichName( trUtf8("γ<span style=\"vertical-align:sub;\">Ms,V</span>") );
    addVarToContainer( gammaMsV );
    gammaMsV->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setGammaMsV ) );
    gammaMsV->addConnectedVars( 2, fyks, fuks );

    VhRds = new DoublePlus( 0.0, "VhRds", m_unitMeasure, UnitMeasure::loadF, true );
    VhRds->setToolTip(trUtf8("Valore di progetto della resistenza a taglio del singolo ancoraggio in caso di rottura dell'acciaio - Taglio"));
    VhRds->setRichName( trUtf8("V<span style=\"vertical-align:super;\">h</span><span style=\"vertical-align:sub;\">Rd,s</span>") );
    addVarToContainer( VhRds );
    VhRds->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVhRds) );
    VhRds->addConnectedVars( 2, VRks, gammaMsV );

    kCp = new DoublePlus( 1.0, "kCp", m_unitMeasure, UnitMeasure::noDimension, true );
    kCp->setToolTip(trUtf8("Coefficiente k necessario per calcolo rottura per pry-out - Taglio"));
    kCp->setRichName( trUtf8("k") );
    addVarToContainer( kCp );
    kCp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setKCp ) );
    kCp->addConnectedVars( 1, hEff );

    VRkCp = new DoublePlus( 0.0, "VRkCp", m_unitMeasure, UnitMeasure::loadF, true );
    VRkCp->setToolTip(trUtf8("Valore caratteristico della resistenza dell'ancoraggio in caso di rottura per pry-out - Taglio"));
    VRkCp->setRichName( trUtf8("V<span style=\"vertical-align:sub;\">Rk,cp</span>") );
    addVarToContainer( VRkCp );
    VRkCp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVRkCp ) );
    VRkCp->addConnectedVars( 3, kCp, NRkp, NRkc );

    gammaMcV = new DoublePlus( 1.0, "gammaMcV", m_unitMeasure, UnitMeasure::noDimension, true );
    gammaMcV->setToolTip(trUtf8("Coefficiente sicurezza rottura bordo e pry-out - Taglio"));
    gammaMcV->setRichName( trUtf8("γ<span style=\"vertical-align:sub;\">Mc,V</span>") );
    addVarToContainer( gammaMcV );
    gammaMcV->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setGammaMcV ) );
    gammaMcV->addConnectedVars( 2, gammaC, gamma2 );

    VgRdCp = new DoublePlus( 0.0, "VgRdCp", m_unitMeasure, UnitMeasure::loadF, true );
    VgRdCp->setToolTip(trUtf8("Valore di progetto della resistenza a taglio del gruppo di ancoraggi in caso di rottura per pry-out - Taglio"));
    VgRdCp->setRichName( trUtf8("V<span style=\"vertical-align:super;\">g</span><span style=\"vertical-align:sub;\">Rd,cp</span>") );
    addVarToContainer( VgRdCp );
    VgRdCp->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVgRdCp) );
    VgRdCp->addConnectedVars( 2, VRkCp, gammaMcV );

    k1C = new DoublePlus( 1.7, "k1C", m_unitMeasure, UnitMeasure::noDimension, true );
    k1C->setToolTip(trUtf8("Coefficiente k1 necessario per calcolo rottura bordo - Taglio"));
    k1C->setRichName( trUtf8("k<span style=\"vertical-align:sub;\">1</span>") );
    addVarToContainer( k1C );
    k1C->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setK1C ) );
    k1C->addConnectedVars( 1, concreteCrack );

    A0CVXPlus = new DoublePlus( 0.0, "A0CVXPlus", m_unitMeasure, UnitMeasure::sectL2, true );
    A0CVXPlus->setToolTip(trUtf8("Area di influenza massima per calcolo rottura bordo - Taglio X+"));
    A0CVXPlus->setRichName( trUtf8("A<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">c,VX+</span>") );
    addVarToContainer( A0CVXPlus );
    A0CVXPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setA0CVXPlus ) );
    A0CVXPlus->addConnectedVars( 6, nY, h, cXPlus, cYMinus, sY, cYPlus );

    ACVXPlus = new DoublePlus( 0.0, "ACVXPlus", m_unitMeasure, UnitMeasure::sectL2, true );
    ACVXPlus->setToolTip(trUtf8("Area di influenza effettiva per calcolo rottura bordo - Taglio X+"));
    ACVXPlus->setRichName( trUtf8("A<span style=\"vertical-align:sub;\">c,VX+</span>") );
    addVarToContainer( ACVXPlus );
    ACVXPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setACVXPlus ) );
    ACVXPlus->addConnectedVars( 6, nY, h, cXPlus, cYMinus, sY, cYPlus );

    V0RkCXPlus = new DoublePlus( 0.0, "V0RkCXPlus", m_unitMeasure, UnitMeasure::loadF, true );
    V0RkCXPlus->setToolTip(trUtf8("Valore caratteristico di riferimento della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio X+"));
    V0RkCXPlus->setRichName( trUtf8("V<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">Rk,cX+</span>") );
    addVarToContainer( V0RkCXPlus );
    V0RkCXPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setV0RkCXPlus) );
    V0RkCXPlus->addConnectedVars( 9, h, cXPlus, cYMinus, sY, cYPlus, fckCube, hEff, d, k1C );

    VRkCXPlus = new DoublePlus( 0.0, "VRkCXPlus", m_unitMeasure, UnitMeasure::loadF, true );
    VRkCXPlus->setToolTip(trUtf8("Valore caratteristico della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio X+"));
    VRkCXPlus->setRichName( trUtf8("V<span style=\"vertical-align:sub;\">Rk,cX+</span>") );
    addVarToContainer( VRkCXPlus );
    VRkCXPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVRkCXPlus) );
    VRkCXPlus->addConnectedVars( 9, h, edgeReinforcement, V0RkCXPlus, ACVXPlus, A0CVXPlus,
                                 cXPlus, cYMinus, sY, cYPlus );

    VgRdCXPlus = new DoublePlus( 0.0, "VgRdCXPlus", m_unitMeasure, UnitMeasure::loadF, true );
    VgRdCXPlus->setToolTip(trUtf8("Valore caratteristico della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio X+"));
    VgRdCXPlus->setRichName( trUtf8("V<span style=\"vertical-align:super;\">g</span><span style=\"vertical-align:sub;\">Rd,cX+</span>") );
    addVarToContainer( VgRdCXPlus );
    VgRdCXPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVgRdCXPlus) );
    VgRdCXPlus->addConnectedVars( 2, VRkCXPlus, gammaMcV );

    A0CVXMinus = new DoublePlus( 0.0, "A0CVXMinus", m_unitMeasure, UnitMeasure::sectL2, true );
    A0CVXMinus->setToolTip(trUtf8("Area di influenza massima per calcolo rottura bordo - Taglio X-"));
    A0CVXMinus->setRichName( trUtf8("A<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">c,VX-</span>") );
    addVarToContainer( A0CVXMinus );
    A0CVXMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setA0CVXMinus ) );
    A0CVXMinus->addConnectedVars( 6, nY, h, cXMinus, cYMinus, sY, cYPlus );

    ACVXMinus = new DoublePlus( 0.0, "ACVXMinus", m_unitMeasure, UnitMeasure::sectL2, true );
    ACVXMinus->setToolTip(trUtf8("Area di influenza effettiva per calcolo rottura bordo - Taglio X-"));
    ACVXMinus->setRichName( trUtf8("A<span style=\"vertical-align:sub;\">c,VX-</span>") );
    addVarToContainer( ACVXMinus );
    ACVXMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setACVXMinus ) );
    ACVXMinus->addConnectedVars( 6, nY, h, cXMinus, cYMinus, sY, cYPlus );

    V0RkCXMinus = new DoublePlus( 0.0, "V0RkCXMinus", m_unitMeasure, UnitMeasure::loadF, true );
    V0RkCXMinus->setToolTip(trUtf8("Valore caratteristico di riferimento della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio X-"));
    V0RkCXMinus->setRichName( trUtf8("V<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">Rk,cX-</span>") );
    addVarToContainer( V0RkCXMinus );
    V0RkCXMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setV0RkCXMinus) );
    V0RkCXMinus->addConnectedVars( 9, h, cXMinus, cYMinus, sY, cYPlus, fckCube, hEff, d, k1C );

    VRkCXMinus = new DoublePlus( 0.0, "VRkCXMinus", m_unitMeasure, UnitMeasure::loadF, true );
    VRkCXMinus->setToolTip(trUtf8("Valore caratteristico della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio X-"));
    VRkCXMinus->setRichName( trUtf8("V<span style=\"vertical-align:sub;\">Rk,cX-</span>") );
    addVarToContainer( VRkCXMinus );
    VRkCXMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVRkCXMinus) );
    VRkCXMinus->addConnectedVars( 9, h, edgeReinforcement, V0RkCXMinus, ACVXMinus, A0CVXMinus,
                                  cXMinus, cYMinus, sY, cYPlus );

    VgRdCXMinus = new DoublePlus( 0.0, "VgRdCXMinus", m_unitMeasure, UnitMeasure::loadF, true );
    VgRdCXMinus->setToolTip(trUtf8("Valore di progetto della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio X-"));
    VgRdCXMinus->setRichName( trUtf8("V<span style=\"vertical-align:super;\">g</span><span style=\"vertical-align:sub;\">Rd,cX-</span>") );
    addVarToContainer( VgRdCXMinus );
    VgRdCXMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVgRdCXMinus) );
    VgRdCXMinus->addConnectedVars( 2, VRkCXMinus, gammaMcV );

    A0CVYPlus = new DoublePlus( 0.0, "A0CVYPlus", m_unitMeasure, UnitMeasure::sectL2, true );
    A0CVYPlus->setToolTip(trUtf8("Area di influenza massima per calcolo rottura bordo - Taglio Y+"));
    A0CVYPlus->setRichName( trUtf8("A<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">c,VY+</span>") );
    addVarToContainer( A0CVYPlus );
    A0CVYPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setA0CVYPlus ) );
    A0CVYPlus->addConnectedVars( 6, nX, h, cYPlus, cXMinus, sX, cXPlus );

    ACVYPlus = new DoublePlus( 0.0, "ACVYPlus", m_unitMeasure, UnitMeasure::sectL2, true );
    ACVYPlus->setToolTip(trUtf8("Area di influenza effettiva per calcolo rottura bordo - Taglio Y+"));
    ACVYPlus->setRichName( trUtf8("A<span style=\"vertical-align:sub;\">c,VY+</span>") );
    addVarToContainer( ACVYPlus );
    ACVYPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setACVYPlus ) );
    ACVYPlus->addConnectedVars( 6, nX, h, cYPlus, cXMinus, sX, cXPlus );

    V0RkCYPlus = new DoublePlus( 0.0, "V0RkCYPlus", m_unitMeasure, UnitMeasure::loadF, true );
    V0RkCYPlus->setToolTip(trUtf8("Valore caratteristico di riferimento della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio Y+"));
    V0RkCYPlus->setRichName( trUtf8("V<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">Rk,cY+</span>") );
    addVarToContainer( V0RkCYPlus );
    V0RkCYPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setV0RkCYPlus) );
    V0RkCYPlus->addConnectedVars( 9, h, cYPlus, cXMinus, sX, cXPlus, fckCube, hEff, d, k1C );

    VRkCYPlus = new DoublePlus( 0.0, "VRkCYPlus", m_unitMeasure, UnitMeasure::loadF, true );
    VRkCYPlus->setToolTip(trUtf8("Valore caratteristico della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio Y+"));
    VRkCYPlus->setRichName( trUtf8("V<span style=\"vertical-align:sub;\">Rk,cY+</span>") );
    addVarToContainer( VRkCYPlus );
    VRkCYPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVRkCYPlus) );
    VRkCYPlus->addConnectedVars( 9, h, edgeReinforcement, V0RkCYPlus, ACVYPlus, A0CVYPlus,
                                 cYPlus, cXMinus, sX, cXPlus );

    VgRdCYPlus = new DoublePlus( 0.0, "VgRdCYPlus", m_unitMeasure, UnitMeasure::loadF, true );
    VgRdCYPlus->setToolTip(trUtf8("Valore di progetto della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio Y+"));
    VgRdCYPlus->setRichName( trUtf8("V<span style=\"vertical-align:super;\">g</span><span style=\"vertical-align:sub;\">Rd,cY+</span>") );
    addVarToContainer( VgRdCYPlus );
    VgRdCYPlus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVgRdCYPlus) );
    VgRdCYPlus->addConnectedVars( 2, VRkCYPlus, gammaMcV );

    A0CVYMinus = new DoublePlus( 0.0, "A0CVYMinus", m_unitMeasure, UnitMeasure::sectL2, true );
    A0CVYMinus->setToolTip(trUtf8("Area di influenza massima per calcolo rottura bordo - Taglio Y-"));
    A0CVYMinus->setRichName( trUtf8("A<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">c,VY-</span>") );
    addVarToContainer( A0CVYMinus );
    A0CVYMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setA0CVYMinus ) );
    A0CVYMinus->addConnectedVars( 6, nX, h, cYMinus, cXMinus, sX, cXPlus );

    ACVYMinus = new DoublePlus( 0.0, "ACVYMinus", m_unitMeasure, UnitMeasure::sectL2, true );
    ACVYMinus->setToolTip(trUtf8("Area di influenza effettiva per calcolo rottura bordo - Taglio Y-"));
    ACVYMinus->setRichName( trUtf8("A<span style=\"vertical-align:sub;\">c,VY-</span>") );
    addVarToContainer( ACVYMinus );
    ACVYMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setACVYMinus ) );
    ACVYMinus->addConnectedVars( 6, nX, h, cYMinus, cXMinus, sX, cXPlus );

    V0RkCYMinus = new DoublePlus( 0.0, "V0RkCYMinus", m_unitMeasure, UnitMeasure::loadF, true );
    V0RkCYMinus->setToolTip(trUtf8("Valore caratteristico di riferimento della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio Y-"));
    V0RkCYMinus->setRichName( trUtf8("V<span style=\"vertical-align:super;\">0</span><span style=\"vertical-align:sub;\">Rk,cY-</span>") );
    addVarToContainer( V0RkCYMinus );
    V0RkCYMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setV0RkCYMinus) );
    V0RkCYMinus->addConnectedVars( 9, h, cYMinus, cXMinus, sX, cXPlus, fckCube, hEff, d, k1C );

    VRkCYMinus = new DoublePlus( 0.0, "VRkCYMinus", m_unitMeasure, UnitMeasure::loadF, true );
    VRkCYMinus->setToolTip(trUtf8("Valore caratteristico della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio Y-"));
    VRkCYMinus->setRichName( trUtf8("V<span style=\"vertical-align:sub;\">Rk,cY-</span>") );
    addVarToContainer( VRkCYMinus );
    VRkCYMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVRkCYMinus) );
    VRkCYMinus->addConnectedVars( 9, h, edgeReinforcement, V0RkCYMinus, ACVYMinus, A0CVYMinus,
                                  cYMinus, cXMinus, sX, cXPlus );

    VgRdCYMinus = new DoublePlus( 0.0, "VgRdCYMinus", m_unitMeasure, UnitMeasure::loadF, true );
    VgRdCYMinus->setToolTip(trUtf8("Valore di progetto della resistenza a taglio del gruppo di ancoraggi in caso di rottura del bordo - Taglio Y-"));
    VgRdCYMinus->setRichName( trUtf8("V<span style=\"vertical-align:super;\">g</span><span style=\"vertical-align:sub;\">Rd,cY-</span>") );
    addVarToContainer( VgRdCYMinus );
    VgRdCYMinus->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&BondedAnchor::setVgRdCYMinus) );
    VgRdCYMinus->addConnectedVars( 2, VRkCYMinus, gammaMcV );

    updateReadOnlyVars();
}

void BondedAnchor::setNTot(bool emitAuto) {
    nTot->setValueNormal( nX->valueNormal() * nY->valueNormal(), emitAuto );
}

void BondedAnchor::setAs(bool emitAuto) {
    As->setValueNormal( M_PI * pow(d->valueNormal()/2.0, 2.0), emitAuto );
}

void BondedAnchor::setNRks( bool emitAuto ){
    NRks->setValueNormal( As->valueNormal() * fuks->valueNormal(), emitAuto );
}

void BondedAnchor::setNhRds( bool emitAuto ){
    NhRds->setValueNormal( dVal(NRks->valueNormal(), gammaMsN->valueNormal()), emitAuto );
}

double BondedAnchor::moltNP() {
    // verifica lett. g) (special cases)
    double molt = 1.0;
    int cMinusCCrNPCount = 0;
    double cMaxVal = 0.0;
    if( cXMinus->valueNormal() < cCrNp->valueNormal() ){
        cMinusCCrNPCount++;
        cMaxVal = qMax( cMaxVal, cXMinus->valueNormal());
    }
    if( cXPlus->valueNormal() < cCrNp->valueNormal() ){
        cMinusCCrNPCount++;
        cMaxVal = qMax( cMaxVal, cXPlus->valueNormal());
    }
    if( cYMinus->valueNormal() < cCrNp->valueNormal() ){
        cMinusCCrNPCount++;
        cMaxVal = qMax( cMaxVal, cYMinus->valueNormal());
    }
    if( cYPlus->valueNormal() < cCrNp->valueNormal() ){
        cMinusCCrNPCount++;
        cMaxVal = qMax( cMaxVal, cYPlus->valueNormal());
    }
    if( cMinusCCrNPCount > 2 && cCrNp->valueNormal() > 0.0 ){
        molt = cMaxVal / cCrNp->valueNormal();
    }
    return molt;
}

double BondedAnchor::moltN() {
    // verifica lett. g) (special cases)
    double molt = 1.0;
    int cMinusCCrNPCount = 0;
    double cMaxVal = 0.0;
    if( cXMinus->valueNormal() < cCrN->valueNormal() ){
        cMinusCCrNPCount++;
        cMaxVal = qMax( cMaxVal, cXMinus->valueNormal());
    }
    if( cXPlus->valueNormal() < cCrN->valueNormal() ){
        cMinusCCrNPCount++;
        cMaxVal = qMax( cMaxVal, cXPlus->valueNormal());
    }
    if( cYMinus->valueNormal() < cCrN->valueNormal() ){
        cMinusCCrNPCount++;
        cMaxVal = qMax( cMaxVal, cYMinus->valueNormal());
    }
    if( cYPlus->valueNormal() < cCrN->valueNormal() ){
        cMinusCCrNPCount++;
        cMaxVal = qMax( cMaxVal, cYPlus->valueNormal());
    }
    if( cMinusCCrNPCount > 2 && cCrN->valueNormal() > 0.0 ){
        molt = cMaxVal / cCrN->valueNormal();
    }
    return molt;
}

void BondedAnchor::setApN(bool emitAuto) {
    double molt = moltNP();
    double cCrNpVal = cCrNp->valueNormal() * molt;
    double sCrNpVal = sCrNp->valueNormal() * molt;

    double ret = 0.0;
    for( unsigned int i=0; i < nY->valueNormal(); ++i ){
        for( unsigned int j=0; j < nX->valueNormal(); ++j ){
            double sDown = 0.0;
            if( i == 0 ) {
                sDown = qMin( cCrNpVal, cYMinus->valueNormal() );
            } else {
                sDown = 0.5 * qMin( sCrNpVal, sY->valueNormal() );
            }

            double sUp = 0.0;
            if( i == nY->valueNormal() - 1 ) {
                sUp = qMin( cCrNpVal, cYPlus->valueNormal() );
            } else {
                sUp = 0.5 * qMin( sCrNpVal, sY->valueNormal() );
            }

            double sLeft = 0.0;
            if( j == 0 ) {
                sLeft = qMin( cCrNpVal, cXMinus->valueNormal() );
            } else {
                sLeft = 0.5 * qMin( sCrNpVal, sX->valueNormal() );
            }

            double sRight = 0.0;
            if( j == nX->valueNormal() ) {
                sRight = qMin( cCrNpVal, cXMinus->valueNormal() );
            } else {
                sRight = 0.5 * qMin( sCrNpVal, sX->valueNormal() );
            }

            ret += (sLeft + sRight ) * ( sUp + sDown );
        }
    }

    ApN->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setA0pN(bool emitAuto) {
    double sCrNpVal = sCrNp->valueNormal() * moltNP();
    double ret = pow(sCrNpVal, 2.0);

    A0pN->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setCCrNp(bool emitAuto) {
    double ret = qMin( 10.0 * d->valueNormal() * pow( tauRkUCr25->valueNormal()*1e-6/7.5, 0.5), 3.0 * hEff->valueNormal() );

    cCrNp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setSCrNp(bool emitAuto) {
    double ret = qMin( 20.0 * d->valueNormal() * pow( tauRkUCr25->valueNormal()*1e-6/7.5, 0.5), 3.0 * hEff->valueNormal() );

    sCrNp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiSNp(bool emitAuto) {
    double ret = 0.0;

    if( cCrNp->valueNormal() != 0.0 ){
        double cMin = qMin( cXMinus->valueNormal(), cXPlus->valueNormal() );
        cMin = qMin( cMin, cYPlus->valueNormal() );
        cMin = qMin( cMin, cYMinus->valueNormal() );
        ret = qMin( 0.7 + 0.3 * cMin / cCrNp->valueNormal(), 1.0);
    }

    psiSNp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiGNp(bool emitAuto) {
    double ret = 0.0;

    // psi0GNp
    double radN = sqrt( nTot->valueNormal() );
    double trkVal = 0.0;
    double k = 0.0;
    if( concreteCrack->valueNormal() == NotCracked ){
        trkVal = tauRkUCr->valueNormal() * 1e-6;
        k = 3.2;
    } else {
        trkVal = tauRkCr->valueNormal() * 1e-6;
        k = 2.3;
    }
    double dVal = d->valueNormal() * 1.0e+3;
    double hEfVal = hEff->valueNormal() * 1.0e+3;
    double fckCubeVal = fckCube->valueNormal() * 1e-6;

    double psi0gNp = 1.0;
    double den = k * sqrt(hEfVal * fckCubeVal);
    if( den > 0.0 ){
        psi0gNp = radN - (radN-1.0) * pow( dVal * trkVal / den , 1.5);
    }
    if( psi0gNp < 1.0 ){
        psi0gNp = 1.0;
    }

    double sVal = 0.50 * (sY->valueNormal() + sX->valueNormal());
    if( sCrNp->valueNormal() > 0.0 ){
        ret = psi0gNp - pow(sVal / sCrNp->valueNormal(), 0.50) * (psi0gNp - 1.0);
        if( ret < 1.0 ){
            ret = 1.0;
        }
    }

    psiGNp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiEcNpX(bool emitAuto) {
    double ret = 0.0;

    if( sCrNp->valueNormal() > 0.0 ){
        double den = 1.0 + 2.0 * eNEdX->valueNormal() / sCrNp->valueNormal();
        if( den > 0.0 ) {
            ret = 1.0 / den;
        }
        if( ret > 1.0 ){
            ret = 1.0;
        }
    }

    psiEcNpX->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiEcNpY(bool emitAuto) {
    double ret = 0.0;

    if( sCrNp->valueNormal() > 0.0 ){
        double den = 1.0 + 2.0 * eNEdY->valueNormal() / sCrNp->valueNormal();
        if( den > 0.0 ) {
            ret = 1.0 / den;
        }
    }
    if( ret > 1.0 ){
        ret = 1.0;
    }

    psiEcNpY->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiReNp(bool emitAuto) {
    double ret = 0.0;
    if( internalReinforcement->valueNormal() == Rare ){
        ret = 1.0;
    } else {
        ret = qMin( 0.50 + hEff->valueNormal() / 0.2, 1.0);
    }

    psiReNp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setN0Rkp( bool emitAuto ){
    double ret = 0.0;

    double dVal = d->valueNormal();
    double hEffVal = hEff->valueNormal() * moltNP();

    double tauRkVal = 0.0;
    if( concreteCrack->value() == Cracked ){
        tauRkVal = tauRkCr->valueNormal();
    } else {
        tauRkVal = tauRkUCr->valueNormal();
    }

    ret = M_PI * dVal * hEffVal * tauRkVal;

    N0Rkp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setNRkp( bool emitAuto ){
    if( A0pN->valueNormal() != 0.0 ){
        NRkp->setValueNormal( N0Rkp->valueNormal() * ApN->valueNormal() / A0pN->valueNormal() * psiSNp->valueNormal() * psiGNp->valueNormal() * psiEcNpX->valueNormal() * psiEcNpY->valueNormal() * psiReNp->valueNormal(), emitAuto );
    } else {
        NRkp->setValueNormal( 0.0, emitAuto );
    }
}

void BondedAnchor::setNgRdp( bool emitAuto ){
    NgRdp->setValueNormal( dVal(NRkp->valueNormal(), gammaMpN->valueNormal()), emitAuto );
}

void BondedAnchor::setCCrN(bool emitAuto) {
    double ret = 1.50 * hEff->valueNormal();
    cCrN->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setSCrN(bool emitAuto) {
    double ret = 3.0 * hEff->valueNormal();
    sCrN->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setAcN(bool emitAuto) {
    double molt = moltN();
    double cCrNVal = cCrN->valueNormal() * molt;
    double sCrNVal = sCrN->valueNormal() * molt;

    double ret = 0.0;
    for( unsigned int i=0; i < nY->valueNormal(); ++i ){
        for( unsigned int j=0; j < nX->valueNormal(); ++j ){
            double sDown = 0.0;
            if( i == 0 ) {
                sDown = qMin( cCrNVal, cYMinus->valueNormal() );
            } else {
                sDown = 0.50 * qMin( sCrNVal, sY->valueNormal() );
            }

            double sUp = 0.0;
            if( i == nY->valueNormal() - 1 ) {
                sUp = qMin( cCrNVal, cYPlus->valueNormal() );
            } else {
                sUp = 0.5 * qMin( sCrNVal, sY->valueNormal() );
            }

            double sLeft = 0.0;
            if( j == 0 ) {
                sLeft = qMin( cCrNVal, cXMinus->valueNormal() );
            } else {
                sLeft = 0.5 * qMin( sCrNVal, sX->valueNormal() );
            }

            double sRight = 0.0;
            if( j == nX->valueNormal() ) {
                sRight = qMin( cCrNVal, cXMinus->valueNormal() );
            } else {
                sRight = 0.5 * qMin( sCrNVal, sX->valueNormal() );
            }

            ret += (sLeft + sRight ) * ( sUp + sDown );
        }
    }

    AcN->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setA0cN(bool emitAuto) {
    double sCrNVal = sCrN->valueNormal() * moltN();
    double ret = pow(sCrNVal, 2.0);

    A0cN->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiSN(bool emitAuto) {
    double ret = 0.0;

    if( cCrN->valueNormal() != 0.0 ){
        double cMin = qMin( cXMinus->valueNormal(), cXPlus->valueNormal() );
        cMin = qMin( cMin, cYPlus->valueNormal() );
        cMin = qMin( cMin, cYMinus->valueNormal() );
        ret = qMin( 0.7 + 0.3 * cMin / cCrN->valueNormal(), 1.0);
    }

    psiSN->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiEcNX(bool emitAuto) {
    double ret = 0.0;

    if( sCrN->valueNormal() > 0.0 ){
        double den = 1.0 + 2.0 * eNEdX->valueNormal() / sCrN->valueNormal();
        if( den > 0.0 ) {
            ret = 1.0 / den;
        }
        if( ret > 1.0 ){
            ret = 1.0;
        }
    }

    psiEcNX->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiEcNY(bool emitAuto) {
    double ret = 0.0;

    if( sCrN->valueNormal() > 0.0 ){
        double den = 1.0 + 2.0 * eNEdY->valueNormal() / sCrN->valueNormal();
        if( den > 0.0 ) {
            ret = 1.0 / den;
        }
    }
    if( ret > 1.0 ){
        ret = 1.0;
    }

    psiEcNY->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiReN(bool emitAuto) {
    double ret = 0.0;
    if( internalReinforcement->valueNormal() == Rare ){
        ret = 1.0;
    } else {
        ret = qMin( 0.50 + hEff->valueNormal() / 0.2, 1.0);
    }

    psiReN->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setN0Rkc(bool emitAuto) {
    double ret = 0.0;

    double hEffVal = hEff->valueNormal() * moltN();
    double k1Val = 0.0;
    if( concreteCrack->valueNormal() == Cracked ){
        k1Val = 7.2;
    } else {
        k1Val = 10.1;
    }
    double fckVal = fckCube->valueNormal();
    if( fckVal > 0.0 && hEffVal > 0.0 ){
        ret = k1Val * sqrt( fckVal ) * pow( hEffVal, 1.50  );
    }

    N0Rkc->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setNRkc(bool emitAuto) {
    if( A0cN->valueNormal() != 0.0 ){
        NRkc->setValueNormal( N0Rkc->valueNormal() * AcN->valueNormal() / A0cN->valueNormal() * psiSN->valueNormal() * psiEcNX->valueNormal() * psiEcNY->valueNormal() * psiReN->valueNormal(), emitAuto );
    } else {
        NRkc->setValueNormal( 0.0, emitAuto );
    }
}

void BondedAnchor::setGammaMsN(bool emitAuto) {
    if( fuks->valueNormal() != 0.0 && fyks->valueNormal() != 0.0 ){
        double ratio = fyks->valueNormal() / fuks->valueNormal();
        double val = 1.2 / ratio;
        if( val < 1.4 ){
            val = 1.4;
        }
        gammaMsN->setValueNormal( val, emitAuto );
    } else {
        gammaMsN->setValueNormal( 0.0, emitAuto );
    }
}

void BondedAnchor::setGammaMpN(bool emitAuto) {
    gammaMpN->setValueNormal( gammaC->valueNormal() * gamma2->valueNormal(), emitAuto );
}

void BondedAnchor::setGammaMcN(bool emitAuto) {
    gammaMcN->setValueNormal( gammaC->valueNormal() * gamma2->valueNormal(), emitAuto );
}

void BondedAnchor::setNgRdc(bool emitAuto) {
    NgRdc->setValueNormal( dVal(NRkc->valueNormal(), gammaMcN->valueNormal()), emitAuto );
}

void BondedAnchor::setCCrSp(bool emitAuto) {
    double ret = ratioCCrSpOvEff->valueNormal() * hEff->valueNormal();
    cCrSp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setSCrSp(bool emitAuto) {
    double ret = ratioSCrSpOvEff->valueNormal() * hEff->valueNormal();
    sCrSp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setPsiHSp(bool emitAuto) {
    double ret = 0.0;
    if( hMin->valueNormal() > 0.0 && hEff->valueNormal() > 0.0 ){
        ret = h->valueNormal() / hMin->valueNormal();
        if( ret < 1.0 ){
            ret = 1.0;
        } else {
            double supVal = pow( 2.0 * hEff->valueNormal() / hMin->valueNormal(), 2.0/3.0 );
            if( ret > supVal ){
                ret = supVal;
            }
        }
    }
    psiHSp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setNRkSp(bool emitAuto) {
    if( A0cN->valueNormal() != 0.0 ){
        NRkSp->setValueNormal( N0Rkc->valueNormal() * AcN->valueNormal() / A0cN->valueNormal() * psiSN->valueNormal() * psiEcNX->valueNormal() * psiEcNY->valueNormal() * psiReN->valueNormal() * psiHSp->valueNormal(), emitAuto );
    } else {
        NRkSp->setValueNormal( 0.0, emitAuto );
    }
}

void BondedAnchor::setGammaMSpN(bool emitAuto) {
    gammaMSpN->setValueNormal( gammaC->valueNormal() * gamma2->valueNormal(), emitAuto );
}

void BondedAnchor::setNgRdSp(bool emitAuto) {
    NgRdSp->setValueNormal( dVal(NRkSp->valueNormal(), gammaMSpN->valueNormal()), emitAuto );
}

void BondedAnchor::setMRks(bool emitAuto) {
    double ret = 0.0;
    double NRds = nTot->valueNormal() * NhRds->valueNormal();
    if( NRds != 0.0 ){
        ret = M0Rks->valueNormal() * (1.0 - NEd->valueNormal() / NRds );
    }
    MRks->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVRks(bool emitAuto) {
    double ret = 0.0;
    if( lVEd->valueNormal() == 0.0 ){
        double molt = 1.0;
        if( nTot->valueNormal() > 1 ){
            molt = 0.8;
        }
        ret = molt * 0.5 * As->valueNormal() * fuks->valueNormal();
    } else {
        ret = alphaM->valueNormal() * MRks->valueNormal() / lVEd->valueNormal();
    }
    VRks->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setGammaMcV(bool emitAuto) {
    gammaMcV->setValueNormal( gammaC->valueNormal() * gamma2->valueNormal(), emitAuto );
}

void BondedAnchor::setVgRdCp(bool emitAuto) {
    VgRdCp->setValueNormal( dVal(VRkCp->valueNormal(), gammaMcV->valueNormal()), emitAuto );
}

void BondedAnchor::setK1C(bool emitAuto) {
    double ret = 0.0;
    if( concreteCrack->valueNormal() == Cracked ){
        ret = 1.7;
    } else {
        ret = 2.4;
    }
    k1C->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setA0CVXPlus(bool emitAuto) {
    double ret = A0CVVal( nY->valueNormal(), cXPlus->valueNormal(), cYMinus->valueNormal(), sY->valueNormal(), cYPlus->valueNormal() );
    A0CVXPlus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setACVXPlus(bool emitAuto) {
    double ret = ACVVal( nY->valueNormal(), cXPlus->valueNormal(), cYMinus->valueNormal(), sY->valueNormal(), cYPlus->valueNormal() );
    ACVXPlus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setV0RkCXPlus(bool emitAuto) {
    double ret = V0RkCVal( nY->valueNormal(), cXPlus->valueNormal(), cYMinus->valueNormal(), sY->valueNormal(), cYPlus->valueNormal() );
    V0RkCXPlus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVRkCXPlus(bool emitAuto) {
    double ret = VRkCVal( nY->valueNormal(), V0RkCXPlus->valueNormal(), ACVXPlus->valueNormal(), A0CVXPlus->valueNormal(),
                          cXPlus->valueNormal(), cYMinus->valueNormal(), sY->valueNormal(), cYPlus->valueNormal() );
    VRkCXPlus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVgRdCXPlus(bool emitAuto) {
    VgRdCXPlus->setValueNormal( dVal(VRkCXPlus->valueNormal(), gammaMcV->valueNormal() ), emitAuto );
}

void BondedAnchor::setA0CVXMinus(bool emitAuto) {
    double ret = A0CVVal( nY->valueNormal(), cXMinus->valueNormal(), cYMinus->valueNormal(), sY->valueNormal(), cYPlus->valueNormal() );;
    A0CVXMinus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setACVXMinus(bool emitAuto) {
    double ret = ACVVal( nY->valueNormal(), cXMinus->valueNormal(), cYMinus->valueNormal(), sY->valueNormal(), cYPlus->valueNormal() );;
    ACVXMinus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setV0RkCXMinus(bool emitAuto) {
    double ret = V0RkCVal( nY->valueNormal(), cXMinus->valueNormal(), cYMinus->valueNormal(), sY->valueNormal(), cYPlus->valueNormal() );
    V0RkCXMinus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVRkCXMinus(bool emitAuto) {
    double ret = VRkCVal( nY->valueNormal(), V0RkCXMinus->valueNormal(), ACVXMinus->valueNormal(), A0CVXMinus->valueNormal(),
                          cXMinus->valueNormal(), cYMinus->valueNormal(), sY->valueNormal(), cYPlus->valueNormal() );
    VRkCXMinus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVgRdCXMinus(bool emitAuto) {
    VgRdCXMinus->setValueNormal( dVal(VRkCXMinus->valueNormal(), gammaMcV->valueNormal() ), emitAuto );
}

void BondedAnchor::setA0CVYPlus(bool emitAuto) {
    double ret = A0CVVal(nX->valueNormal(), cYPlus->valueNormal(), cXMinus->valueNormal(), sX->valueNormal(), cXPlus->valueNormal() );
    A0CVYPlus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setACVYPlus(bool emitAuto) {
    double ret = ACVVal(nX->valueNormal(), cYPlus->valueNormal(), cXMinus->valueNormal(), sX->valueNormal(), cXPlus->valueNormal() );
    ACVYPlus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setV0RkCYPlus(bool emitAuto) {
    double ret = V0RkCVal( nX->valueNormal(), cYPlus->valueNormal(), cXMinus->valueNormal(), sX->valueNormal(), cXPlus->valueNormal() );
    V0RkCYPlus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVRkCYPlus(bool emitAuto) {
    double ret = VRkCVal( nX->valueNormal(), V0RkCYPlus->valueNormal(), ACVYPlus->valueNormal(), A0CVYPlus->valueNormal(),
                          cYPlus->valueNormal(), cXMinus->valueNormal(), sX->valueNormal(), cXPlus->valueNormal() );
    VRkCYPlus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVgRdCYPlus(bool emitAuto) {
    VgRdCYPlus->setValueNormal( dVal(VRkCYPlus->valueNormal(), gammaMcV->valueNormal() ), emitAuto );
}

void BondedAnchor::setA0CVYMinus(bool emitAuto) {
    double ret = A0CVVal( nX->valueNormal(), cYMinus->valueNormal(), cXMinus->valueNormal(), sX->valueNormal(), cXPlus->valueNormal() );
    A0CVYMinus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setACVYMinus(bool emitAuto) {
    double ret = ACVVal(nX->valueNormal(), cYMinus->valueNormal(), cXMinus->valueNormal(), sX->valueNormal(), cXPlus->valueNormal() );

    ACVYMinus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setV0RkCYMinus(bool emitAuto) {
    double ret = V0RkCVal( nX->valueNormal(), cYMinus->valueNormal(), cXMinus->valueNormal(), sX->valueNormal(), cXPlus->valueNormal() );

    V0RkCYMinus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVRkCYMinus(bool emitAuto) {
    double ret = VRkCVal( nX->valueNormal(), V0RkCYMinus->valueNormal(), ACVYMinus->valueNormal(), A0CVYMinus->valueNormal(),
                          cYMinus->valueNormal(), cXMinus->valueNormal(), sX->valueNormal(), cXPlus->valueNormal() );
    VRkCYMinus->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVgRdCYMinus(bool emitAuto) {
    VgRdCYMinus->setValueNormal( dVal(VRkCYMinus->valueNormal(), gammaMcV->valueNormal() ), emitAuto );
}

double BondedAnchor::A0CVVal( unsigned int nVal, double c1ValInput, double c21ValInput, double s2ValInput, double c22ValInput) {
    double ret = 0.0;

    if( nVal < 1 ){
        return ret;
    }

    double c1Val = c1VEffVal( nVal, c1ValInput, c21ValInput, s2ValInput, c22ValInput);
    if( c1Val <= 0.0 ) {
        return ret;
    }

    ret = nVal * 4.5 * pow( c1Val, 2.0 );

    return ret;
}

double BondedAnchor::ACVVal( unsigned int nVal, double c1ValInput, double c21ValInput, double s2ValInput, double c22ValInput) {
    double ret = 0.0;

    if( (nVal < 1) || (h->valueNormal() <= 0.0) ){
        return ret;
    }

    double c1Val = c1VEffVal( nVal, c1ValInput, c21ValInput, s2ValInput, c22ValInput);
    if( c1Val <= 0.0 ) {
        return ret;
    }

    // altezza resistente
    double hVal = qMin( 1.5 * c1Val, h->valueNormal() );

    double cCrVal = 1.5 * c1Val;
    double sCrVal = 3.0 * c1Val;

    for( unsigned int i=0; i < nVal; ++i ){
        double sDown = 0.0;
        if( i == 0 ) {
            sDown = qMin( cCrVal, c21ValInput );
        } else {
            sDown = 0.5 * qMin( sCrVal, s2ValInput );
        }

        double sUp = 0.0;
        if( i == nY->valueNormal() - 1 ) {
            sUp = qMin( cCrVal, c22ValInput );
        } else {
            sUp = 0.5 * qMin( sCrVal, s2ValInput );
        }

        ret += (sUp + sDown ) * hVal;
    }

    return ret;
}

double BondedAnchor::V0RkCVal( unsigned int nVal, double c1ValInput, double c21ValInput, double s2ValInput, double c22ValInput) {
    double ret = 0.0;

    if( c1ValInput <= 0.0 || fckCube->valueNormal() <= 0.0 || h->valueNormal() <= 0.0 ){
        return ret;
    }

    double c1Val = c1VEffVal( nVal, c1ValInput, c21ValInput, s2ValInput, c22ValInput);
    if( c1Val <= 0.0 ) {
        return ret;
    }
    double c2MaxVal = qMax( c21ValInput, c22ValInput );
    if( c2MaxVal <= 0.0 ) {
        return ret;
    }

    if( c2MaxVal < 1.5 * c1Val && h->valueNormal() <= 1.5 * c1Val){
        c1Val = qMax ( c2MaxVal/1.5, s2ValInput/ 3.0);
        c1Val = qMax( c1Val, h->valueNormal() / 1.5 );
    }

    double D = hEff->valueNormal() / c1Val;
    if( D < 0.0 ) {
        return ret;
    }
    double alphaVal = 0.1 * pow( D, 0.5 );
    D = d->valueNormal() / c1Val;
    if( D < 0.0 ) {
        return ret;
    }
    double betaVal = 0.1 * pow( D, 0.2 );

    ret = k1C->valueNormal() * \
            pow( d->valueNormal()*1.0e+3, alphaVal ) * \
            pow( hEff->valueNormal()*1.0e+3, betaVal ) * \
            sqrt(fckCube->valueNormal() * 1.0e-6) * \
            pow( c1Val*1.0e+3, 1.5);


    return ret;
}

double BondedAnchor::VRkCVal( unsigned int nVal, double V0RkCVal, double ACVVal, double A0CVVal,
                              double c1ValInput, double c21ValInput, double s2ValInput, double c22ValInput) {
    double ret = 0.0;

    if( A0CVVal <= 0.0 || ACVVal <= 0.0 ||
            c1ValInput <= 0.0 || c22ValInput < 0.0  || c22ValInput < 0.0 ||
            h->valueNormal() <= 0.0 ){
        return ret;
    }

    double c1Val = c1VEffVal( nVal, c1ValInput, c21ValInput, s2ValInput, c22ValInput);
    if( c1Val <= 0.0 ) {
        return ret;
    }
    double c2Val = qMin( c21ValInput, c22ValInput );
    if( c2Val <= 0.0 ) {
        return ret;
    }

    double psiSVVal = qMin( 0.7 * 0.3 * c2Val / c1Val, 1.0);
    double psiHVVal = qMin( pow(1.5 * c1Val / h->valueNormal(), 0.5) , 1.0 );
    double psiAlphaVVal = 1.0;
    double psiEcVVal = 1.0;
    double psiReVVal = 1.0;
    if( edgeReinforcement->valueNormal() == EdgeReinforced ){
        psiReVVal = 1.2;
    } else if( edgeReinforcement->valueNormal() == EdgeReinforcedStirrups ){
        psiReVVal = 1.4;
    }

    ret = V0RkCVal * ACVVal / A0CVVal * psiSVVal * psiHVVal * psiAlphaVVal * psiEcVVal * psiReVVal;

    return ret;
}

double BondedAnchor::dVal(double kVal, double gamma) {
    double ret = 0.0;
    if( gamma != 0.0 ){
        ret = kVal / gamma;
    }
    return ret;
}

double BondedAnchor::c1VEffVal( unsigned int nVal, double c1ValInput, double c21ValInput, double s2ValInput, double c22ValInput ) {
    double c1Val = c1ValInput;
    double c2MaxVal = qMax( c21ValInput, c22ValInput );
    if( c2MaxVal < 1.5 * c1Val && h->valueNormal() <= 1.5 * c1Val){
        c1Val = qMax( c2MaxVal/1.5, h->valueNormal() / 1.5 );
        if( nVal > 1 ) {
            c1Val = qMax ( c1Val, s2ValInput/ 3.0);
        }
    }
    return c1Val;
}

void BondedAnchor::setGammaMsV(bool emitAuto) {
    if( fuks->valueNormal() != 0.0 && fyks->valueNormal() != 0.0 ){
        double ratio = fyks->valueNormal() / fuks->valueNormal();
        gammaMsV->setValueNormal( 0.0 );
        if( fuks->valueNormal() <= 8.0e+8 && ratio <= 0.8 ){
            double val = 1.0 / ratio;
            if( val < 1.25 ){
                val = 1.25;
            }
            gammaMsV->setValueNormal( val, emitAuto );
        } else {
            gammaMsV->setValueNormal( 1.5, emitAuto );
        }
    } else {
        gammaMsV->setValueNormal( 0.0, emitAuto );
    }
}

void BondedAnchor::setVhRds(bool emitAuto) {
    VhRds->setValueNormal( dVal( VRks->valueNormal(), gammaMsV->valueNormal()), emitAuto );
}

void BondedAnchor::setKCp(bool emitAuto) {
    double ret = 0.0;
    if( hEff->valueNormal() < 0.06 ){
        ret = 1.0;
    } else {
        ret = 2.0;
    }
    kCp->setValueNormal( ret, emitAuto );
}

void BondedAnchor::setVRkCp(bool emitAuto) {
    double ret = qMin( kCp->valueNormal() * NRkp->valueNormal(), kCp->valueNormal() * NRkc->valueNormal());
    VRkCp->setValueNormal( ret, emitAuto );
}

#include <QCoreApplication>

class BAnchorConcreteEnumVal{
public:
    BAnchorConcreteEnumVal(BondedAnchor::Concrete v, const QString & ns, const QString & s):
        val(v),
        normalStr(ns),
        str(s){
    }
    BondedAnchor::Concrete val;
    QString normalStr;
    QString str;
};

class BAnchorConcretePrivate{
    Q_DECLARE_TR_FUNCTIONS(ConcreteCrackPlusPrivate)
public:
    BAnchorConcretePrivate(BondedAnchor::Concrete v):
        value(v){
        enumList.append( BAnchorConcreteEnumVal( BondedAnchor::Cracked, "Cracked", trUtf8("Fessurato")) );
        enumList.append( BAnchorConcreteEnumVal( BondedAnchor::NotCracked, "NotCracked", trUtf8("Non fessurato")) );
    }
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    BondedAnchor::Concrete value;
    QList<BAnchorConcreteEnumVal> enumList;
};

BAnchorConcrete::BAnchorConcrete( BondedAnchor::Concrete vv,
                                  const QString & nn,
                                  bool ro):
    EnumPlus( nn, ro ),
    m_d( new  BAnchorConcretePrivate(vv) ){
}

BAnchorConcrete::BAnchorConcrete( BAnchorConcrete & val ):
    EnumPlus(val),
    m_d( new  BAnchorConcretePrivate(val.value()) ) {
    m_d->value = val.m_d->value;
}

BAnchorConcrete::~BAnchorConcrete(){
    delete m_d;
}

QString BAnchorConcrete::typeVar(){
    return QString("SteelCncrModelULS");
}

BAnchorConcrete & BAnchorConcrete::operator=( BAnchorConcrete & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & BAnchorConcrete::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        BAnchorConcrete * valSteelCncrModelPlus = dynamic_cast<BAnchorConcrete *>(&val);
        if(valSteelCncrModelPlus) {
            setValueNormal( valSteelCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

BondedAnchor::Concrete BAnchorConcrete::value(){
    return m_d->value;
}

BondedAnchor::Concrete BAnchorConcrete::valueNormal(){
    return m_d->value;
}

QString BAnchorConcrete::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString BAnchorConcrete::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void BAnchorConcrete::setValueNormal( BondedAnchor::Concrete vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void BAnchorConcrete::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void BAnchorConcrete::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void BAnchorConcrete::setValue( BondedAnchor::Concrete vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void BAnchorConcrete::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int BAnchorConcrete::enumCount() {
    return m_d->enumList.size();
}

QString BAnchorConcrete::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int BAnchorConcrete::valueNormalInt() {
    return m_d->valueIndex();
}



class BAnchorInternalReinforcementEnumVal{
public:
    BAnchorInternalReinforcementEnumVal(BondedAnchor::InternalReinforcement v, const QString & ns, const QString & s):
        val(v),
        normalStr(ns),
        str(s){
    }
    BondedAnchor::InternalReinforcement val;
    QString normalStr;
    QString str;
};

class BAnchorInternalReinforcementPrivate{
    Q_DECLARE_TR_FUNCTIONS(ConcreteCrackPlusPrivate)
public:
    BAnchorInternalReinforcementPrivate(BondedAnchor::InternalReinforcement v):
        value(v){
        enumList.append( BAnchorInternalReinforcementEnumVal( BondedAnchor::Rare, "Rare", trUtf8("Normale")) );
        enumList.append( BAnchorInternalReinforcementEnumVal( BondedAnchor::Dense, "Dense", trUtf8("Fitta")) );
    }
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    BondedAnchor::InternalReinforcement value;
    QList<BAnchorInternalReinforcementEnumVal> enumList;
};

BAnchorInternalReinforcement::BAnchorInternalReinforcement( BondedAnchor::InternalReinforcement vv,
                                                            const QString & nn,
                                                            bool ro):
    EnumPlus( nn, ro ),
    m_d( new  BAnchorInternalReinforcementPrivate(vv) ){
}

BAnchorInternalReinforcement::BAnchorInternalReinforcement( BAnchorInternalReinforcement & val ):
    EnumPlus(val),
    m_d( new  BAnchorInternalReinforcementPrivate(val.value()) ) {
    m_d->value = val.m_d->value;
}

BAnchorInternalReinforcement::~BAnchorInternalReinforcement(){
    delete m_d;
}

QString BAnchorInternalReinforcement::typeVar(){
    return QString("SteelCncrModelULS");
}

BAnchorInternalReinforcement & BAnchorInternalReinforcement::operator=( BAnchorInternalReinforcement & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & BAnchorInternalReinforcement::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        BAnchorInternalReinforcement * valSteelCncrModelPlus = dynamic_cast<BAnchorInternalReinforcement *>(&val);
        if(valSteelCncrModelPlus) {
            setValueNormal( valSteelCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

BondedAnchor::InternalReinforcement BAnchorInternalReinforcement::value(){
    return m_d->value;
}

BondedAnchor::InternalReinforcement BAnchorInternalReinforcement::valueNormal(){
    return m_d->value;
}

QString BAnchorInternalReinforcement::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString BAnchorInternalReinforcement::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void BAnchorInternalReinforcement::setValueNormal( BondedAnchor::InternalReinforcement vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void BAnchorInternalReinforcement::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void BAnchorInternalReinforcement::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void BAnchorInternalReinforcement::setValue( BondedAnchor::InternalReinforcement vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void BAnchorInternalReinforcement::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int BAnchorInternalReinforcement::enumCount() {
    return m_d->enumList.size();
}

QString BAnchorInternalReinforcement::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int BAnchorInternalReinforcement::valueNormalInt() {
    return m_d->valueIndex();
}






class BAnchorEdgeReinforcementEnumVal{
public:
    BAnchorEdgeReinforcementEnumVal(BondedAnchor::EdgeReinforcement v, const QString & ns, const QString & s):
        val(v),
        normalStr(ns),
        str(s){
    }
    BondedAnchor::EdgeReinforcement val;
    QString normalStr;
    QString str;
};

class BAnchorEdgeReinforcementPrivate{
    Q_DECLARE_TR_FUNCTIONS(ConcreteCrackPlusPrivate)
public:
    BAnchorEdgeReinforcementPrivate(BondedAnchor::EdgeReinforcement v):
        value(v){
        enumList.append( BAnchorEdgeReinforcementEnumVal( BondedAnchor::EdgeNotReinforced, "NotReinforced", trUtf8("Nessun rinforzo")) );
        enumList.append( BAnchorEdgeReinforcementEnumVal( BondedAnchor::EdgeReinforced, "Reinforced", trUtf8("Spigolo rinforzato")) );
        enumList.append( BAnchorEdgeReinforcementEnumVal( BondedAnchor::EdgeReinforcedStirrups, "ReinforcedStirrups", trUtf8("Spigolo rinforzato - staffatura fitta")) );
    }
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    BondedAnchor::EdgeReinforcement value;
    QList<BAnchorEdgeReinforcementEnumVal> enumList;
};

BAnchorEdgeReinforcement::BAnchorEdgeReinforcement( BondedAnchor::EdgeReinforcement vv,
                                                    const QString & nn,
                                                    bool ro):
    EnumPlus( nn, ro ),
    m_d( new  BAnchorEdgeReinforcementPrivate(vv) ){
}

BAnchorEdgeReinforcement::BAnchorEdgeReinforcement( BAnchorEdgeReinforcement & val ):
    EnumPlus(val),
    m_d( new  BAnchorEdgeReinforcementPrivate(val.value()) ) {
    m_d->value = val.m_d->value;
}

BAnchorEdgeReinforcement::~BAnchorEdgeReinforcement(){
    delete m_d;
}

QString BAnchorEdgeReinforcement::typeVar(){
    return QString("SteelCncrModelULS");
}

BAnchorEdgeReinforcement & BAnchorEdgeReinforcement::operator=( BAnchorEdgeReinforcement & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & BAnchorEdgeReinforcement::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        BAnchorEdgeReinforcement * valSteelCncrModelPlus = dynamic_cast<BAnchorEdgeReinforcement *>(&val);
        if(valSteelCncrModelPlus) {
            setValueNormal( valSteelCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

BondedAnchor::EdgeReinforcement BAnchorEdgeReinforcement::value(){
    return m_d->value;
}

BondedAnchor::EdgeReinforcement BAnchorEdgeReinforcement::valueNormal(){
    return m_d->value;
}

QString BAnchorEdgeReinforcement::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString BAnchorEdgeReinforcement::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void BAnchorEdgeReinforcement::setValueNormal( BondedAnchor::EdgeReinforcement vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void BAnchorEdgeReinforcement::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void BAnchorEdgeReinforcement::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void BAnchorEdgeReinforcement::setValue( BondedAnchor::EdgeReinforcement vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void BAnchorEdgeReinforcement::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int BAnchorEdgeReinforcement::enumCount() {
    return m_d->enumList.size();
}

QString BAnchorEdgeReinforcement::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int BAnchorEdgeReinforcement::valueNormalInt() {
    return m_d->valueIndex();
}
