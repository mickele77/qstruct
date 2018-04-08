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
#ifndef QSTRUCTURE_H
#define QSTRUCTURE_H

class UnitMeasure;
class MaterialModel;
class SectionModel;
class TimberConnectionModel;
class SoilModel;
class RetainingWallModel;
class SoilSheetModel;

class QXmlStreamWriter;
class QXmlStreamReader;

#include <QObject>

class QStructurePrivate;

/**
* @class QStructure
*
* @brief Classe contenitore generale
*
* Classe contenente i modelli presenti nella struttura
*
* @author Michele Mocciola
*
*/

class QStructure : public QObject
{
    Q_OBJECT
public:
    /** Costruttore */
    explicit QStructure(QObject *parent = 0);

    static QString name();

    /** Puntatore alla classe UnitMeasure, contenente le unità di misura */
    UnitMeasure * unitMeasure();
    /** Puntatore alla classe MaterialModel, contenente i materiali */
    MaterialModel * materialModel();
    /** Puntatore alla classe SectionModel, contenente le sezioni */
    SectionModel * sectionModel();
    /** Puntatore alla classe TimberConnectionModel, contenente le connessioni in legno */
    TimberConnectionModel * timberConnectionModel();
    /** Puntatore alla classe SoilModel, contenente i terreni */
    SoilModel * soilModel();
    /** Puntatore alla classe RetainingWallModel, contenente i muri di sostegno */
    RetainingWallModel * retainingWallModel();
    /** Puntatore alla classe SoilSheetModel, contenente i muri di sostegno */
    SoilSheetModel * soilSheetModel();

    /** Riazzera la struttura (leggi i suoi modelli) */
    void clear();

    /** Scrive il contenuto dell'oggetto nel formato Xml */
    void writeXml(QXmlStreamWriter *writer);

    /** Legge da file Xml */
    void readXml( QXmlStreamReader * reader );

signals:
    /** Segnale emesso quando la struttura (leggi uno dei suoi modelli) cambia */
    void structureChanged();

private:
    QStructurePrivate * m_d;
};

#endif // QSTRUCTURE_H
