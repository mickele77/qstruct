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

#include "retainingwallqscombinationgui.h"
#include "ui_retainingwallqscombinationgui.h"
#include "retainingwallqscombinationguiprivate.h"

#include "varplusguiutility.h"

#include "retainingwallgui.h"
#include "retainingwall.h"
#include "doubleplus.h"
#include "point2dplus.h"
#include "boolplus.h"

#include <QString>
#include <QMimeData>
#include <QClipboard>

#include <cmath>

RetainingWallQSCombinationGUIPrivate::RetainingWallQSCombinationGUIPrivate( UnitMeasure * ump, const QString & cn,
                                                                            DoublePlus *gk2, DoublePlus *qk,
                                                                            DoublePlus * gTanPhiPrime, DoublePlus * gCPrime,
                                                                            DoublePlus *gG1Min, DoublePlus *gG1Max,
                                                                            DoublePlus *gG2Min, DoublePlus *gG2Max,
                                                                            DoublePlus *gQMin, DoublePlus *gQMax,
                                                                            DoublePlus *gSliding, DoublePlus *gQLim  ):
    VarPlusContainer(ump, "", ""),
    retainingWall(NULL),
    combinationName(cn),
    Gk2(gk2),
    Qk(qk),
    gammaTanPhiPrime(gTanPhiPrime),
    gammaCPrime(gCPrime),
    gammaG1Min(gG1Min),
    gammaG1Max(gG1Max),
    gammaG2Min(gG2Min),
    gammaG2Max(gG2Max),
    gammaQMin(gQMin),
    gammaQMax(gQMax),
    gammaSliding(gSliding),
    gammaQLim(gQLim){

    initVar();
}

void RetainingWallQSCombinationGUIPrivate::initVar(){
    QLimBPrime = new DoublePlus(0.0, "QLimBPrime", m_unitMeasure, UnitMeasure::length );
    QLimBPrime->setRichName( QObject::trUtf8("B'"));
    QLimBPrime->setToolTip( combinationName + " - " + QObject::trUtf8("Base equivalente della fondazione"));
    QLimBPrime->setReadOnly( true );
    addVarToContainer( QLimBPrime );

    QLimLPrime = new DoublePlus(0.0, "QLimLPrime", m_unitMeasure, UnitMeasure::length );
    QLimLPrime->setRichName( QObject::trUtf8("L'"));
    QLimLPrime->setToolTip( combinationName + " - " + QObject::trUtf8("Lunghezza equivalente della fondazione"));
    QLimLPrime->setReadOnly( true );
    addVarToContainer( QLimLPrime );

    QLimD = new DoublePlus(0.0, "QLimD", m_unitMeasure, UnitMeasure::length );
    QLimD->setRichName( QObject::trUtf8("D"));
    QLimD->setToolTip( combinationName + " - " + QObject::trUtf8("Approfondimento fondazione"));
    QLimD->setReadOnly( true );
    addVarToContainer( QLimD );

    QLimLInfinite = new BoolPlus(true, "QLimLInfinite" );
    QLimLInfinite->setRichName( QObject::trUtf8("Lunghezza infinita"));
    QLimLInfinite->setToolTip( combinationName + " - " + QObject::trUtf8("Considera la fondazione di lunghezza infinita nel calcolo della formula di Brinch-Hansen"));
    addVarToContainer( QLimLInfinite );

    SoilDownPhiPrimeD = new DoublePlus(0.0, "QSSoilDownPhiPrimeD", m_unitMeasure, UnitMeasure::angle );
    SoilDownPhiPrimeD->setRichName( QObject::trUtf8("φ'<span style=\" vertical-align:sub;\">monte,d</span>"));
    SoilDownPhiPrimeD->setToolTip( combinationName + " - " + QObject::trUtf8("Angolo di attrito interno di calcolo del terreno a valle"));
    SoilDownPhiPrimeD->setReadOnly( true );
    addVarToContainer( SoilDownPhiPrimeD );

    SoilDownCPrimeD = new DoublePlus(0.0, "QSSoilDownCPrimeD", m_unitMeasure, UnitMeasure::tension );
    SoilDownCPrimeD->setRichName( QObject::trUtf8("c'<span style=\" vertical-align:sub;\">valle,d</span>"));
    SoilDownCPrimeD->setToolTip( combinationName + QObject::trUtf8(" - Coesione di calcolo del terreno a valle"));
    SoilDownCPrimeD->setReadOnly( true );
    addVarToContainer( SoilDownCPrimeD );

    SoilDownDeltaD = new DoublePlus(0.0, "QSSoilDownDeltaD", m_unitMeasure, UnitMeasure::angle );
    SoilDownDeltaD->setRichName( QObject::trUtf8("δ<span style=\" vertical-align:sub;\">valle,d</span>"));
    SoilDownDeltaD->setToolTip( combinationName + " - " + QObject::trUtf8("Angolo di attrito fondazione muro-terreno "));
    SoilDownDeltaD->setReadOnly( true );
    addVarToContainer( SoilDownDeltaD );

    SoilUpPhiPrimeD = new DoublePlus(0.0, "QSSoilUpPhiPrimeD", m_unitMeasure, UnitMeasure::angle );
    SoilUpPhiPrimeD->setRichName( QObject::trUtf8("φ'<span style=\" vertical-align:sub;\">monte,d</span>"));
    SoilUpPhiPrimeD->setToolTip( combinationName + " - " + QObject::trUtf8("Angolo di attrito interno di calcolo del terreno a monte"));
    SoilUpPhiPrimeD->setReadOnly( true );
    addVarToContainer( SoilUpPhiPrimeD );

    SoilUpCPrimeD = new DoublePlus(0.0, "QSSoilUpCPrimeD", m_unitMeasure, UnitMeasure::tension );
    SoilUpCPrimeD->setRichName( QObject::trUtf8("c'<span style=\" vertical-align:sub;\">monte,d</span>"));
    SoilUpCPrimeD->setToolTip( combinationName + " - " + QObject::trUtf8("Coesione di calcolo del terreno a monte"));
    SoilUpCPrimeD->setReadOnly( true );
    addVarToContainer( SoilUpCPrimeD );

    SoilUpDeltaD = new DoublePlus(0.0, "QSSoilUpDeltaD", m_unitMeasure, UnitMeasure::angle );
    SoilUpDeltaD->setRichName( QObject::trUtf8("δ<span style=\" vertical-align:sub;\">monte,d</span>"));
    SoilUpDeltaD->setToolTip( combinationName + " - " + QObject::trUtf8("Angolo di attrito muro-terreno "));
    SoilUpDeltaD->setReadOnly( false );
    addVarToContainer( SoilUpDeltaD );

    Ka = new DoublePlus(0.0, "QSKa", m_unitMeasure, UnitMeasure::noDimension );
    Ka->setRichName( QObject::trUtf8("K<span style=\" vertical-align:sub;\">a</span>"));
    Ka->setToolTip( combinationName + " - " + QObject::trUtf8("Coefficiente spinta attiva"));
    Ka->setReadOnly( true );
    addVarToContainer( Ka );

    Kp = new DoublePlus(0.0, "QSKp", m_unitMeasure, UnitMeasure::noDimension );
    Kp->setRichName( QObject::trUtf8("K<span style=\" vertical-align:sub;\">p</span>"));
    Kp->setToolTip( combinationName + " - " + QObject::trUtf8("Coefficiente spinta passiva"));
    Kp->setReadOnly( true );
    addVarToContainer( Kp );

    ChiKp = new DoublePlus(0.5, "QSChiKp", m_unitMeasure, UnitMeasure::noDimension );
    ChiKp->setRichName( QObject::trUtf8("χ<span style=\" vertical-align:sub;\">kp</span>"));
    ChiKp->setToolTip( combinationName + " - " + QObject::trUtf8("Fattore riduttivo coefficiente spinta passiva"));
    ChiKp->setReadOnly( false );
    addVarToContainer( ChiKp );

    KpRed = new DoublePlus(0.0, "QSKpD", m_unitMeasure, UnitMeasure::noDimension );
    KpRed->setRichName( QObject::trUtf8("K<span style=\" vertical-align:sub;\">p,d</span>"));
    KpRed->setToolTip( combinationName + " - " + QObject::trUtf8("Coefficiente spinta passiva in condizioni di progetto"));
    KpRed->setReadOnly( true );
    addVarToContainer( KpRed );

    SlidingVEd = new DoublePlus(0.0, "SlidingVEd", m_unitMeasure, UnitMeasure::loadF );
    SlidingVEd->setRichName( QObject::trUtf8("V<span style=\" vertical-align:sub;\">Ed</span>"));
    SlidingVEd->setToolTip( combinationName + " - " + QObject::trUtf8("Forza verticale agente"));
    SlidingVEd->setReadOnly( true );
    addVarToContainer( SlidingVEd );

    SlidingHEd = new DoublePlus(0.0, "SlidingHEd", m_unitMeasure, UnitMeasure::loadF );
    SlidingHEd->setRichName( QObject::trUtf8("H<span style=\" vertical-align:sub;\">Ed</span>"));
    SlidingHEd->setToolTip( combinationName + " - " + QObject::trUtf8("Forza orizzontale agente dovuta alla spinta attiva"));
    SlidingHEd->setReadOnly( true );
    addVarToContainer( SlidingHEd );

    SlidingHRdP = new DoublePlus(0.0, "SlidingHRdP", m_unitMeasure, UnitMeasure::loadF );
    SlidingHRdP->setRichName( QObject::trUtf8("H<span style=\" vertical-align:sub;\">Rd,P</span>"));
    SlidingHRdP->setToolTip( combinationName + " - " + QObject::trUtf8("Forza orizzontale resistente dovuta alla spinta passiva"));
    SlidingHRdP->setReadOnly( true );
    addVarToContainer( SlidingHRdP );

    SlidingApplyKp = new BoolPlus( false, "SlidingApplyKp" );
    SlidingApplyKp->setRichName( QObject::trUtf8("Resistenza passiva contro fondazione"));
    SlidingApplyKp->setToolTip( combinationName + " - " + QObject::trUtf8("Se si tiene conto o meno del contributo della resistenza passiva nella verifica a scorrimento"));
    SlidingApplyKp->setReadOnly( false );
    addVarToContainer( SlidingApplyKp );

    SlidingHRd = new DoublePlus(0.0, "QSSlidingHRd", m_unitMeasure, UnitMeasure::loadF, true );
    SlidingHRd->setRichName( QObject::trUtf8("H<span style=\" vertical-align:sub;\">Rd</span>"));
    SlidingHRd->setToolTip( combinationName + " - " + QObject::trUtf8("Verifica a scorrimento - Forza orizzontale resistente"));
    addVarToContainer( SlidingHRd );

    QLimVEd = new DoublePlus(0.0, "QLimVEd", m_unitMeasure, UnitMeasure::loadF );
    QLimVEd->setRichName( QObject::trUtf8("V<span style=\" vertical-align:sub;\">Ed</span>"));
    QLimVEd->setToolTip( combinationName + " - " + QObject::trUtf8("Forza verticale agente"));
    QLimVEd->setReadOnly( true );
    addVarToContainer( QLimVEd );

    QLimApplyKp = new BoolPlus( false, "QLimApplyKp" );
    QLimApplyKp->setRichName( QObject::trUtf8("Resistenza passiva contro fondazione"));
    QLimApplyKp->setToolTip( combinationName + " - " + QObject::trUtf8("Se si tiene conto o meno del contributo della resistenza passiva nella verifica della capacitÃ  portante"));
    QLimApplyKp->setReadOnly( false );
    addVarToContainer( QLimApplyKp );

    QLimHEd = new DoublePlus(0.0, "QLimHEd", m_unitMeasure, UnitMeasure::loadF );
    QLimHEd->setRichName( QObject::trUtf8("H<span style=\" vertical-align:sub;\">Ed</span>"));
    QLimHEd->setToolTip( combinationName + " - " + QObject::trUtf8("Forza orizzontale agente nella verifica della capacitÃ  portante"));
    QLimHEd->setReadOnly( true );
    addVarToContainer( QLimHEd );

    QLimHRdPMax = new DoublePlus(0.0, "QLimHRdPMax", m_unitMeasure, UnitMeasure::loadF );
    QLimHRdPMax->setRichName( QObject::trUtf8("H<span style=\" vertical-align:sub;\">Rd,P,max</span>"));
    QLimHRdPMax->setToolTip( combinationName + " - " + QObject::trUtf8("Spinta passiva massima disponibile nella verifica della capacitÃ  portante"));
    QLimHRdPMax->setReadOnly( true );
    addVarToContainer( QLimHRdPMax );

    QLimHRdP = new DoublePlus(0.0, "QLimHRdP", m_unitMeasure, UnitMeasure::loadF );
    QLimHRdP->setRichName( QObject::trUtf8("H<span style=\" vertical-align:sub;\">Rd,P,eff</span>"));
    QLimHRdP->setToolTip( combinationName + " - " + QObject::trUtf8("Spinta passiva mobilitata nella verifica della capacitÃ  portante"));
    QLimHRdP->setReadOnly( true );
    addVarToContainer( QLimHRdP );

    QLimMEdC = new Point2DPlus("QLimMEdC", "y", 0.0, "z", 0.0, m_unitMeasure, UnitMeasure::length );
    QLimMEdC->y->setRichName( QObject::trUtf8("y<span style=\" vertical-align:sub;\">C</span>"));
    QLimMEdC->y->setToolTip( combinationName + " - " + QObject::trUtf8("Ascissa polo per calcolo del momento nella verifica della capacitÃ  portante"));
    QLimMEdC->y->setReadOnly( true );
    QLimMEdC->z->setRichName( QObject::trUtf8("z<span style=\" vertical-align:sub;\">C</span>"));
    QLimMEdC->z->setToolTip( combinationName + " - " + QObject::trUtf8("Ordinata polo per calcolo del momento nella verifica della capacitÃ  portante"));
    QLimMEdC->z->setReadOnly( true );
    addVarToContainer( QLimMEdC );

    QLimMEd = new DoublePlus(0.0, "QLimMEd", m_unitMeasure, UnitMeasure::loadM );
    QLimMEd->setRichName( QObject::trUtf8("M<span style=\" vertical-align:sub;\">Ed</span>"));
    QLimMEd->setToolTip( combinationName + " - " + QObject::trUtf8("Momento agente nella verifica della capacitÃ  portante"));
    QLimMEd->setReadOnly( true );
    addVarToContainer( QLimMEd );

    QLimSoilDownGammaWPrimeD = new DoublePlus(0.0, "QLimSoilDownGammaWPrimeD", m_unitMeasure, UnitMeasure::loadFVolume );
    QLimSoilDownGammaWPrimeD->setRichName( QObject::trUtf8("γ'<span style=\" vertical-align:sub;\">d</span>"));
    QLimSoilDownGammaWPrimeD->setToolTip( combinationName + " - " + QObject::trUtf8("Peso specifico di calcolo del terreno a valle nella verifica della capacitÃ  portante"));
    QLimSoilDownGammaWPrimeD->setReadOnly( true );
    addVarToContainer( QLimSoilDownGammaWPrimeD );

    QLimQEd = new DoublePlus(0.0, "QLimQEd", m_unitMeasure, UnitMeasure::loadFArea );
    QLimQEd->setRichName( QObject::trUtf8("q<span style=\" vertical-align:sub;\">Ed</span>"));
    QLimQEd->setToolTip( combinationName + " - " + QObject::trUtf8("Carico distribuito nella verifica della capacitÃ  portante"));
    QLimQEd->setReadOnly( true );
    addVarToContainer( QLimQEd );

    QLimNGammaW = new DoublePlus(0.0, "QLimNGammaW", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimNGammaW->setRichName( QObject::trUtf8("N<span style=\" vertical-align:sub;\">γ</span>"));
    QLimNGammaW->setReadOnly( true );
    addVarToContainer( QLimNGammaW );

    QLimSGammaW = new DoublePlus(1.0, "QLimSGammaW", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimSGammaW->setRichName( QObject::trUtf8("s<span style=\" vertical-align:sub;\">γ</span>"));
    QLimSGammaW->setReadOnly( true );
    addVarToContainer( QLimSGammaW );

    QLimIGammaW = new DoublePlus(1.0, "QLimIGammaW", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimIGammaW->setRichName( QObject::trUtf8("i<span style=\" vertical-align:sub;\">γ</span>"));
    QLimIGammaW->setReadOnly( true );
    addVarToContainer( QLimIGammaW );

    QLimBGammaW = new DoublePlus(1.0, "QLimBGammaW", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimBGammaW->setRichName( QObject::trUtf8("b<span style=\" vertical-align:sub;\">γ</span>"));
    QLimBGammaW->setReadOnly( true );
    addVarToContainer( QLimBGammaW );

    QLimGGammaW = new DoublePlus(1.0, "QLimGGammaW", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimGGammaW->setRichName( QObject::trUtf8("g<span style=\" vertical-align:sub;\">γ</span>"));
    QLimGGammaW->setReadOnly( true );
    addVarToContainer( QLimGGammaW );

    QLimNC = new DoublePlus(0.0, "QLimNC", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimNC->setRichName( QObject::trUtf8("N<span style=\" vertical-align:sub;\">c</span>"));
    QLimNC->setReadOnly( true );
    addVarToContainer( QLimNC );

    QLimSC = new DoublePlus(1.0, "QLimSC", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimSC->setRichName( QObject::trUtf8("s<span style=\" vertical-align:sub;\">c</span>"));
    QLimSC->setReadOnly( true );
    addVarToContainer( QLimSC );

    QLimIC = new DoublePlus(1.0, "QLimIC", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimIC->setRichName( QObject::trUtf8("i<span style=\" vertical-align:sub;\">c</span>"));
    QLimIC->setReadOnly( true );
    addVarToContainer( QLimIC );

    QLimBC = new DoublePlus(1.0, "QLimBC", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimBC->setRichName( QObject::trUtf8("b<span style=\" vertical-align:sub;\">c</span>"));
    QLimBC->setReadOnly( true );
    addVarToContainer( QLimBC );

    QLimGC = new DoublePlus(1.0, "QLimGC", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimGC->setRichName( QObject::trUtf8("g<span style=\" vertical-align:sub;\">c</span>"));
    QLimGC->setReadOnly( true );
    addVarToContainer( QLimGC );

    QLimDC = new DoublePlus(1.0, "QLimDC", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimDC->setRichName( QObject::trUtf8("d<span style=\" vertical-align:sub;\">c</span>"));
    QLimDC->setReadOnly( true );
    addVarToContainer( QLimDC );

    QLimNQ = new DoublePlus(0.0, "QLimNQ", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimNQ->setRichName( QObject::trUtf8("N<span style=\" vertical-align:sub;\">q</span>"));
    QLimNQ->setReadOnly( true );
    addVarToContainer( QLimNQ );

    QLimSQ = new DoublePlus(1.0, "QLimSQ", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimSQ->setRichName( QObject::trUtf8("s<span style=\" vertical-align:sub;\">q</span>"));
    QLimSQ->setReadOnly( true );
    addVarToContainer( QLimSQ );

    QLimIQ = new DoublePlus(1.0, "QLimIQ", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimIQ->setRichName( QObject::trUtf8("i<span style=\" vertical-align:sub;\">q</span>"));
    QLimIQ->setReadOnly( true );
    addVarToContainer( QLimIQ );

    QLimBQ = new DoublePlus(1.0, "QLimBQ", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimBQ->setRichName( QObject::trUtf8("b<span style=\" vertical-align:sub;\">q</span>"));
    QLimBQ->setReadOnly( true );
    addVarToContainer( QLimBQ );

    QLimGQ = new DoublePlus(1.0, "QLimGQ", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimGQ->setRichName( QObject::trUtf8("g<span style=\" vertical-align:sub;\">q</span>"));
    QLimGQ->setReadOnly( true );
    addVarToContainer( QLimGQ );

    QLimDQ = new DoublePlus(1.0, "QLimDQ", m_unitMeasure, UnitMeasure::noDimension, true );
    QLimDQ->setRichName( QObject::trUtf8("d<span style=\" vertical-align:sub;\">q</span>"));
    QLimDQ->setReadOnly( true );
    addVarToContainer( QLimDQ );

    qLimD = new DoublePlus(0.0, "qLimD", m_unitMeasure, UnitMeasure::loadFArea, true );
    qLimD->setRichName( QObject::trUtf8("q<span style=\" vertical-align:sub;\">lim,d</span>"));
    qLimD->setToolTip( QObject::trUtf8("Pressione limite al contatto tra fondazione e terreno (formula di Brinch-Hansen)"));
    qLimD->setReadOnly( true );
    addVarToContainer( qLimD );

    QLimVRd = new DoublePlus(0.0, "QLimVRd", m_unitMeasure, UnitMeasure::loadF, true );
    QLimVRd->setRichName( QObject::trUtf8("V<span style=\" vertical-align:sub;\">Rd</span>"));
    QLimVRd->setToolTip( QObject::trUtf8("Carico massimo che può essere trasmesso dalla fondazione al terreno"));
    QLimVRd->setReadOnly( true );
    addVarToContainer( QLimVRd );
}

RetainingWallQSCombinationGUI::RetainingWallQSCombinationGUI( UnitMeasure * ump,
                                                              const QString &cn,
                                                              DoublePlus * Gk2, DoublePlus * Qk,
                                                              DoublePlus * gammaTanPhiPrime, DoublePlus * gammaCPrime,
                                                              DoublePlus *gammaG1Min, DoublePlus *gammaG1Max,
                                                              DoublePlus *gammaG2Min, DoublePlus *gammaG2Max,
                                                              DoublePlus *gammaQMin, DoublePlus *gammaQMax,
                                                              DoublePlus *gammaSliding, DoublePlus *gammaQLim,
                                                              RetainingWallGUI *parent):
    QWidget(parent),
    m_ui(new Ui::RetainingWallQSCombinationGUI),
    m_d( new RetainingWallQSCombinationGUIPrivate( ump, cn,
                                                   Gk2, Qk,
                                                   gammaTanPhiPrime, gammaCPrime,
                                                   gammaG1Min, gammaG1Max,
                                                   gammaG2Min, gammaG2Max,
                                                   gammaQMin, gammaQMax,
                                                   gammaSliding, gammaQLim) ) {
    m_ui->setupUi(this);

    setVar();

    connect( m_ui->pushButton, SIGNAL(clicked()), this, SLOT(calculateQSCombination()));

    connect( m_ui->copyClipPButton, SIGNAL(clicked()), this, SLOT(copyClip()) );
}

RetainingWallQSCombinationGUI::~RetainingWallQSCombinationGUI() {
    delete m_d;
    delete m_ui;
}

void RetainingWallQSCombinationGUI::setRetainingWall( RetainingWall * wall ){
    m_d->retainingWall = wall;
}

void RetainingWallQSCombinationGUI::setVar(  ) {
    VarPlusGUI::connectVar( m_d->QLimBPrime, m_ui->QLimBPrimeLabel, m_ui->QLimBPrimeLEdit, m_ui->QLimBPrimeUMLabel, m_ui->QLimBPrimeCheckBox );
    VarPlusGUI::connectVar( m_d->QLimLPrime, m_ui->QLimLPrimeLabel, m_ui->QLimLPrimeLEdit, m_ui->QLimLPrimeUMLabel, m_ui->QLimLPrimeCheckBox );
    VarPlusGUI::connectVar( m_d->QLimD, m_ui->QLimDLabel, m_ui->QLimDLEdit, m_ui->QLimDUMLabel, m_ui->QLimDCheckBox );
    VarPlusGUI::connectVar( m_d->QLimLInfinite,m_ui->QLimLInfiniteLabel, 0, 0, m_ui->QLimLInfiniteCheckbox );
    VarPlusGUI::connectVar( m_d->SoilUpPhiPrimeD, m_ui->SoilUpPhiPrimeDLabel, m_ui->SoilUpPhiPrimeDLEdit, m_ui->SoilUpPhiPrimeDUMLabel, m_ui->SoilUpPhiPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->SoilUpCPrimeD, m_ui->SoilUpCPrimeDLabel, m_ui->SoilUpCPrimeDLEdit, m_ui->SoilUpCPrimeDUMLabel, m_ui->SoilUpCPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->SoilUpDeltaD, m_ui->SoilUpDeltaDLabel, m_ui->SoilUpDeltaDLEdit, m_ui->SoilUpDeltaDUMLabel, m_ui->SoilUpDeltaDCheckBox );
    VarPlusGUI::connectVar( m_d->SoilDownPhiPrimeD, m_ui->SoilDownPhiPrimeDLabel, m_ui->SoilDownPhiPrimeDLEdit, m_ui->SoilDownPhiPrimeDUMLabel, m_ui->SoilDownPhiPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->SoilDownCPrimeD, m_ui->SoilDownCPrimeDLabel, m_ui->SoilDownCPrimeDLEdit, m_ui->SoilDownCPrimeDUMLabel, m_ui->SoilDownCPrimeDCheckBox );
    VarPlusGUI::connectVar( m_d->SoilDownDeltaD, m_ui->SoilDownDeltaDLabel, m_ui->SoilDownDeltaDLEdit, m_ui->SoilDownDeltaDUMLabel, m_ui->SoilDownDeltaDCheckBox );
    VarPlusGUI::connectVar( m_d->QLimSoilDownGammaWPrimeD, m_ui->QLimGammaWDLabel, m_ui->QLimGammaWDLEdit, m_ui->QLimGammaWDUMLabel, m_ui->QLimGammaWDCheckBox );
    VarPlusGUI::connectVar( m_d->Ka, m_ui->KaLabel, m_ui->KaLEdit, m_ui->KaUMLabel, m_ui->KaCheckBox );
    VarPlusGUI::connectVar( m_d->Kp, m_ui->KpLabel, m_ui->KpLEdit, m_ui->KpUMLabel, m_ui->KpCheckBox );
    VarPlusGUI::connectVar( m_d->ChiKp, m_ui->ChiKpLabel, m_ui->ChiKpLEdit, m_ui->ChiKpUMLabel);
    VarPlusGUI::connectVar( m_d->KpRed, m_ui->KpRedLabel, m_ui->KpRedLEdit, m_ui->KpRedUMLabel, m_ui->KpRedCheckBox );
    VarPlusGUI::connectVar( m_d->SlidingVEd, m_ui->SlidingVEdLabel, m_ui->SlidingVEdLEdit, m_ui->SlidingVEdUMLabel, m_ui->SlidingVEdCheckBox );
    VarPlusGUI::connectVar( m_d->SlidingHEd, m_ui->SlidingHEdLabel, m_ui->SlidingHEdLEdit, m_ui->SlidingHEdUMLabel, m_ui->SlidingHEdCheckBox );
    VarPlusGUI::connectVar( m_d->SlidingHRdP, m_ui->SlidingHRdPLabel, m_ui->SlidingHRdPLEdit, m_ui->SlidingHRdPUMLabel, m_ui->SlidingHRdPCheckBox );
    VarPlusGUI::connectVar( m_d->SlidingApplyKp, m_ui->SlidingApplyKpLabel, 0, 0, m_ui->SlidingApplyKpCheckBox );
    VarPlusGUI::connectVar( m_d->SlidingHRd, m_ui->SlidingHRdLabel, m_ui->SlidingHRdLEdit, m_ui->SlidingHRdUMLabel);
    VarPlusGUI::connectVar( m_d->QLimApplyKp, m_ui->QLimApplyKpLabel, 0, 0, m_ui->QLimApplyKpCheckBox );
    VarPlusGUI::connectVar( m_d->QLimHRdPMax, m_ui->QLimHRdPMaxLabel, m_ui->QLimHRdPMaxLEdit, m_ui->QLimHRdPMaxUMLabel, m_ui->QLimHRdPMaxCheckBox );
    VarPlusGUI::connectVar( m_d->QLimHRdP, m_ui->QLimHRdPLabel, m_ui->QLimHRdPLEdit, m_ui->QLimHRdPUMLabel, m_ui->QLimHRdPCheckBox );
    VarPlusGUI::connectVar( m_d->QLimVEd, m_ui->QLimVEdLabel, m_ui->QLimVEdLEdit, m_ui->QLimVEdUMLabel, m_ui->QLimVEdCheckBox );
    VarPlusGUI::connectVar( m_d->QLimHEd, m_ui->QLimHEdLabel, m_ui->QLimHEdLEdit, m_ui->QLimHEdUMLabel, m_ui->QLimHEdCheckBox );
    VarPlusGUI::connectVar( m_d->QLimMEdC->y, m_ui->QLimMEdYCLabel, m_ui->QLimMEdYCLEdit, m_ui->QLimMEdYCUMLabel, m_ui->QLimMEdYCCheckBox );
    VarPlusGUI::connectVar( m_d->QLimMEdC->z, m_ui->QLimMEdZCLabel, m_ui->QLimMEdZCLEdit, m_ui->QLimMEdZCUMLabel, m_ui->QLimMEdZCCheckBox );
    VarPlusGUI::connectVar( m_d->QLimMEd, m_ui->QLimMEdLabel, m_ui->QLimMEdLEdit, m_ui->QLimMEdUMLabel, m_ui->QLimMEdCheckBox );
    VarPlusGUI::connectVar( m_d->QLimQEd, m_ui->QLimQEdLabel, m_ui->QLimQEdLEdit, m_ui->QLimQEdUMLabel, m_ui->QLimQEdCheckBox );
    VarPlusGUI::connectVar( m_d->QLimNGammaW, m_ui->NGammaWLabel, m_ui->NGammaWLEdit, m_ui->NGammaWUMLabel, m_ui->NGammaWCheckBox );
    VarPlusGUI::connectVar( m_d->QLimSGammaW, m_ui->sGammaWLabel, m_ui->sGammaWLEdit, m_ui->sGammaWUMLabel, m_ui->sGammaWCheckBox );
    VarPlusGUI::connectVar( m_d->QLimIGammaW, m_ui->iGammaWLabel, m_ui->iGammaWLEdit, m_ui->iGammaWUMLabel, m_ui->iGammaWCheckBox );
    VarPlusGUI::connectVar( m_d->QLimBGammaW, m_ui->bGammaWLabel, m_ui->bGammaWLEdit, m_ui->bGammaWUMLabel, m_ui->bGammaWCheckBox );
    VarPlusGUI::connectVar( m_d->QLimGGammaW, m_ui->gGammaWLabel, m_ui->gGammaWLEdit, m_ui->gGammaWUMLabel, m_ui->gGammaWCheckBox );
    VarPlusGUI::connectVar( m_d->QLimNC, m_ui->NcLabel, m_ui->NcLEdit, m_ui->NcUMLabel, m_ui->NcCheckBox );
    VarPlusGUI::connectVar( m_d->QLimSC, m_ui->scLabel, m_ui->scLEdit, m_ui->scUMLabel, m_ui->scCheckBox );
    VarPlusGUI::connectVar( m_d->QLimIC, m_ui->icLabel, m_ui->icLEdit, m_ui->icUMLabel, m_ui->icCheckBox );
    VarPlusGUI::connectVar( m_d->QLimBC, m_ui->bcLabel, m_ui->bcLEdit, m_ui->bcUMLabel, m_ui->bcCheckBox );
    VarPlusGUI::connectVar( m_d->QLimGC, m_ui->gcLabel, m_ui->gcLEdit, m_ui->gcUMLabel, m_ui->gcCheckBox );
    VarPlusGUI::connectVar( m_d->QLimDC, m_ui->dcLabel, m_ui->dcLEdit, m_ui->dcUMLabel, m_ui->dcCheckBox );
    VarPlusGUI::connectVar( m_d->QLimNQ, m_ui->NqLabel, m_ui->NqLEdit, m_ui->NqUMLabel, m_ui->NqCheckBox );
    VarPlusGUI::connectVar( m_d->QLimSQ, m_ui->sqLabel, m_ui->sqLEdit, m_ui->sqUMLabel, m_ui->sqCheckBox );
    VarPlusGUI::connectVar( m_d->QLimIQ, m_ui->iqLabel, m_ui->iqLEdit, m_ui->iqUMLabel, m_ui->iqCheckBox );
    VarPlusGUI::connectVar( m_d->QLimBQ, m_ui->bqLabel, m_ui->bqLEdit, m_ui->bqUMLabel, m_ui->bqCheckBox );
    VarPlusGUI::connectVar( m_d->QLimGQ, m_ui->gqLabel, m_ui->gqLEdit, m_ui->gqUMLabel, m_ui->gqCheckBox );
    VarPlusGUI::connectVar( m_d->QLimDQ, m_ui->dqLabel, m_ui->dqLEdit, m_ui->dqUMLabel, m_ui->dqCheckBox );
    VarPlusGUI::connectVar( m_d->qLimD, m_ui->qLimDLabel, m_ui->qLimDLEdit, m_ui->qLimDUMLabel );
    VarPlusGUI::connectVar( m_d->QLimVRd, m_ui->QLimVRdLabel, m_ui->QLimVRdLEdit, m_ui->QLimVRdUMLabel );
}

void RetainingWallQSCombinationGUI::calculateQSCombination() {
    if( m_d->retainingWall ){
        m_d->retainingWall->calculatePhiPrimeD( m_d->SoilDownPhiPrimeD, m_d->SoilUpPhiPrimeD, m_d->gammaTanPhiPrime );
        m_d->retainingWall->calculateGammaWD( m_d->QLimSoilDownGammaWPrimeD, NULL, m_d->gammaG1Min );
        m_d->retainingWall->calculateCPrimeD( m_d->SoilDownCPrimeD, m_d->SoilUpCPrimeD, m_d->gammaCPrime );
        m_d->retainingWall->calculateDeltaD( m_d->SoilDownDeltaD, m_d->SoilUpDeltaD, m_d->gammaTanPhiPrime );
        if( m_d->SoilDownDeltaD->valueNormal() > m_d->SoilDownPhiPrimeD->valueNormal() ){
            m_d->SoilDownDeltaD->setValueNormal( m_d->SoilDownPhiPrimeD->valueNormal() );
        }
        m_d->retainingWall->calculateK( m_d->SoilUpPhiPrimeD, m_d->SoilUpDeltaD, m_d->Ka, m_d->SoilDownPhiPrimeD, m_d->SoilDownDeltaD, m_d->Kp, m_d->ChiKp, m_d->KpRed );

        m_d->retainingWall->calculateSlidingVH( m_d->SlidingVEd, m_d->SlidingHEd, m_d->SlidingHRdP, m_d->SoilDownCPrimeD, m_d->SoilUpCPrimeD, m_d->Ka, m_d->KpRed, m_d->Gk2, m_d->Qk, m_d->gammaG1Min, m_d->gammaG1Max, m_d->gammaG2Min, m_d->gammaG2Max, m_d->gammaQMin, m_d->gammaQMax );
        m_d->retainingWall->calculateSliding( m_d->SlidingHRd, m_d->SlidingApplyKp, m_d->SlidingHRdP, m_d->SlidingHEd, m_d->SlidingVEd, m_d->SoilDownDeltaD, m_d->gammaSliding );

        if( m_d->QLimQEd->readOnly() ){
            m_d->QLimQEd->setValueNormal( m_d->QLimSoilDownGammaWPrimeD->valueNormal() * m_d->retainingWall->soilDownZ->valueNormal() );
        }
        m_d->retainingWall->calculateQLimVHM( m_d->QLimVEd, m_d->QLimHEd, m_d->QLimMEdC, m_d->QLimMEd, m_d->SoilDownCPrimeD, m_d->SoilUpCPrimeD, m_d->Ka, m_d->QLimApplyKp, m_d->KpRed, m_d->QLimHRdPMax, m_d->QLimHRdP, m_d->Gk2, m_d->Qk, m_d->gammaG1Min, m_d->gammaG1Max, m_d->gammaG2Min, m_d->gammaG2Max, m_d->gammaQMin, m_d->gammaQMax );
        m_d->retainingWall->calculateQLim( m_d->qLimD, m_d->QLimVRd,
                                           m_d->SoilDownPhiPrimeD, m_d->SoilDownCPrimeD, m_d->QLimSoilDownGammaWPrimeD, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite, m_d->QLimD, m_d->QLimHEd, m_d->QLimVEd, m_d->QLimMEd, m_d->QLimQEd,
                                           m_d->QLimNGammaW, m_d->QLimSGammaW, m_d->QLimIGammaW, m_d->QLimBGammaW, m_d->QLimGGammaW,
                                           m_d->QLimNC, m_d->QLimSC, m_d->QLimIC, m_d->QLimBC, m_d->QLimGC, m_d->QLimDC,
                                           m_d->QLimNQ, m_d->QLimSQ, m_d->QLimIQ, m_d->QLimBQ, m_d->QLimGQ, m_d->QLimDQ,
                                           m_d->gammaQLim);
    }
}

void RetainingWallQSCombinationGUI::copyClip(){
    QList<VarPlus *> v;

    // v << trUtf8("Verifica scorrimento e capacitÃ  portante");

    // v << trUtf8("Parametri geotecnici");
    v << m_d->gammaTanPhiPrime << m_d->SoilDownPhiPrimeD << m_d->SoilUpPhiPrimeD;
    v << m_d->QLimSoilDownGammaWPrimeD;
    v << m_d->gammaCPrime << m_d->SoilDownCPrimeD << m_d->SoilUpCPrimeD;
    v << m_d->gammaTanPhiPrime << m_d->SoilDownDeltaD << m_d->SoilUpDeltaD;
    v << m_d->Ka << m_d->Kp << m_d->ChiKp << m_d->KpRed;

    // v << trUtf8("Verifica scorrimento");
    v << m_d->SlidingHEd << m_d->SlidingVEd << m_d->SlidingHRdP << m_d->SlidingHRd;

    // v << trUtf8("Verifica capacitÃ  portante");
    v << m_d->QLimSoilDownGammaWPrimeD << m_d->QLimQEd;
    v << m_d->QLimHEd << m_d->QLimVEd << m_d->QLimMEdC->y << m_d->QLimMEdC->z << m_d->QLimMEd;
    v << m_d->QLimBPrime << m_d->QLimLPrime << m_d->QLimD;
    v << m_d->QLimNGammaW << m_d->QLimSGammaW << m_d->QLimIGammaW << m_d->QLimBGammaW << m_d->QLimGGammaW;
    v << m_d->QLimNC << m_d->QLimSC << m_d->QLimIC << m_d->QLimBC << m_d->QLimGC << m_d->QLimDC;
    v << m_d->QLimNQ << m_d->QLimSQ << m_d->QLimIQ << m_d->QLimBQ << m_d->QLimGQ << m_d->QLimDQ;
    v << m_d->qLimD;
    v << m_d->QLimVRd;

    QClipboard * clip = QApplication::clipboard();
    QMimeData * data = new QMimeData;
    data->setHtml( printData(v) );
    clip->setMimeData( data  );
}

QString RetainingWallQSCombinationGUI::printData( QList<VarPlus *> & v ){
    QString text;
    for( int i=0; i < v.size(); i++ ){
        text +=  v.at(i)->richName() + " ";
        if( v.at(i)->unitMeasurePointer() != NULL )
            text += "[" + v.at(i)->unitMeasurePointer()->string(  v.at(i)->unitMeasureValue() ) + "] ";
        text += "= " + v.at(i)->valueStr() + "\n";
    }
    return text;
}
