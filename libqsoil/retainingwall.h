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
#ifndef RETAININGWALL_H
#define RETAININGWALL_H

class RetainingWallPrivate;
class RWEarthQuakeDirection;
class SoilModel;
class Soil;
class DoublePlus;
class BoolPlus;
class Point2DPlus;
class QGraphicsItem;

#include "varpluscontainer.h"
#include <QObject>

/**
* @class RetainingWall
*
* @brief Classe usata per modellizzare muri di contenimento
*
* Questa classe viene impiegata per modellizzare muri controterra a gravita'
*
* @author Michele Mocciola
*
*/

class RetainingWall : public QObject, public VarPlusContainer
{
    Q_OBJECT
public:
    /** Direzione del sisma su cui effettuare le verifiche */
    enum EarthQuakeDirection{
        EQup,
        EQdown
    };

    /** Costruttore */
    RetainingWall(UnitMeasure *ump, const QString &n, Soil *sDown, Soil *sUp, QObject *parent = 0);

    /** Costruttore da flusso XML */
    RetainingWall(UnitMeasure *ump, const QXmlStreamAttributes &attrs, SoilModel *sModel, QObject *parent = 0);

    /** Terreno verso valle */
    Soil * soilDown();
    /** Imposta il terreno verso valle */
    void setSoilDown( Soil * s );
    /** Z del terreno verso valle */
    DoublePlus * soilDownZ;
    /** Coefficiente di attrito muro-terrano verso valle */
    DoublePlus * soilDownDelta;
    /** Terreno verso monte */
    Soil * soilUp();
    /** Imposta il terreno verso monte */
    void setSoilUp( Soil * s );
    /** Z del terreno verso monte */
    DoublePlus * soilUpZ;
    /** Inclinazione del piano di campagna verso monte */
    DoublePlus * soilUpI;
    /** Coefficiente di attrito muro-terrano verso monte */
    DoublePlus * soilUpDelta;

    /** Inclinazione della fondazione rispetto al piano orizzontale */
    DoublePlus * baseAlpha;

    /** Peso specifico del materiale della mensola di valle */
    DoublePlus * toeGammaW;
    /** Spessore della mensola di valle, verso valle (misurata in verticale) */
    DoublePlus * toeHLeft;
    /** Spessore della mensola di valle, verso monte (misurata in verticale) */
    DoublePlus * toeHRight;
    /** Lunghezza della mensola di valle (misurata in orizzontale) */
    DoublePlus * toeB;
    /** Profondità della mensola di valle */
    DoublePlus * toeD;

    /** Peso specifico del materiale della mensola di monte */
    DoublePlus * heelGammaW;
    /** Spessore della mensola di monte, verso valle (misurata in verticale) */
    DoublePlus * heelHLeft;
    /** Spessore della mensola di monte, verso monte (misurata in verticale) */
    DoublePlus * heelHRight;
    /** Lunghezza della mensola di monte (misurata in orizzontale) */
    DoublePlus * heelB;
    /** Profondità della mensola di monte */
    DoublePlus * heelD;

    /** Peso specifico del materiale costituente lo sperone */
    DoublePlus * counterfortGammaW;
    /** Larghezza dello sperone in basso */
    DoublePlus * counterfortBBottom;
    /** Larghezza dello sperone in alto */
    DoublePlus * counterfortBTop;
    /** Altezza dello sperone */
    DoublePlus * counterfortH;
    /** Spessore dello sperone */
    DoublePlus * counterfortT;

    /** Peso specifico del materiale del dente */
    DoublePlus * keyGammaW;
    /** Ascissa del primo punto del dente */
    DoublePlus * keyY;
    /** Spessore del dente (misurata in verticale) */
    DoublePlus * keyH;
    /** Lunghezza del dente (misurata in orizzontale) */
    DoublePlus * keyB;
    /** Profondità del dente */
    DoublePlus * keyD;

    /** Peso specifico del materiale costituente il muro */
    DoublePlus * stemGammaW;
    /** Angolo rispetto alla verticale del paramento del muro verso monte */
    DoublePlus * stemBeta;
    /** Spessore del muro in basso */
    DoublePlus * stemTBottom;
    /** Spessore del muro in alto */
    DoublePlus * stemTTop;
    /** Altezza del muro */
    DoublePlus * stemH;
    /** Profondità del muro */
    DoublePlus * stemD;

    /** Determina i valori di calcolo del peso specifico del terreno */
    void calculateGammaWD( DoublePlus * soilDownGammaW, DoublePlus * soilUpGammaW, DoublePlus * gamma = NULL);
    /** Determina i valori di calcolo dell'angolo di attrito interno */
    void calculatePhiPrimeD( DoublePlus * soilDownPhiPrimeD, DoublePlus * soilUpPhiPrimeD, DoublePlus * gammaTanPhi );
    /** Determina i valori di calcolo della coesione del terreno */
    void calculateCPrimeD( DoublePlus * soilDownCPrimeD, DoublePlus * soilUpCPrimeD, DoublePlus * gammaCPrime );
    /** Determina i valori di calcolo dell'attrito muro- terreno */
    void calculateDeltaD( DoublePlus * soilDownDelta, DoublePlus * soilUpDelta, DoublePlus * gammaTanPhi );
    /** Determina i valori dei coefficientei di spinta attiva e passivi per il muro */
    void calculateK(  DoublePlus * soilUpPhiPrimeD, DoublePlus * soilUpDeltaD, DoublePlus * Ka,
                      DoublePlus * soilDownPhiPrimeD, DoublePlus * soilDownDeltaD, DoublePlus * Kp, DoublePlus *chiKp, DoublePlus *KpRed );
    /** Calcola l'angolo di inclinazione delle forze agenti sul muro in condizioni sismiche */
    void calculateEQTheta( DoublePlus * ret, DoublePlus * kh, DoublePlus * kv, RWEarthQuakeDirection * direction );
    /** Determina i valori dei coefficientei di spinta attiva e passivi per il muro, sotto azione sismica */
    void calculateEQK( DoublePlus * theta,
                       DoublePlus * soilUpPhiPrimeD, DoublePlus * soilUpDeltaD, DoublePlus * Ka,
                       DoublePlus * soilDownPhiPrimeD, DoublePlus * soilDownDeltaD, DoublePlus * Kp, DoublePlus *chiKp, DoublePlus *KpRed  );

    /** Calcola la componente verticale minima, la componente orizzontale massima
        @param VEd carico verticale agente
        @param HRdP spinta passiva
        @param HEd spinta attiva */
    void calculateSlidingVH( DoublePlus * VEd,
                             DoublePlus * HEd,
                             DoublePlus * HRdP,
                             DoublePlus * soilDownCPrimeD,
                             DoublePlus * soilUpCPrimeD,
                             DoublePlus * Ka,
                             DoublePlus * Kp,
                             DoublePlus * Gk2,
                             DoublePlus * Qk,
                             DoublePlus * gammaG1Min,
                             DoublePlus * gammaG1Max,
                             DoublePlus * gammaG2Min,
                             DoublePlus * gammaG2Max,
                             DoublePlus * gammaQMin,
                             DoublePlus * gammaQMax  );

    /** Verifica a scorrimento
    */
    void calculateSliding( DoublePlus * HRd,
                           BoolPlus *applyKp,
                           DoublePlus * HRdP,
                           DoublePlus * HEd,
                           DoublePlus * VEd,
                           DoublePlus *soilDownDeltaD,
                           DoublePlus *gammaR = NULL);

    /** Verifica capacità portante. Calcolo V, H e M. */
    void calculateQLimVHM( DoublePlus * VEd,
                           DoublePlus * HEd,
                           Point2DPlus * MdC,
                           DoublePlus * MEd,
                           DoublePlus * soilDownCPrimeD,
                           DoublePlus * soilUpCPrimeD,
                           DoublePlus * Ka,
                           BoolPlus *applyKp,
                           DoublePlus * Kp,
                           DoublePlus * HRdPMax,
                           DoublePlus * HRdP,
                           DoublePlus * Gk2,
                           DoublePlus * Qk,
                           DoublePlus * gammaG1Min, DoublePlus * gammaG1Max,
                           DoublePlus * gammaG2Min, DoublePlus * gammaG2Max,
                           DoublePlus * gammaQMin, DoublePlus *gammaQMax );

    /** Verifica a scorrimento in condizioni sismiche. Calcolo V ed H
        @param VEd carico verticale agente
        @param HRdP spinta passiva
        @param HEd spinta attiva */
    void calculateEQSlidingVH( DoublePlus * VEd,
                               DoublePlus * HEd,
                               DoublePlus * HRdP,
                               DoublePlus * soilDownCPrimeD,
                               DoublePlus * soilUpCPrimeD,
                               DoublePlus * Ka,
                               DoublePlus *Kp,
                               RWEarthQuakeDirection *EQDir,
                               DoublePlus * kv,
                               DoublePlus * kh,
                               DoublePlus * Gk2,
                               DoublePlus * Qk, DoublePlus *psi2Q );

    /** Verifica capacità portante in condizioni sismiche. Calcolo V, H ed M
        @param HRdPMax spinta passiva massima disponibile
        @param HRdP spinta passiva effettiva (nel caso non sia necessario
                mobilitare tutta la spinta passiva)*/
    void calculateEQQLimVHM( DoublePlus * VEd,
                             DoublePlus * HEd,
                             Point2DPlus * MdC,
                             DoublePlus * MEd,
                             DoublePlus * soilDownCPrimeD,
                             DoublePlus * soilUpCPrimeD,
                             DoublePlus * Ka,
                             BoolPlus *applyKp,
                             DoublePlus *Kp,
                             DoublePlus * HRdPMax,
                             DoublePlus * HRdP,
                             RWEarthQuakeDirection *EQDir,
                             DoublePlus * kv,
                             DoublePlus * kh,
                             DoublePlus * Gk2,
                             DoublePlus *Qk, DoublePlus *psi2Q );

    /** Verifica della capacità portante */
    void calculateQLim( DoublePlus * qLim, DoublePlus * VRd, DoublePlus * phiD, DoublePlus * cd, DoublePlus * gammaW, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite, DoublePlus * D,
                        DoublePlus * H, DoublePlus * V, DoublePlus * M, DoublePlus * q,
                        DoublePlus * NGammaW, DoublePlus * sGammaW, DoublePlus * iGammaW, DoublePlus * bGammaW, DoublePlus * gGammaW,
                        DoublePlus * Nc, DoublePlus * sc, DoublePlus * ic, DoublePlus * bc, DoublePlus * gc, DoublePlus * dc,
                        DoublePlus * Nq, DoublePlus * sq, DoublePlus * iq, DoublePlus * bq, DoublePlus * gq, DoublePlus * dq, DoublePlus * gammaQLim = NULL );


    /** Restituisce l'oggetto grafico che rappresenta il muro in sezione */
    QGraphicsItem * qGraphicsItem();

    /** Scrive il contenuto della classe su flusso XML */
    void writeXml(QXmlStreamWriter *writer);

    /** Legge da XML gli attributi del muro */
    void loadFromXML(const QXmlStreamAttributes &attrs, SoilModel *sModel);

public slots:
    /** Effettua la verifica a ribaltamento
        @param overTurningC polo attorno a cui vengono calcolati i momenti
        @param overTurningMEd momento instabilizzante
        @param overTurningMRd momento stabilizzante
        @param overTurningSoilDownCPrimeD coesione del terreno a valle
        @param overTurningSoilUpCPrimeD coesione del terreno a monte
        @param overTurningKa coefficiente di spinta attiva
        @param overTurningKpRed coefficiente di spinta passiva
        @param Gk2 carico permanente agente a monte
        @param Qk carico variabile agente a monte
        @param gammaG1MinEQU coefficiente di sicurezza per carichi strutturali (valore minimo)
        @param gammaG1MaxEQU coefficiente di sicurezza per carichi strutturali (valore massimo)
        @param gammaG2MinEQU coefficiente di sicurezza per carichi permanenti (valore minimo)
        @param gammaG2MaxEQU coefficiente di sicurezza per carichi permanenti (valore massimo)
        @param gammaQMinEQU coefficiente di sicurezza per carichi variabili (valore minimo)
        @param gammaQMaxEQU coefficiente di sicurezza per carichi variabili (valore massimo) */
    void calculateOverTurning( Point2DPlus * overTurningC, DoublePlus * overTurningMEd, DoublePlus * overTurningMRd,
                               DoublePlus * overTurningSoilDownCPrimeD, DoublePlus * overTurningSoilUpCPrimeD,
                               DoublePlus * overTurningKa,
                               BoolPlus * applyKp, DoublePlus * overTurningKpRed,
                               DoublePlus * Gk2, DoublePlus * Qk,
                               DoublePlus * gammaG1MinEQU, DoublePlus * gammaG1MaxEQU, DoublePlus * gammaG2MinEQU, DoublePlus * gammaG2MaxEQU, DoublePlus * gammaQMinEQU, DoublePlus * gammaQMaxEQU );
    /** Effettua la verifica a ribaltamento in condizioni sisimiche
        @param overTurningC polo attorno a cui vengono calcolati i momenti
        @param overTurningMEd momento instabilizzante
        @param overTurningMRd momento stabilizzante
        @param overTurningKa coefficiente di spinta attiva
        @param overTurningKpRed coefficiente di spinta passiva
        @param EQDir direzione del sisma (verso l'alto e verso il basso)
        @param kh azione sismica orizzontale
        @param kv azione sismica verticale
        @param Gk2 carico permanente agente a monte
        @param Qk carico variabile agente a monte
        @param psi2Q coefficiente contemporaneità dei carichi variabili in condizioni quasi-permanenti */
    void calculateEQOverTurning( Point2DPlus * MdC, DoublePlus * MEd, DoublePlus * MRd,
                                 DoublePlus * soilDownCPrimeD, DoublePlus * soilUpCPrimeD,
                                 DoublePlus *Ka, BoolPlus *applyKp, DoublePlus *Kp,
                                 RWEarthQuakeDirection *EQDir, DoublePlus * kv,
                                 DoublePlus * kh, DoublePlus *Gk2, DoublePlus *Qk, DoublePlus *psi2Q );

private:
    RetainingWallPrivate * m_d;

    void initVar();

private slots:
    void setHeelHRight();
    void setToeHRight();
    void setGammaW();
    void setD();
    void setCounterfortH();

    /** Se SoilDownDelta e SoilUpDelta sono automatici, imposta i relativi valori pari agli angoli di attrito
        interno dei terreni. */
    void setSoilDelta();

    /** Ridisegna le sezioni */
    void updateSections();

signals:
    void soilUpChanged( Soil * oldSoil, Soil * newSoil);
    void soilDownChanged( Soil * oldSoil, Soil * newSoil);
};

class RWEarthQuakeDirectionPrivate;
#include "enumplus.h"

/**
* @class RWEarthQuakeDirection
*
* @brief Classe usata per indicare la teoria con cui è sono calcolati i coefficienti di spinta
*
* Questa classe viene impiegata per determinare la teoria con cui sono calcolati i coefficienti di
* spinta
*
* @author Michele Mocciola
*
*/

class RWEarthQuakeDirection : public EnumPlus
{
    Q_OBJECT
public:
    RWEarthQuakeDirection( RetainingWall::EarthQuakeDirection dd,
                           const QString & nn,
                           bool ro = false );

    QString typeVar();

    RWEarthQuakeDirection & operator=( RWEarthQuakeDirection &);
    VarPlus & operator=( VarPlus &);

    RetainingWall::EarthQuakeDirection value();
    RetainingWall::EarthQuakeDirection valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( RetainingWall::EarthQuakeDirection, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( RetainingWall::EarthQuakeDirection, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    RWEarthQuakeDirectionPrivate * m_d;

    ~RWEarthQuakeDirection();
};

#endif // RETAININGWALL_H
