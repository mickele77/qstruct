#ifndef RETAININGWALLGUIPRIVATE_H
#define RETAININGWALLGUIPRIVATE_H

#include "varpluscontainer.h"
#include <QObject>
#include "ui_retainingwallgui.h"


class Point2DPlus;
class DoublePlus;
class BoolPlus;
class RetainingWallQSCombinationGUI;
class RetainingWall;
class SoilModel;
class RWEarthQuakeDirection;
class RetainingWallGUI;

class RetainingWallGUIPrivate: public QObject, public VarPlusContainer {
    Q_OBJECT
private:
    void initVar();

public slots:
    void setEQSS();
    void setEQbetaM();

private slots:
    void setEQS();
    void setEQaMax();
    void setEQkh();
    void setEQkv();
    void setEQtheta();

public:
    RetainingWallGUIPrivate( UnitMeasure * ump, SoilModel * sm, RetainingWallGUI *parent );
    ~RetainingWallGUIPrivate();

    RetainingWall * retainingWall;
    SoilModel * soilModel;
    RetainingWallQSCombinationGUI * A1C1CombGUI;
    RetainingWallQSCombinationGUI * A1C2CombGUI;
    RetainingWallQSCombinationGUI * A2CombGUI;
    RetainingWallQSCombinationGUI * EQA2CombGUI;
    RetainingWallQSCombinationGUI * EQA1C2CombGUI;

    /** Coefficiente parziale γR1 per capacità portante della fondazione */
    DoublePlus * gammaQLimR1;
    /** Coefficiente parziale γR1 per corrimento */
    DoublePlus * gammaSlidingR1;
    /** Coefficiente parziale γR1 per resistenza del terreno a valle */
    DoublePlus * gammaRdDownR1;
    /** Coefficiente parziale γR2 per capacità portante della fondazione */
    DoublePlus * gammaQLimR2;
    /** Coefficiente parziale γR2 per corrimento */
    DoublePlus * gammaSlidingR2;
    /** Coefficiente parziale γR2 per resistenza del terreno a valle */
    DoublePlus * gammaRdDownR2;
    /** Coefficiente parziale γR3 per capacità portante della fondazione */
    DoublePlus * gammaQLimR3;
    /** Coefficiente parziale γR3 per corrimento */
    DoublePlus * gammaSlidingR3;
    /** Coefficiente parziale γR3 per resistenza del terreno a valle */
    DoublePlus * gammaRdDownR3;

    DoublePlus * gammaTanPhiPrimeM1;
    DoublePlus * gammaCPrimeM1;
    DoublePlus * gammaCUM1;
    DoublePlus * gammaGammaWM1;

    DoublePlus * gammaTanPhiPrimeM2;
    DoublePlus * gammaCPrimeM2;
    DoublePlus * gammaCUM2;
    DoublePlus * gammaGammaWM2;

    DoublePlus * gammaG1MinEQU;
    DoublePlus * gammaG1MaxEQU;
    DoublePlus * gammaG2MinEQU;
    DoublePlus * gammaG2MaxEQU;
    DoublePlus * gammaQMinEQU;
    DoublePlus * gammaQMaxEQU;

    DoublePlus * gammaG1MinA1;
    DoublePlus * gammaG1MaxA1;
    DoublePlus * gammaG2MinA1;
    DoublePlus * gammaG2MaxA1;
    DoublePlus * gammaQMinA1;
    DoublePlus * gammaQMaxA1;

    DoublePlus * gammaG1MinA2;
    DoublePlus * gammaG1MaxA2;
    DoublePlus * gammaG2MinA2;
    DoublePlus * gammaG2MaxA2;
    DoublePlus * gammaQMinA2;
    DoublePlus * gammaQMaxA2;

    /** Carichi permanenti non strutturali agenti a monte */
    DoublePlus * Gk2;
    /** Carichi variabili agenti a monte */
    DoublePlus * Qk;
    /** Coefficiente di contemporaneità dei carichi accidentali */
    DoublePlus * psi2Q;

    /** Verifica a ribaltamento - Angolo di attrito di calcolo del terreno a monte */
    DoublePlus * overTurningSoilUpPhiPrimeD;
    /** Verifica a ribaltamento - Angolo di attrito di calcolo del terreno a valle */
    DoublePlus * overTurningSoilDownPhiPrimeD;
    /** Verifica a ribaltamento - Coesione di calcolo del terreno a monte */
    DoublePlus * overTurningSoilUpCPrimeD;
    /** Verifica a ribaltamento - Coesione di calcolo del terreno a valle */
    DoublePlus * overTurningSoilDownCPrimeD;
    /** Verifica a ribaltamento - Angolo di attrito muro-terreno */
    DoublePlus * overTurningSoilUpDeltaD;
    /** Verifica a ribaltamento - Angolo di attrito fondazione-terreno */
    DoublePlus * overTurningSoilDownDeltaD;
    /** Verifica a ribaltamento - Coefficiente spinta attiva */
    DoublePlus * overTurningKa;
    /** Verifica a ribaltamento - Ci dice se si tiene conto e meno della spinta passiva */
    BoolPlus * overTurningApplyKp;
    /** Verifica a ribaltamento - Coefficiente spinta passiva */
    DoublePlus * overTurningKp;
    /** Verifica a ribaltamento - Fattore riduttivo del coefficiente di spinta passiva */
    DoublePlus * overTurningChiKp;
    /** Verifica a ribaltamento - Coefficiente spinta passiva ridotto */
    DoublePlus * overTurningKpRed;
    /** Verifica a ribaltamento - Polo per calcolo momenti */
    Point2DPlus * overTurningC;
    /** Verifica a ribaltamento - Momento resistente */
    DoublePlus * overTurningMRd;
    /** Verifica a ribaltamento - Momento ribaltante */
    DoublePlus * overTurningMEd;


    /** Accelerazione orizzontale massima su sito di riferimento rigido */
    DoublePlus * EQag;
    /** Parametro per la definizione dell'azione sismica */
    DoublePlus * EQF0;
    /** Coefficiente di amplificazione stratigrafica */
    DoublePlus * EQSS;
    /** Coefficiente di amplificazione topografica */
    DoublePlus * EQST;
    /** Coefficiente di amplificazione */
    DoublePlus * EQS;
    /** Accelerazione orizzontale massima attesa sul sito */
    DoublePlus * EQaMax;
    /** Coefficiente di correlazione tra l'accelerazione massima e la spinta inerziale orizzontale */
    DoublePlus * EQbetaM;
    /** Coefficiente di spinta inerziale orizzontale */
    DoublePlus * EQkh;
    /** Coefficiente di spinta inerziale verticale */
    DoublePlus * EQkv;
    /** Direzione del sisma */
    RWEarthQuakeDirection * EQDirection;
    /** Inclinazione carichi in condizioni sismiche */
    DoublePlus * EQtheta;
    /** Verifica a ribaltamento - Angolo di attrito di calcolo del terreno a monte */
    DoublePlus * EQoverTurningSoilUpPhiPrimeD;
    /** Verifica a ribaltamento - Coesione di calcolo del terreno a monte */
    DoublePlus * EQoverTurningSoilUpCPrimeD;
    /** Verifica a ribaltamento - Angolo di attrito di calcolo del terreno a valle */
    DoublePlus * EQoverTurningSoilDownPhiPrimeD;
    /** Verifica a ribaltamento - Coesione di calcolo del terreno a valle */
    DoublePlus * EQoverTurningSoilDownCPrimeD;
    /** Verifica a ribaltamento - Angolo di attrito muro-terreno */
    DoublePlus * EQoverTurningSoilUpDeltaD;
    /** Verifica a ribaltamento - Angolo di attrito fondazione-terreno */
    DoublePlus * EQoverTurningSoilDownDeltaD;
    /** Verifica a ribaltamento - Coefficiente spinta attiva */
    DoublePlus * EQoverTurningKa;
    /** Verifica a ribaltamento - Ci dice se si tiene conto e meno della spinta passiva */
    BoolPlus * EQoverTurningApplyKp;
    /** Verifica a ribaltamento - Coefficiente spinta passiva */
    DoublePlus * EQoverTurningKp;
    /** Verifica a ribaltamento - Fattore riduttivo del coefficiente di spinta passiva */
    DoublePlus * EQoverTurningChiKp;
    /** Verifica a ribaltamento - Coefficiente spinta passiva ridotto */
    DoublePlus * EQoverTurningKpRed;
    /** Verifica a ribaltamento - Polo per calcolo momenti */
    Point2DPlus * EQoverTurningC;
    /** Verifica a ribaltamento - Momento resistente */
    DoublePlus * EQoverTurningMRd;
    /** Verifica a ribaltamento - Momento ribaltante */
    DoublePlus * EQoverTurningMEd;

    Ui::RetainingWallGUI *ui;
};

#endif // RETAININGWALLGUIPRIVATE_H
