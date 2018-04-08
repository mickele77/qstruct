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
#ifndef SECTIONTHIN_H
#define SECTIONTHIN_H

#include "section.h"

#define WPLPREC 1.0e-3

class SectionThinPrivate;

class MaterialModel;
class MultiSectionLineModel;
class MultiSectionArcModel;
class SectionLine;
class SectionArc;

/**
* @class SectionThin (
*
* @brief Classe usata per modellizzare una generica sezione sottile
*
* Questa classe viene impiegata per modellizzare una generica sezione sottile
* (i.e. spessore molto minore delle dimensioni trasversali della sezion)
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionThin : public Section {
    Q_OBJECT

public:
    /** Costruttore */
    explicit SectionThin(UnitMeasure * ump,
                         Material * mat,
                         const QString & nn,
                         QObject *parent=0);

    /** Costruttore per cerazione da attributi XML */
    SectionThin(UnitMeasure * ump,
                const QXmlStreamAttributes & attrs,
                MaterialModel * matModel, SectionLoadPhaseModel *lmodel,
                QObject * parent = 0);

    ~SectionThin();

    SectionThin & operator=( SectionThin &);
    Section & operator=( Section &);

    /** Legge da flusso XML i dati necessari per la sezione. */
    void readXml( QXmlStreamReader * reader );

    /** Scrive la sezione su flusso XML */
    void writeXml(QXmlStreamWriter *writer);

    MultiSectionLineModel * sectionLineModel();
    MultiSectionArcModel * sectionArcModel();

    void translateNormal(double yTrans, double zTrans , bool suspSignals=true);
    void rotateNormal(double rot , bool suspSignals=true);

    int nSectionLine();
    SectionLine * sectionLine(int);
    SectionLine * sectionLineId( unsigned int);

    int nSectionArc();
    SectionArc * sectionArc(int);
    SectionArc * sectionArcId( unsigned int);

    /** Restituisce l'oggetto grafico che rappresenta la sezione
    Se non c'e' oggetto grafico specifico restituisce NULL */
    QGraphicsItem * qGraphicsItem();

    /** Spessore degli elementi della sezione (viene considerato solo se
        la variabile t dell'elemento considerato è impostato su automatic) */
    DoublePlus * t;

    /** Area */
    double ANormal();
    /** Momento statico rispetto all'asse y */
    double SyNormal();
    /** Momento statico rispetto all'asse z */
    double SzNormal();
    /** Momento di inerzia rispetto all'asse y */
    double IyyNormal();
    /** Momento di inerzia rispetto all'asse z */
    double IzzNormal();
    /** Momento centrifugo rispetto agli assi y e z */
    double IyzNormal();
    /** Integrale di y^3 sulla superficie della sezione */
    double Sy3DyDzNormal();
    /** Integrale di z^3 sulla superficie della sezione */
    double Sz3DyDzNormal();
    /** Integrale di y^2z sulla superficie della sezione */
    double Sy2zDyDzNormal();
    /** Integrale di yz^2 sulla superficie della sezione */
    double Syz2DyDzNormal();
    /** Calcola il fattore di rigidezza torsionale della sezione (effettua il
        calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double ITNormal();
    /** Integrale di yz^2 sulla superficie della sezione */
    double pWNormal();

    /** Modulo resistente elastico massimo a flessione - asse y-y */
    double WMyNormal();
    /** Modulo resistente elastico massimo a flessione - asse z-z */
    double WMzNormal();

    /** Modulo resistente plastico a flessione - asse y-y */
    double WPlMyNormal();
    /** Modulo resistente plastico a flessione - asse z-z */
    double WPlMzNormal();
    /** Modulo resistente plastico a flessione - momento parallelo vettore (ny, nz)  */
    double WPlNormal( double ny, double nz );

    /** Imposta il materiale della sezione
      @param mat il nuovo materiale
      @param emitAuto emetti il segnale modelChanged */
    void setMaterial( Material * mat, bool emitAuto = true );

    /** Calcola e restituisce l'ascissa massima della sezione
      * @return il valore normale della massima ascissa della sezione */
    double yMaxNormal();
    /** Calcola e restituisce l'ascissa minima della sezione
      * @return il valore normale della minima ascissa della sezione */
    double yMinNormal();
    /** Calcola e restituisce l'ordinata massima della sezione
      * @return il valore normale della massima ordinata della sezione */
    double zMaxNormal();
    /** Calcola e restituisce l'ordinata minima della sezione
      * @return il valore normale della minima ordinata della sezione */
    double zMinNormal();

    void clear(bool emitAuto = true);

protected:
    SectionThinPrivate * m_d;

protected slots:
    /** Imposta il valore dello spessore degli elementi componenti la sezione */
    void setTNormal();
    void setTNormalModelLine();
    void setTNormalModelArc();

private:
    void initVar();

    /** Calcola parametro per calcolo modulo plastico flessionale */
    double WPlNormalT( double theta );
};

#endif // SECTIONTHIN_H
