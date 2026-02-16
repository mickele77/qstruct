#include "soilgui.h"
#include "ui_soilgui.h"

#include "soil.h"
#include "doubleplus.h"
#include "qstringplus.h"
#include "boolplus.h"
#include "varpluscontainer.h"
#include "varplusguiutility.h"

#include <cmath>

class SoilGUIPrivate: public VarPlusContainer{
public:
    SoilGUIPrivate( UnitMeasure * ump ):
        VarPlusContainer(ump, "", "" ),
        ui(new Ui::SoilGUI) {
        soil = NULL;

        KGammaTanPhiPrime = new DoublePlus(1.0, "KGammaTanPhiPrime", ump, UnitMeasure::noDimension);
        KGammaTanPhiPrime->setRichName( QObject::tr("γ<span style=\" vertical-align:sub;\">φ</span>") );
        KGammaTanPhiPrime->setToolTip( QObject::tr("Coefficiente di sicurezza della tangente dell'angolo di attrito interno") );
        addVarToContainer( KGammaTanPhiPrime );

        KPhiPrimeD = new DoublePlus(0.0, "KphiPrimeD", ump, UnitMeasure::angle);
        KPhiPrimeD->setRichName( QObject::tr("φ'<span style=\" vertical-align:sub;\">d</span>") );
        KPhiPrimeD->setToolTip( QObject::tr("Angolo di attrito interno di progetto") );
        KPhiPrimeD->setReadOnly( true );
        addVarToContainer( KPhiPrimeD );

        Ki = new DoublePlus(0.0, "Ki", ump, UnitMeasure::angle);
        Ki->setRichName( QObject::tr("i"));
        Ki->setToolTip( QObject::tr("Inclinazione del piano campagna rispetto all'orizzontale"));
        addVarToContainer( Ki );

        Kbeta = new DoublePlus(0.0, "Kbeta", ump, UnitMeasure::angle);
        Kbeta->setRichName( QObject::tr("β"));
        Kbeta->setToolTip( QObject::tr("Inclinazione del paramento del muro rispetto alla verticale"));
        addVarToContainer( Kbeta );

        Kdelta = new DoublePlus(0.0, "Kdelta", ump, UnitMeasure::angle);;
        Kdelta->setRichName( QObject::tr("δ"));
        Kdelta->setToolTip( QObject::tr("Angolo di attrito tra muro e terreno"));
        addVarToContainer( Kdelta );

        K_Ka = new DoublePlus(0.0, "K_Ka", ump, UnitMeasure::noDimension);
        K_Ka->setRichName( QObject::tr("K<span style=\" vertical-align:sub;\">a</span>"));
        K_Ka->setToolTip( QObject::tr("Coefficiente di spinta attiva"));
        K_Ka->setReadOnly( true );
        addVarToContainer( K_Ka );

        K_Kp = new DoublePlus(0.0, "Kp", ump, UnitMeasure::noDimension);
        K_Kp->setRichName( QObject::tr("K<span style=\" vertical-align:sub;\">p</span>"));
        K_Kp->setToolTip( QObject::tr("Coefficiente di spinta passiva"));
        K_Kp->setReadOnly( true );
        addVarToContainer( K_Kp );

        QLimGammaGammaW = new DoublePlus(1.0, "QLimGammaGammaW", ump, UnitMeasure::noDimension);
        QLimGammaGammaW->setRichName( QObject::tr("γ<span style=\" vertical-align:sub;\">γ</span>") );
        QLimGammaGammaW->setToolTip( QObject::tr("Peso specifico del terreno - Coefficiente di sicurezza") );
        addVarToContainer( QLimGammaGammaW );

        QLimGammaWD = new DoublePlus( 0.0, "QLimGammaWD", ump, UnitMeasure::loadFVolume );
        QLimGammaWD->setRichName( QObject::tr("γ'") );
        QLimGammaWD->setToolTip( QObject::tr("Peso specifico del terreno - Valore di calcolo") );
        QLimGammaWD->setReadOnly( true );
        addVarToContainer( QLimGammaWD );

        QLimB = new DoublePlus( 3.0, "QLimB", ump, UnitMeasure::length );
        QLimB->setRichName( QObject::tr("B"));
        QLimB->setToolTip( QObject::tr("Dimensione trasversale della fondazione (lungo y)"));
        QLimB->setReadOnly( false );
        addVarToContainer( QLimB );

        QLimLInfinite = new BoolPlus( true, "QLimLInfinite" );
        QLimLInfinite->setRichName( QObject::tr("Lunghezza infinita"));
        QLimLInfinite->setToolTip( QObject::tr("Fondazione di lughezza infinita"));
        addVarToContainer( QLimLInfinite );

        QLimL = new DoublePlus( 1.0, "QLimL", ump, UnitMeasure::length );
        QLimL->setRichName( QObject::tr("L"));
        QLimL->setToolTip( QObject::tr("Dimensione longitudinale della fondazione (lungo x)"));
        QLimL->setReadOnly( false );
        addVarToContainer( QLimL );

        QLimAlpha = new DoublePlus( 0.0, "QLimAlpha", ump, UnitMeasure::angle );
        QLimAlpha->setRichName( QObject::tr("α"));
        QLimAlpha->setToolTip( QObject::tr("Inclinazione della fondazione rispetto all'orizzontale"));
        addVarToContainer( QLimAlpha );

        QLimD = new DoublePlus( 1.0, "QLimD", ump, UnitMeasure::length );
        QLimD->setRichName( QObject::tr("D"));
        QLimD->setToolTip( QObject::tr("Approfondimento della fondazione rispetto alla quota del piano di campagna a valle"));
        addVarToContainer( QLimD );

        QLimOmega = new DoublePlus( 0.0, "QLimOmega", ump, UnitMeasure::angle );
        QLimOmega->setRichName( QObject::tr("ω"));
        QLimOmega->setToolTip( QObject::tr("Inclinazione del piano di campagna a monte rispetto all'orizzontale"));
        addVarToContainer( QLimOmega );

        QLimGammaTanPhiPrime = new DoublePlus(1.25, "QLimGammaTanPhiPrime", ump, UnitMeasure::noDimension);
        QLimGammaTanPhiPrime->setRichName( QObject::tr("γ<span style=\" vertical-align:sub;\">φ</span>") );
        QLimGammaTanPhiPrime->setToolTip( QObject::tr("Coefficiente di sicurezza della tangente dell'angolo di attrito interno") );
        addVarToContainer( QLimGammaTanPhiPrime );

        QLimPhiPrimeD = new DoublePlus(0.0, "QLimPhiPrimeD", ump, UnitMeasure::angle);
        QLimPhiPrimeD->setRichName( QObject::tr("φ'<span style=\" vertical-align:sub;\">d</span>") );
        QLimPhiPrimeD->setToolTip( QObject::tr("Angolo di attrito interno di progetto") );
        QLimPhiPrimeD->setReadOnly( true );
        addVarToContainer( QLimPhiPrimeD );

        QLimGammaCPrime = new DoublePlus(1.25, "QLimGammaCPrime", ump, UnitMeasure::noDimension);
        QLimGammaCPrime->setRichName( QObject::tr("γ<span style=\" vertical-align:sub;\">c</span>") );
        QLimGammaCPrime->setToolTip( QObject::tr("Coefficiente di sicurezza della coesione") );
        addVarToContainer( QLimGammaCPrime );

        QLimCPrimeD = new DoublePlus(0.0, "QLimCPrimeD", ump, UnitMeasure::tension);
        QLimCPrimeD->setRichName( QObject::tr("c'<span style=\" vertical-align:sub;\">d</span>") );
        QLimCPrimeD->setToolTip( QObject::tr("Coesione di progetto") );
        QLimCPrimeD->setReadOnly( true );
        addVarToContainer( QLimCPrimeD );

        QLimVd = new DoublePlus(0.0, "QLimVd", ump, UnitMeasure::loadF);
        QLimVd->setRichName( QObject::tr("V<span style=\" vertical-align:sub;\">d</span>") );
        QLimVd->setToolTip( QObject::tr("Condizini di progetto - Carico verticale") );
        addVarToContainer( QLimVd );

        QLimHyd = new DoublePlus(0.0, "QLimHyd", ump, UnitMeasure::loadF);
        QLimHyd->setRichName( QObject::tr("H<span style=\" vertical-align:sub;\">y,d</span>") );
        QLimHyd->setToolTip( QObject::tr("Condizini di progetto - Carico orizzontale lungo y") );
        addVarToContainer( QLimHyd );

        QLimHxd = new DoublePlus(0.0, "QLimHxd", ump, UnitMeasure::loadF);
        QLimHxd->setRichName( QObject::tr("H<span style=\" vertical-align:sub;\">x,d</span>") );
        QLimHxd->setToolTip( QObject::tr("Condizini di progetto - Carico orizzontale lungo x") );
        addVarToContainer( QLimHxd );

        QLimHd = new DoublePlus(0.0, "QLimHd", ump, UnitMeasure::loadF);
        QLimHd->setRichName( QObject::tr("H<span style=\" vertical-align:sub;\">d</span>") );
        QLimHd->setToolTip( QObject::tr("Condizini di progetto - Risultante carichi orizzontale") );
        QLimHd->setReadOnly( true );
        addVarToContainer( QLimHd );

        QLimMxd = new DoublePlus(0.0, "QLimMxd", ump, UnitMeasure::loadF);
        QLimMxd->setRichName( QObject::tr("M<span style=\" vertical-align:sub;\">x,d</span>") );
        QLimMxd->setToolTip( QObject::tr("Condizini di progetto - Momento agente nel piano perpendicolare ad asse x") );
        addVarToContainer( QLimMxd );

        QLimMyd = new DoublePlus(0.0, "QLimMyd", ump, UnitMeasure::loadF);
        QLimMyd->setRichName( QObject::tr("M<span style=\" vertical-align:sub;\">y,d</span>") );
        QLimMyd->setToolTip( QObject::tr("Condizini di progetto - Momento agente nel piano perpendicolare ad asse y") );
        addVarToContainer( QLimMyd );

        QLimBPrime = new DoublePlus( 3.0, "QLimBPrime", ump, UnitMeasure::length );
        QLimBPrime->setRichName( QObject::tr("B'"));
        QLimBPrime->setToolTip( QObject::tr("Dimensione trasversale della fondazione (lungo y)"));
        QLimBPrime->setReadOnly( true );
        addVarToContainer( QLimBPrime );

        QLimLPrime = new DoublePlus( 1.0, "QLimLPrime", ump, UnitMeasure::length );
        QLimLPrime->setRichName( QObject::tr("L'"));
        QLimLPrime->setToolTip( QObject::tr("Dimensione longitudinale equivalente della fondazione (lungo x)"));
        QLimLPrime->setReadOnly( true );
        addVarToContainer( QLimLPrime );

        QLimqPrimed = new DoublePlus( 0.0, "QLimqPrimed", ump, UnitMeasure::loadFArea );
        QLimqPrimed->setRichName( QObject::tr("q'<span style=\" vertical-align:sub;\">d</span>"));
        QLimqPrimed->setToolTip( QObject::tr("Condizioni di progetto - Carico verticale distribuito agente a monte"));
        addVarToContainer( QLimqPrimed );

        NGammaWPrime =  new DoublePlus( 1.0, "NGammaWPrime", ump, UnitMeasure::noDimension );
        NGammaWPrime->setRichName( QObject::tr("N<span style=\" vertical-align:sub;\">γ</span>"));
        NGammaWPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di capacitÃ    portante legato al peso specifico"));
        NGammaWPrime->setReadOnly( true );
        addVarToContainer( NGammaWPrime );

        sGammaWPrime =  new DoublePlus( 1.0, "sGammaWPrime", ump, UnitMeasure::noDimension );
        sGammaWPrime->setRichName( QObject::tr("s<span style=\" vertical-align:sub;\">γ</span>"));
        sGammaWPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di capacitÃ    portante legato al peso specifico"));
        sGammaWPrime->setReadOnly( true );
        addVarToContainer( sGammaWPrime );

        iGammaWPrime =  new DoublePlus( 1.0, "iGammaWPrime", ump, UnitMeasure::noDimension );
        iGammaWPrime->setRichName( QObject::tr("i<span style=\" vertical-align:sub;\">γ</span>"));
        iGammaWPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di inclinazione del carico legato al peso specifico"));
        iGammaWPrime->setReadOnly( true );
        addVarToContainer( iGammaWPrime );

        bGammaWPrime =  new DoublePlus( 1.0, "bGammaWPrime", ump, UnitMeasure::noDimension );
        bGammaWPrime->setRichName( QObject::tr("b<span style=\" vertical-align:sub;\">γ</span>"));
        bGammaWPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di inclinazione della fondazione legato al peso specifico"));
        bGammaWPrime->setReadOnly( true );
        addVarToContainer( bGammaWPrime );

        gGammaWPrime =  new DoublePlus( 1.0, "gGammaWPrime", ump, UnitMeasure::noDimension );
        gGammaWPrime->setRichName( QObject::tr("g<span style=\" vertical-align:sub;\">γ</span>"));
        gGammaWPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di inclinazione del piano campagna legato al peso specifico"));
        gGammaWPrime->setReadOnly( true );
        addVarToContainer( gGammaWPrime );

        NcPrime =  new DoublePlus( 1.0, "NcPrime", ump, UnitMeasure::noDimension );
        NcPrime->setRichName( QObject::tr("N<span style=\" vertical-align:sub;\">c</span>"));
        NcPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di capacitÃ    portante legato alla coesione"));
        NcPrime->setReadOnly( true );
        addVarToContainer( NcPrime );

        scPrime =  new DoublePlus( 1.0, "scPrime", ump, UnitMeasure::noDimension );
        scPrime->setRichName( QObject::tr("N<span style=\" vertical-align:sub;\">c</span>"));
        scPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di forma legato alla coesione"));
        scPrime->setReadOnly( true );
        addVarToContainer( scPrime );

        icPrime =  new DoublePlus( 1.0, "icPrime", ump, UnitMeasure::noDimension );
        icPrime->setRichName( QObject::tr("i<span style=\" vertical-align:sub;\">c</span>"));
        icPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di inclinazione del carico legato alla coesione"));
        icPrime->setReadOnly( true );
        addVarToContainer( icPrime );

        bcPrime =  new DoublePlus( 1.0, "bcPrime", ump, UnitMeasure::noDimension );
        bcPrime->setRichName( QObject::tr("b<span style=\" vertical-align:sub;\">c</span>"));
        bcPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di inclinazione della fondazione legato alla coesione"));
        bcPrime->setReadOnly( true );
        addVarToContainer( bcPrime );

        gcPrime =  new DoublePlus( 1.0, "gcPrime", ump, UnitMeasure::noDimension );
        gcPrime->setRichName( QObject::tr("g<span style=\" vertical-align:sub;\">c</span>"));
        gcPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di inclinazione del piano campagna legato alla coesione"));
        gcPrime->setReadOnly( true );
        addVarToContainer( gcPrime );

        dcPrime =  new DoublePlus( 1.0, "dcPrime", ump, UnitMeasure::noDimension );
        dcPrime->setRichName( QObject::tr("d<span style=\" vertical-align:sub;\">c</span>"));
        dcPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di affondamento della fondazione legato alla coesione"));
        dcPrime->setReadOnly( true );
        addVarToContainer( dcPrime );

        NqPrime =  new DoublePlus( 1.0, "NqPrime", ump, UnitMeasure::noDimension );
        NqPrime->setRichName( QObject::tr("N<span style=\" vertical-align:sub;\">q</span>"));
        NqPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di capacitÃ    portante legato al carico distribuito applicato a monte"));
        NqPrime->setReadOnly( true );
        addVarToContainer(  NqPrime );

        sqPrime =  new DoublePlus( 1.0, "sqPrime", ump, UnitMeasure::noDimension );
        sqPrime->setRichName( QObject::tr("s<span style=\" vertical-align:sub;\">q</span>"));
        sqPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di forma legato al carico distribuito applicato a monte"));
        sqPrime->setReadOnly( true );
        addVarToContainer(  sqPrime );

        iqPrime =  new DoublePlus( 1.0, "iqPrime", ump, UnitMeasure::noDimension );
        iqPrime->setRichName( QObject::tr("i<span style=\" vertical-align:sub;\">q</span>"));
        iqPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di inclinazione del carico legato al carico distribuito applicato a monte"));
        iqPrime->setReadOnly( true );
        addVarToContainer(  iqPrime );

        bqPrime =  new DoublePlus( 1.0, "bqPrime", ump, UnitMeasure::noDimension );
        bqPrime->setRichName( QObject::tr("b<span style=\" vertical-align:sub;\">q</span>"));
        bqPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di inclinazione della fondazione legato al carico distribuito applicato a monte"));
        bqPrime->setReadOnly( true );
        addVarToContainer(  bqPrime );

        gqPrime =  new DoublePlus( 1.0, "gqPrime", ump, UnitMeasure::noDimension );
        gqPrime->setRichName( QObject::tr("g<span style=\" vertical-align:sub;\">q</span>"));
        gqPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di inclinazione del piano campagna legato al carico distribuito applicato a monte"));
        gqPrime->setReadOnly( true );
        addVarToContainer(  gqPrime );

        dqPrime =  new DoublePlus( 1.0, "dqPrime", ump, UnitMeasure::noDimension );
        dqPrime->setRichName( QObject::tr("d<span style=\" vertical-align:sub;\">q</span>"));
        dqPrime->setToolTip( QObject::tr("Condizioni drenate - Coefficiente di affondamento della fondazione legato al carico distribuito applicato a monte"));
        dqPrime->setReadOnly( true );
        addVarToContainer(  dqPrime );

        qLimPrimeD =  new DoublePlus( 0.0, "qLimPrimeD", ump, UnitMeasure::tension );
        qLimPrimeD->setRichName( QObject::tr("q<span style=\" vertical-align:sub;\">lim,d</span>"));
        qLimPrimeD->setToolTip( QObject::tr("Condizioni drenate - Tensione limite di progetto"));
        qLimPrimeD->setReadOnly( true );
        addVarToContainer(  qLimPrimeD );

        FLimPrimeD =  new DoublePlus( 0.0, "FLimPrimeD", ump, UnitMeasure::loadF );
        FLimPrimeD->setRichName( QObject::tr("Q<span style=\" vertical-align:sub;\">lim,d</span>"));
        FLimPrimeD->setToolTip( QObject::tr("Condizioni drenate - Carico limite di di progetto"));
        FLimPrimeD->setReadOnly( true );
        addVarToContainer(  FLimPrimeD );
    };
    Ui::SoilGUI *ui;

    Soil * soil;
    DoublePlus * KGammaTanPhiPrime;
    DoublePlus * KPhiPrimeD;
    DoublePlus * Ki;
    DoublePlus * Kbeta;
    DoublePlus * Kdelta;
    DoublePlus * K_Ka;
    DoublePlus * K_Kp;

    // Peso specifico del terreno - Coefficiente di sicurezza
    DoublePlus * QLimGammaGammaW;
    // Peso specifico del terreno - Valore di calcolo
    DoublePlus * QLimGammaWD;
    // Dimensione trasversale della fondazione (lungo y)
    DoublePlus * QLimB;
    // Fondazione di lunghezza infinita
    BoolPlus * QLimLInfinite;
    // Dimensione longitudinale della fondazione (lungo x)
    DoublePlus * QLimL;
    // Inclinazione della fondazione
    DoublePlus * QLimAlpha;
    // Approfondimento della fondazione
    DoublePlus * QLimD;
    // Inclinazione del piano di campagna a monte
    DoublePlus * QLimOmega;
    // Coefficiente di sicurezza dell'angolo di attrito interno
    DoublePlus * QLimGammaTanPhiPrime;
    // Angolo di attrito interno di progetto
    DoublePlus * QLimPhiPrimeD;
    // Coefficiente di sicurezza della coesione interna
    DoublePlus * QLimGammaCPrime;
    // Coesione interna di progetto
    DoublePlus * QLimCPrimeD;
    // Condizioni di progetto - Carico verticale
    DoublePlus * QLimVd;
    // Condizioni di progetto - Carico orizzontale lungo y
    DoublePlus * QLimHyd;
    // Condizioni di progetto - Carico orizzontale lungo x
    DoublePlus * QLimHxd;
    // Condizioni di progetto - Risultante carichi orizzontale
    DoublePlus * QLimHd;
    // Condizioni di progetto - Momento agente nel piano perpendicolare ad asse x
    DoublePlus * QLimMxd;
    // Condizioni di progetto - Momento agente nel piano perpendicolare ad asse y
    DoublePlus * QLimMyd;
    // Dimensione trasversale equivalente della fondazione (lungo y)
    DoublePlus * QLimBPrime;
    // Dimensione longitudinale equivalente della fondazione (lungo x)
    DoublePlus * QLimLPrime;
    // Condizioni di progetto - Carico verticale distribuito agente a monte
    DoublePlus * QLimqPrimed;

    // Condizioni drenate - Coefficiente di capacitÃ    portante legato a gammaW
    DoublePlus * NGammaWPrime;
    // Condizioni drenate - Coefficiente di forma legato a gammaW
    DoublePlus * sGammaWPrime;
    // Condizioni drenate - Coefficiente di inclinazione del carico legato a gammaW
    DoublePlus * iGammaWPrime;
    // Condizioni drenate - Coefficiente di incliazione della fondazione legato a gammaW
    DoublePlus * bGammaWPrime;
    // Condizioni drenate -  Coefficiente di incliazione del piano campagna legato a gammaW
    DoublePlus * gGammaWPrime;
    // Condizioni drenate - Coefficiente di capacitÃ    portante legato a c
    DoublePlus * NcPrime;
    // Condizioni drenate - Coefficiente di forma legato a c
    DoublePlus * scPrime;
    // Condizioni drenate - Coefficiente di inclinazione del carico legato a c
    DoublePlus * icPrime;
    // Condizioni drenate - Coefficiente di incliazione della fondazione legato a c
    DoublePlus * bcPrime;
    // Condizioni drenate - Coefficiente di incliazione del piano campagna legato a c
    DoublePlus * gcPrime;
    // Condizioni drenate - Coefficiente di affondamento legato a c
    DoublePlus * dcPrime;
    // Condizioni drenate - Coefficiente di capacitÃ    portante legato a q
    DoublePlus * NqPrime;
    // Condizioni drenate - Coefficiente di forma legato a q
    DoublePlus * sqPrime;
    // Condizioni drenate - Coefficiente di inclinazione del carico legato a q
    DoublePlus * iqPrime;
    // Condizioni drenate - Coefficiente di incliazione della fondazione legato a q
    DoublePlus * bqPrime;
    // Condizioni drenate - Coefficiente di incliazione del piano campagna legato a q
    DoublePlus * gqPrime;
    // Condizioni drenate - Coefficiente di affondamento legato a q
    DoublePlus * dqPrime;
    // Condizioni drenate - Tensione limite di progetto
    DoublePlus * qLimPrimeD;
    // Condizioni drenate - Carico limite di di progetto
    DoublePlus * FLimPrimeD;

};

SoilGUI::SoilGUI(UnitMeasure * ump, QWidget *parent) :
    QWidget(parent),
    m_d(new SoilGUIPrivate( ump )){
    m_d->ui->setupUi(this);

    initVar();
}

void SoilGUI::initVar(){
    VarPlusGUI::connectVar( m_d->KGammaTanPhiPrime, m_d->ui->KGammaTanPhiPrimeLabel, m_d->ui->KGammaTanPhiPrimeLEdit, m_d->ui->KGammaTanPhiPrimeUMLabel );

    VarPlusGUI::connectVar( m_d->KPhiPrimeD, m_d->ui->KPhiPrimeDLabel, m_d->ui->KPhiPrimeDLEdit, m_d->ui->KPhiPrimeDUMLabel, m_d->ui->KPhiPrimeDCheckBox );
    connect( m_d->KGammaTanPhiPrime, &DoublePlus::valueChanged, this, &SoilGUI::setKPhiPrimeD );
    connect( m_d->KPhiPrimeD, &DoublePlus::readOnlyChanged, this, &SoilGUI::setKPhiPrimeD );
    setKPhiPrimeD();

    VarPlusGUI::connectVar( m_d->Ki, m_d->ui->KiLabel, m_d->ui->KiLEdit, m_d->ui->KiUMLabel );
    VarPlusGUI::connectVar( m_d->Kbeta, m_d->ui->KbetaLabel, m_d->ui->KbetaLEdit, m_d->ui->KbetaUMLabel );
    VarPlusGUI::connectVar( m_d->Kdelta, m_d->ui->KdeltaLabel, m_d->ui->KdeltaLEdit, m_d->ui->KdeltaUMLabel );
    VarPlusGUI::connectVar( m_d->K_Ka, m_d->ui->KaLabel, m_d->ui->KaLEdit, m_d->ui->KaUMLabel );
    connect( m_d->K_Ka, &DoublePlus::readOnlyChanged, this, &SoilGUI::setKa );
    connect( m_d->Ki, &DoublePlus::valueChanged, this, &SoilGUI::setKa );
    connect( m_d->Kbeta, &DoublePlus::valueChanged, this, &SoilGUI::setKa );
    connect( m_d->Kdelta, &DoublePlus::valueChanged, this, &SoilGUI::setKa );
    connect( m_d->KPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setKa );
    setKa();

    VarPlusGUI::connectVar( m_d->K_Kp, m_d->ui->KpLabel, m_d->ui->KpLEdit, m_d->ui->KpUMLabel );
    connect( m_d->K_Kp, &DoublePlus::readOnlyChanged, this, &SoilGUI::setKp );
    connect( m_d->Ki, &DoublePlus::valueChanged, this, &SoilGUI::setKp );
    connect( m_d->Kbeta, &DoublePlus::valueChanged, this, &SoilGUI::setKp );
    connect( m_d->Kdelta, &DoublePlus::valueChanged, this, &SoilGUI::setKp );
    connect( m_d->KPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setKp );
    setKp();

    VarPlusGUI::connectVar( m_d->QLimGammaGammaW, m_d->ui->QLimGammaGammaWLabel, m_d->ui->QLimGammaGammaWLEdit, m_d->ui->QLimGammaGammaWUMLabel );

    VarPlusGUI::connectVar( m_d->QLimGammaWD, m_d->ui->QLimGammaWDLabel, m_d->ui->QLimGammaWDLEdit, m_d->ui->QLimGammaWDUMLabel, m_d->ui->QLimGammaWDCheckBox );
    connect( m_d->QLimGammaWD, &DoublePlus::readOnlyChanged, this, &SoilGUI::setQLimGammaWd );
    setQLimGammaWd();

    VarPlusGUI::connectVar( m_d->QLimB, m_d->ui->QLimBLabel, m_d->ui->QLimBLEdit, m_d->ui->QLimBUMLabel );
    VarPlusGUI::connectVar( m_d->QLimLInfinite, m_d->ui->QLimLInfiniteLabel, m_d->ui->QLimLInfiniteCheckBox );

    VarPlusGUI::connectVar( m_d->QLimL, m_d->ui->QLimLLabel, m_d->ui->QLimLLEdit, m_d->ui->QLimLUMLabel );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setQLimLInfinite );
    setQLimLInfinite();

    VarPlusGUI::connectVar( m_d->QLimAlpha, m_d->ui->QLimAlphaLabel, m_d->ui->QLimAlphaLEdit, m_d->ui->QLimAlphaUMLabel );
    VarPlusGUI::connectVar( m_d->QLimD, m_d->ui->QLimDLabel, m_d->ui->QLimDLEdit, m_d->ui->QLimDUMLabel );
    VarPlusGUI::connectVar( m_d->QLimOmega, m_d->ui->QLimOmegaLabel, m_d->ui->QLimOmegaLEdit, m_d->ui->QLimOmegaUMLabel );
    VarPlusGUI::connectVar( m_d->QLimGammaTanPhiPrime, m_d->ui->QLimGammaTanPhiPrimeLabel, m_d->ui->QLimGammaTanPhiPrimeLEdit, m_d->ui->QLimGammaTanPhiPrimeUMLabel );

    VarPlusGUI::connectVar( m_d->QLimPhiPrimeD, m_d->ui->QLimPhiPrimeDLabel, m_d->ui->QLimPhiPrimeDLEdit, m_d->ui->QLimPhiPrimeDUMLabel, m_d->ui->QLimPhiPrimeDCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setQLimPhiPrimeD );
    connect( m_d->QLimGammaTanPhiPrime, &DoublePlus::valueChanged, this, &SoilGUI::setQLimPhiPrimeD );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::readOnlyChanged, this, &SoilGUI::setQLimPhiPrimeD );
    setQLimPhiPrimeD();

    VarPlusGUI::connectVar( m_d->QLimGammaCPrime, m_d->ui->QLimGammaCPrimeLabel, m_d->ui->QLimGammaCPrimeLEdit, m_d->ui->QLimGammaCPrimeUMLabel );

    VarPlusGUI::connectVar( m_d->QLimCPrimeD, m_d->ui->QLimCPrimeDLabel, m_d->ui->QLimCPrimeDLEdit, m_d->ui->QLimCPrimeDUMLabel, m_d->ui->QLimCPrimeDCheckBox );
    connect( m_d->QLimGammaCPrime, &DoublePlus::valueChanged, this, &SoilGUI::setQLimCPrimeD );
    connect( m_d->QLimCPrimeD, &DoublePlus::readOnlyChanged, this, &SoilGUI::setQLimCPrimeD );
    setQLimCPrimeD();

    VarPlusGUI::connectVar( m_d->QLimVd, m_d->ui->QLimVdLabel, m_d->ui->QLimVdLEdit, m_d->ui->QLimVdUMLabel );
    VarPlusGUI::connectVar( m_d->QLimHyd, m_d->ui->QLimHydLabel, m_d->ui->QLimHydLEdit, m_d->ui->QLimHydUMLabel );
    VarPlusGUI::connectVar( m_d->QLimHxd, m_d->ui->QLimHxdLabel, m_d->ui->QLimHxdLEdit, m_d->ui->QLimHxdUMLabel );

    VarPlusGUI::connectVar( m_d->QLimHd, m_d->ui->QLimHdLabel, m_d->ui->QLimHdLEdit, m_d->ui->QLimHdUMLabel, m_d->ui->QLimHdCheckBox );
    connect( m_d->QLimHxd, &DoublePlus::valueChanged, this, &SoilGUI::setQLimHd );
    connect( m_d->QLimHyd, &DoublePlus::valueChanged, this, &SoilGUI::setQLimHd );
    connect( m_d->QLimHd, &DoublePlus::readOnlyChanged, this, &SoilGUI::setQLimHd );
    setQLimHd();

    VarPlusGUI::connectVar( m_d->QLimMxd, m_d->ui->QLimMxdLabel, m_d->ui->QLimMxdLEdit, m_d->ui->QLimMxdUMLabel );
    VarPlusGUI::connectVar( m_d->QLimMyd, m_d->ui->QLimMydLabel, m_d->ui->QLimMydLEdit, m_d->ui->QLimMydUMLabel );
    VarPlusGUI::connectVar( m_d->QLimBPrime, m_d->ui->QLimBPrimeLabel, m_d->ui->QLimBPrimeLEdit, m_d->ui->QLimBPrimeUMLabel, m_d->ui->QLimBPrimeCheckBox );
    connect( m_d->QLimBPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setQLimBPrime );
    connect( m_d->QLimB, &DoublePlus::valueChanged, this, &SoilGUI::setQLimBPrime );
    connect( m_d->QLimVd, &DoublePlus::valueChanged, this, &SoilGUI::setQLimBPrime );
    connect( m_d->QLimMxd, &DoublePlus::valueChanged, this, &SoilGUI::setQLimBPrime );
    setQLimBPrime();

    VarPlusGUI::connectVar( m_d->QLimLPrime, m_d->ui->QLimLPrimeLabel, m_d->ui->QLimLPrimeLEdit, m_d->ui->QLimLPrimeUMLabel, m_d->ui->QLimLPrimeCheckBox );
    connect( m_d->QLimLPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setQLimLPrime );
    connect( m_d->QLimL, &DoublePlus::valueChanged, this, &SoilGUI::setQLimLPrime );
    connect( m_d->QLimVd, &DoublePlus::valueChanged, this, &SoilGUI::setQLimLPrime );
    connect( m_d->QLimMyd, &DoublePlus::valueChanged, this, &SoilGUI::setQLimLPrime );
    setQLimLPrime();

    VarPlusGUI::connectVar( m_d->QLimqPrimed, m_d->ui->QLimqPrimedLabel, m_d->ui->QLimqPrimedLEdit, m_d->ui->QLimqPrimedUMLabel );

    VarPlusGUI::connectVar( m_d->NGammaWPrime, m_d->ui->NGammaWPrimeLabel, m_d->ui->NGammaWPrimeLEdit, m_d->ui->NGammaWPrimeUMLabel, m_d->ui->NGammaWPrimeCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setNGammaWPrime );
    connect( m_d->NGammaWPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setNGammaWPrime );
    setNGammaWPrime();

    VarPlusGUI::connectVar( m_d->sGammaWPrime, m_d->ui->sGammaWPrimeLabel, m_d->ui->sGammaWPrimeLEdit, m_d->ui->sGammaWPrimeUMLabel, m_d->ui->sGammaWPrimeCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setSGammaWPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setSGammaWPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setSGammaWPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setSGammaWPrime );
    connect( m_d->sGammaWPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setSGammaWPrime );
    setSGammaWPrime();

    VarPlusGUI::connectVar( m_d->iGammaWPrime, m_d->ui->iGammaWPrimeLabel, m_d->ui->iGammaWPrimeLEdit, m_d->ui->iGammaWPrimeUMLabel, m_d->ui->iGammaWPrimeCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setIGammaWPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setIGammaWPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setIGammaWPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setIGammaWPrime );
    connect( m_d->QLimHd, &DoublePlus::valueChanged, this, &SoilGUI::setIGammaWPrime );
    connect( m_d->QLimVd, &DoublePlus::valueChanged, this, &SoilGUI::setIGammaWPrime );
    connect( m_d->iGammaWPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setIGammaWPrime );
    setIGammaWPrime();

    VarPlusGUI::connectVar( m_d->bGammaWPrime, m_d->ui->bGammaWPrimeLabel, m_d->ui->bGammaWPrimeLEdit, m_d->ui->bGammaWPrimeUMLabel, m_d->ui->bGammaWPrimeCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setBGammaWPrime );
    connect( m_d->QLimCPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setBGammaWPrime );
    connect( m_d->QLimAlpha, &DoublePlus::valueChanged, this, &SoilGUI::setBGammaWPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setBGammaWPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setBGammaWPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setBGammaWPrime );
    connect( m_d->bGammaWPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setBGammaWPrime );
    setBGammaWPrime();

    VarPlusGUI::connectVar( m_d->gGammaWPrime, m_d->ui->gGammaWPrimeLabel, m_d->ui->gGammaWPrimeLEdit, m_d->ui->gGammaWPrimeUMLabel, m_d->ui->gGammaWPrimeCheckBox );
    connect( m_d->QLimOmega, &DoublePlus::valueChanged, this, &SoilGUI::setGGammaWPrime );
    connect( m_d->gGammaWPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setGGammaWPrime );
    setGGammaWPrime();

    VarPlusGUI::connectVar( m_d->NcPrime, m_d->ui->NcPrimeLabel, m_d->ui->NcPrimeLEdit, m_d->ui->NcPrimeUMLabel, m_d->ui->NcPrimeCheckBox );
    connect( m_d->NcPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setNcPrime );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setNcPrime );
    setNcPrime();

    VarPlusGUI::connectVar( m_d->scPrime, m_d->ui->scPrimeLabel, m_d->ui->scPrimeLEdit, m_d->ui->scPrimeUMLabel, m_d->ui->scPrimeCheckBox );
    connect( m_d->scPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setScPrime );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setScPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::readOnlyChanged, this, &SoilGUI::setScPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setScPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setScPrime );
    setScPrime();

    VarPlusGUI::connectVar( m_d->icPrime, m_d->ui->icPrimeLabel, m_d->ui->icPrimeLEdit, m_d->ui->icPrimeUMLabel, m_d->ui->icPrimeCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setIcPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setIcPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setIcPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setIcPrime );
    connect( m_d->QLimHd, &DoublePlus::valueChanged, this, &SoilGUI::setIcPrime );
    connect( m_d->QLimVd, &DoublePlus::valueChanged, this, &SoilGUI::setIcPrime );
    connect( m_d->icPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setIcPrime );
    setIcPrime();

    VarPlusGUI::connectVar( m_d->bcPrime, m_d->ui->bcPrimeLabel, m_d->ui->bcPrimeLEdit, m_d->ui->bcPrimeUMLabel, m_d->ui->bcPrimeCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setBcPrime );
    connect( m_d->QLimCPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setBcPrime );
    connect( m_d->QLimAlpha, &DoublePlus::valueChanged, this, &SoilGUI::setBcPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setBcPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setBcPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setBcPrime );
    connect( m_d->bcPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setBcPrime );
    setBcPrime();

    VarPlusGUI::connectVar( m_d->gcPrime, m_d->ui->gcPrimeLabel, m_d->ui->gcPrimeLEdit, m_d->ui->gcPrimeUMLabel, m_d->ui->gcPrimeCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setGcPrime );
    connect( m_d->QLimOmega, &DoublePlus::valueChanged, this, &SoilGUI::setGcPrime );
    connect( m_d->gcPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setGcPrime );
    setGcPrime();

    VarPlusGUI::connectVar( m_d->dcPrime, m_d->ui->dcPrimeLabel, m_d->ui->dcPrimeLEdit, m_d->ui->dcPrimeUMLabel, m_d->ui->dcPrimeCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setDcPrime );
    connect( m_d->QLimD, &DoublePlus::valueChanged, this, &SoilGUI::setDcPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setDcPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setDcPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setDcPrime );
    connect( m_d->dcPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setDcPrime );
    setDcPrime();

    VarPlusGUI::connectVar( m_d->NqPrime, m_d->ui->NqPrimeLabel, m_d->ui->NqPrimeLEdit, m_d->ui->NqPrimeUMLabel, m_d->ui->NqPrimeCheckBox );
    connect( m_d->NqPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setNqPrime );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setNqPrime );
    setNqPrime();

    VarPlusGUI::connectVar( m_d->sqPrime, m_d->ui->sqPrimeLabel, m_d->ui->sqPrimeLEdit, m_d->ui->sqPrimeUMLabel, m_d->ui->sqPrimeCheckBox );
    connect( m_d->sqPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setSqPrime );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setSqPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setSqPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setSqPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setSqPrime );
    setSqPrime();

    VarPlusGUI::connectVar( m_d->iqPrime, m_d->ui->iqPrimeLabel, m_d->ui->iqPrimeLEdit, m_d->ui->iqPrimeUMLabel, m_d->ui->iqPrimeCheckBox );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setIqPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setIqPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setIqPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setIqPrime );
    connect( m_d->QLimHd, &DoublePlus::valueChanged, this, &SoilGUI::setIqPrime );
    connect( m_d->QLimVd, &DoublePlus::valueChanged, this, &SoilGUI::setIqPrime );
    connect( m_d->iqPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setIqPrime );
    setIqPrime();

    VarPlusGUI::connectVar( m_d->bqPrime, m_d->ui->bqPrimeLabel, m_d->ui->bqPrimeLEdit, m_d->ui->bqPrimeUMLabel, m_d->ui->bqPrimeCheckBox );
    connect( m_d->bqPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setBqPrime );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setBqPrime );
    connect( m_d->QLimCPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setBqPrime );
    connect( m_d->QLimAlpha, &DoublePlus::valueChanged, this, &SoilGUI::setBqPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setBqPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setBqPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setBqPrime );
    setBqPrime();

    VarPlusGUI::connectVar( m_d->gqPrime, m_d->ui->gqPrimeLabel, m_d->ui->gqPrimeLEdit, m_d->ui->gqPrimeUMLabel, m_d->ui->gqPrimeCheckBox );
    connect( m_d->gqPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setGqPrime );
    connect( m_d->QLimOmega, &DoublePlus::valueChanged, this, &SoilGUI::setGqPrime );
    setGqPrime();

    VarPlusGUI::connectVar( m_d->dqPrime, m_d->ui->dqPrimeLabel, m_d->ui->dqPrimeLEdit, m_d->ui->dqPrimeUMLabel, m_d->ui->dqPrimeCheckBox );
    connect( m_d->dqPrime, &DoublePlus::readOnlyChanged, this, &SoilGUI::setDqPrime );
    connect( m_d->QLimPhiPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setDqPrime );
    connect( m_d->QLimD, &DoublePlus::valueChanged, this, &SoilGUI::setDqPrime );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setDqPrime );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setDqPrime );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setDqPrime );
    setDqPrime();

    VarPlusGUI::connectVar( m_d->qLimPrimeD, m_d->ui->qLimPrimeDLabel, m_d->ui->qLimPrimeDLEdit, m_d->ui->qLimPrimeDUMLabel );
    connect( m_d->qLimPrimeD, &DoublePlus::readOnlyChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->QLimqPrimed, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->QLimLInfinite, &BoolPlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->NGammaWPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->sGammaWPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->iGammaWPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->bGammaWPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->gGammaWPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->NcPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->scPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->icPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->bcPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->gcPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->dcPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->NqPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->sqPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->iqPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->bqPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->gqPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    connect( m_d->dqPrime, &DoublePlus::valueChanged, this, &SoilGUI::setqLimPrimeD );
    setqLimPrimeD();

    VarPlusGUI::connectVar( m_d->FLimPrimeD, m_d->ui->QLimPrimeDLabel, m_d->ui->QLimPrimeDLEdit, m_d->ui->QLimPrimeDUMLabel );
    connect( m_d->FLimPrimeD, &DoublePlus::readOnlyChanged, this, &SoilGUI::setFLimPrimeD );
    connect( m_d->qLimPrimeD, &DoublePlus::valueChanged, this, &SoilGUI::setFLimPrimeD );
    connect( m_d->QLimBPrime, &DoublePlus::valueChanged, this, &SoilGUI::setFLimPrimeD );
    connect( m_d->QLimLPrime, &DoublePlus::valueChanged, this, &SoilGUI::setFLimPrimeD );
    setFLimPrimeD();
}

SoilGUI::~SoilGUI() {
    delete m_d;
}

void SoilGUI::setSoil(Soil * s) {
    if( s != m_d->soil ){
        if( m_d->soil != NULL ){
            VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
            VarPlusGUI::disconnectEnumVar( m_d->ui->typeLabel, m_d->ui->typeComboBox );
            VarPlusGUI::disconnectVar( m_d->ui->gammaWLabel, m_d->ui->gammaWLEdit, m_d->ui->gammaWUMLabel);
            disconnect( m_d->soil->gammaW, &DoublePlus::valueChanged, this, &SoilGUI::setQLimGammaWd );
            VarPlusGUI::disconnectVar( m_d->ui->phiPrimeKLabel, m_d->ui->phiPrimeKLEdit, m_d->ui->phiPrimeKUMLabel );
            disconnect( m_d->soil->phiPrimeK, &DoublePlus::valueChanged, this, &SoilGUI::setKPhiPrimeD );
            disconnect( m_d->soil->phiPrimeK, &DoublePlus::valueChanged, this, &SoilGUI::setQLimPhiPrimeD );
            VarPlusGUI::disconnectVar( m_d->ui->cPrimeKLabel, m_d->ui->cPrimeKLEdit, m_d->ui->cPrimeKUMLabel );
            disconnect( m_d->soil->cPrimeK, &DoublePlus::valueChanged, this, &SoilGUI::setQLimCPrimeD );
            VarPlusGUI::disconnectVar( m_d->ui->cUKLabel, m_d->ui->cUKLEdit, m_d->ui->cUKUMLabel );
            VarPlusGUI::disconnectEnumVar( m_d->ui->KaTheoryLabel, m_d->ui->KaTheoryComboBox );
            disconnect( m_d->soil->KaTheory, &SoilKTheoryPlus::valueChanged, this, &SoilGUI::setKa );
            VarPlusGUI::disconnectEnumVar( m_d->ui->KpTheoryLabel, m_d->ui->KpTheoryComboBox );
            disconnect( m_d->soil->KpTheory, &SoilKTheoryPlus::valueChanged, this, &SoilGUI::setKp );
        }

        m_d->soil = s;

        if( m_d->soil != NULL ){
            VarPlusGUI::connectVar( m_d->soil->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
            VarPlusGUI::connectEnumVar( m_d->soil->soilType, m_d->ui->typeLabel, m_d->ui->typeComboBox );
            VarPlusGUI::connectVar( m_d->soil->gammaW, m_d->ui->gammaWLabel, m_d->ui->gammaWLEdit, m_d->ui->gammaWUMLabel );
            connect( m_d->soil->gammaW, &DoublePlus::valueChanged, this, &SoilGUI::setQLimGammaWd );

            VarPlusGUI::connectVar(m_d->soil->phiPrimeK, m_d->ui->phiPrimeKLabel, m_d->ui->phiPrimeKLEdit, m_d->ui->phiPrimeKUMLabel );
            connect( m_d->soil->phiPrimeK, &DoublePlus::valueChanged, this, &SoilGUI::setKPhiPrimeD );
            connect( m_d->soil->phiPrimeK, &DoublePlus::valueChanged, this, &SoilGUI::setQLimPhiPrimeD );

            VarPlusGUI::connectVar(m_d->soil->cPrimeK, m_d->ui->cPrimeKLabel, m_d->ui->cPrimeKLEdit, m_d->ui->cPrimeKUMLabel );
            connect( m_d->soil->cPrimeK, &DoublePlus::valueChanged, this, &SoilGUI::setQLimCPrimeD );

            VarPlusGUI::connectVar( m_d->soil->cUK, m_d->ui->cUKLabel, m_d->ui->cUKLEdit, m_d->ui->cUKUMLabel );

            VarPlusGUI::connectEnumVar( m_d->soil->KaTheory, m_d->ui->KaTheoryLabel, m_d->ui->KaTheoryComboBox );
            connect( m_d->soil->KaTheory, &SoilKTheoryPlus::valueChanged, this, &SoilGUI::setKa );

            VarPlusGUI::connectEnumVar( m_d->soil->KpTheory, m_d->ui->KpTheoryLabel, m_d->ui->KpTheoryComboBox );
            connect( m_d->soil->KpTheory, &SoilKTheoryPlus::valueChanged, this, &SoilGUI::setKp );

            VarPlusGUI::connectEnumVar( m_d->soil->soilEQCategory, m_d->ui->soilEQCategoryLabel, m_d->ui->soilEQCategoryComboBox );

            setKPhiPrimeD();
            setKa();
            setKp();

            setQLimBPrime();
            setQLimLPrime();
            setQLimGammaWd();
            setQLimPhiPrimeD();
            setQLimCPrimeD();
            setNGammaWPrime();
            setSGammaWPrime();
            setIGammaWPrime();
            setBGammaWPrime();
            setGGammaWPrime();
            setNcPrime();
            setScPrime();
            setIcPrime();
            setBcPrime();
            setGcPrime();
            setDcPrime();
            setNqPrime();
            setSqPrime();
            setIqPrime();
            setBqPrime();
            setGqPrime();
            setDqPrime();
            setqLimPrimeD();
            setFLimPrimeD();
        }
    }
}

void SoilGUI::setKPhiPrimeD(){
    if( m_d->KPhiPrimeD->readOnly() && m_d->soil != NULL ){
        m_d->soil->phiPrimeD( m_d->KPhiPrimeD, m_d->KGammaTanPhiPrime );
    }
}

void SoilGUI::setKa(){
    if( m_d->K_Ka->readOnly() && m_d->soil != NULL ){
        m_d->soil->Ka( m_d->K_Ka, m_d->KPhiPrimeD, m_d->Ki, m_d->Kbeta, m_d->Kdelta );
    }
}

void SoilGUI::setKp(){
    if( m_d->K_Kp->readOnly() && m_d->soil != NULL ){
        m_d->soil->Kp( m_d->K_Kp, m_d->KPhiPrimeD, m_d->Ki, m_d->Kbeta, m_d->Kdelta );
    }
}

void SoilGUI::setQLimGammaWd(){
    if( m_d->QLimGammaWD->readOnly() && m_d->soil != NULL ){
        m_d->soil->gammaWD( m_d->QLimGammaWD, m_d->QLimGammaGammaW );
    }
}

void SoilGUI::setQLimPhiPrimeD(){
    if( m_d->QLimPhiPrimeD->readOnly() && m_d->soil != NULL ){
        m_d->soil->phiPrimeD( m_d->QLimPhiPrimeD, m_d->QLimGammaTanPhiPrime );
    }
}

void SoilGUI::setQLimCPrimeD(){
    if( m_d->QLimCPrimeD->readOnly() && m_d->soil != NULL ){
        m_d->soil->cPrimeD( m_d->QLimCPrimeD, m_d->QLimGammaCPrime );
    }
}

void SoilGUI::setQLimHd(){
    if( m_d->QLimHd->readOnly() ){
        m_d->QLimHd->setValueNormal( pow( pow(m_d->QLimHxd->valueNormal(),2.0) + pow(m_d->QLimHyd->valueNormal(),2.0), 0.5) );
    }
}

void SoilGUI::setQLimBPrime(){
    if( m_d->QLimBPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->LPrime( m_d->QLimBPrime, m_d->QLimB, m_d->QLimMxd, m_d->QLimVd );
    }
}

void SoilGUI::setQLimLInfinite(){
    m_d->QLimL->setEnabled( !m_d->QLimLInfinite->valueNormal() );
    m_d->QLimLPrime->setEnabled( !m_d->QLimLInfinite->valueNormal() );
    if( m_d->QLimLInfinite->valueNormal() ){
        m_d->QLimL->setValueNormal( 1.0 );
    }
}

void SoilGUI::setQLimLPrime(){
    if( m_d->QLimLPrime->readOnly() && m_d->soil != NULL ){
        if( m_d->QLimLInfinite->valueNormal() ){
            m_d->QLimLPrime->setValueNormal( 1.0 );
        } else {
            m_d->soil->LPrime( m_d->QLimLPrime, m_d->QLimL, m_d->QLimMyd, m_d->QLimVd );
        }
    }
}

void SoilGUI::setNGammaWPrime(){
    if( m_d->NGammaWPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->NGammaWPrime( m_d->NGammaWPrime, m_d->QLimPhiPrimeD );
    }
}

void SoilGUI::setSGammaWPrime(){
    if( m_d->sGammaWPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->sGammaWPrime( m_d->sGammaWPrime, m_d->QLimPhiPrimeD, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite );
    }
}

void SoilGUI::setIGammaWPrime(){
    if( m_d->iGammaWPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->iGammaWPrime( m_d->iGammaWPrime, m_d->QLimPhiPrimeD, m_d->QLimCPrimeD, m_d->QLimVd, m_d->QLimHd, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite );
    }
}

void SoilGUI::setBGammaWPrime(){
    if( m_d->bGammaWPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->bGammaWPrime( m_d->bGammaWPrime, m_d->QLimPhiPrimeD, m_d->QLimAlpha );
    }
}

void SoilGUI::setGGammaWPrime(){
    if( m_d->gGammaWPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->gGammaWPrime( m_d->gGammaWPrime, m_d->QLimOmega );
    }
}

void SoilGUI::setNcPrime(){
    if( m_d->NcPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->NcPrime( m_d->NcPrime, m_d->QLimPhiPrimeD );
    }
}

void SoilGUI::setScPrime(){
    if( m_d->scPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->scPrime( m_d->scPrime, m_d->QLimPhiPrimeD, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite );
    }
}

void SoilGUI::setIcPrime(){
    if( m_d->icPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->icPrime( m_d->icPrime, m_d->QLimPhiPrimeD, m_d->QLimCPrimeD, m_d->QLimVd, m_d->QLimHd, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite );
    }
}

void SoilGUI::setBcPrime(){
    if( m_d->bcPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->bcPrime( m_d->bcPrime, m_d->QLimPhiPrimeD, m_d->QLimAlpha );
    }
}

void SoilGUI::setGcPrime(){
    if( m_d->gcPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->gcPrime( m_d->gcPrime, m_d->QLimPhiPrimeD, m_d->QLimOmega );
    }
}

void SoilGUI::setDcPrime(){
    if( m_d->dcPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->dcPrime( m_d->dcPrime, m_d->QLimPhiPrimeD, m_d->QLimD, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite );
    }
}

void SoilGUI::setNqPrime(){
    if( m_d->NqPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->NqPrime( m_d->NqPrime, m_d->QLimPhiPrimeD );
    }
}

void SoilGUI::setSqPrime(){
    if( m_d->sqPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->sqPrime( m_d->sqPrime, m_d->QLimPhiPrimeD, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite );
    }
}

void SoilGUI::setIqPrime(){
    if( m_d->iqPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->iqPrime( m_d->iqPrime, m_d->QLimPhiPrimeD, m_d->QLimCPrimeD, m_d->QLimVd, m_d->QLimHd, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite );
    }
}

void SoilGUI::setBqPrime(){
    if( m_d->bqPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->bqPrime( m_d->bqPrime, m_d->QLimPhiPrimeD, m_d->QLimAlpha );
    }
}

void SoilGUI::setGqPrime(){
    if( m_d->gqPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->gqPrime( m_d->gqPrime, m_d->QLimOmega );
    }
}

void SoilGUI::setDqPrime(){
    if( m_d->dqPrime->readOnly() && m_d->soil != NULL ){
        m_d->soil->dqPrime( m_d->dqPrime, m_d->QLimPhiPrimeD, m_d->QLimD, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite );
    }
}

void SoilGUI::setqLimPrimeD(){
    if( m_d->qLimPrimeD->readOnly() && m_d->soil != NULL ){
        m_d->soil->qLimPrime( m_d->qLimPrimeD,
                              m_d->QLimGammaWD, m_d->QLimCPrimeD, m_d->QLimqPrimed,
                              m_d->QLimBPrime, m_d->QLimBPrime, m_d->QLimLInfinite,
                              m_d->NGammaWPrime, m_d->iGammaWPrime, m_d->sGammaWPrime, m_d->bGammaWPrime, m_d->gGammaWPrime,
                              m_d->NcPrime, m_d->icPrime, m_d->scPrime, m_d->bcPrime, m_d->gcPrime, m_d->dcPrime,
                              m_d->NqPrime, m_d->iqPrime, m_d->sqPrime, m_d->bqPrime, m_d->gqPrime, m_d->dqPrime);
    }
}

void SoilGUI::setFLimPrimeD(){
    if( m_d->FLimPrimeD->readOnly() && m_d->soil != NULL ){
        m_d->soil->QLimVRdPrime( m_d->FLimPrimeD, m_d->qLimPrimeD, m_d->QLimBPrime, m_d->QLimLPrime );
    }
}
