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
#ifndef SOILSHEET_H
#define SOILSHEET_H

class SoilSheetPrivate;
class SoilModel;
class Soil;
class DoublePlus;
class BoolPlus;
class Point2DPlus;
class QGraphicsItem;

#include "varpluscontainer.h"
#include <QObject>

/**
* @class SoilSheet
*
* @brief Classe usata per modellizzare paratie
*
* Questa classe viene impiegata per modellizzare paratie
*
* @author Michele Mocciola
*
*/

class SoilSheet : public QObject, public VarPlusContainer
{
    Q_OBJECT
public:
    /** Costruttore */
    SoilSheet(UnitMeasure *ump, const QString &n, Soil *sDown, Soil *sUp, QObject *parent = 0);

    /** Costruttore da flusso XML */
    SoilSheet(UnitMeasure *ump, const QXmlStreamAttributes &attrs, SoilModel *sModel, QObject *parent = 0);

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
    /** Coefficiente di attrito muro-terrano verso monte */
    DoublePlus * soilUpDelta;

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


    /** Restituisce l'oggetto grafico che rappresenta il muro in sezione */
    QGraphicsItem * qGraphicsItem();

    /** Scrive il contenuto della classe su flusso XML */
    void writeXml(QXmlStreamWriter *writer);

    /** Legge da XML gli attributi del muro */
    void loadFromXML(const QXmlStreamAttributes &attrs, SoilModel *sModel);

public slots:

private:
    SoilSheetPrivate * m_d;

    void initVar();

private slots:
    /** Se SoilDownDelta e SoilUpDelta sono automatici, imposta i relativi valori pari agli angoli di attrito
        interno dei terreni. */
    void setSoilDelta();

signals:
    void soilUpChanged( Soil * oldSoil, Soil * newSoil);
    void soilDownChanged( Soil * oldSoil, Soil * newSoil);
};

#endif // SOILSHEET_H
