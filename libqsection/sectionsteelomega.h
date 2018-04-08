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
#ifndef SECTIONSTEELOMEGA_H
#define SECTIONSTEELOMEGA_H

#include <QObject>

#include "sectionsteelprofile.h"

#define PREC 1.0e-4
#define MAXITER 40

class SectionSteelOmegaPrivate;

class SectionThin;
class Steel;
class Point2DPlus;
class DoublePlus;

class EXPORT_SECTION_LIB_OPT SectionSteelOmega : public SectionSteelProfile {
    Q_OBJECT
public:
    explicit SectionSteelOmega(DoublePlus **p, Steel * mat, UnitMeasure *ump);
    ~SectionSteelOmega();

    SectionSteel::SectionShape sectionShape();

    /** Numero di parametri necessari per definire la sezione */
    int np();

    /** Area della sezione */
    double ANormal();
    double SyNormal();
    double SzNormal();
    double IyyNormal();
    double IzzNormal();
    double IyzNormal();
    double Sz3DyDzNormal();
    double Sy3DyDzNormal();
    double Syz2DyDzNormal();
    double Sy2zDyDzNormal();

    double tyNormal();
    double tzNormal();
    double tyzNormal();

    /** Spessore massimo */
    double maxTNormal();
    double ITNormal();
    double IwNormal();

    double WElMyNormal();
    double WElMzNormal();
    double WPlMyNormal();
    double WPlMzNormal();
    void EffNMinusNormal( double * AEff, double * yGEff, double * zGEff, double prec = PREC );
    double WEffMyMaxNormal( double prec = PREC );
    double WEffMyMinNormal( double prec = PREC );
    double WEffMzMaxNormal( double prec = PREC );
    double WEffMzMinNormal( double prec = PREC );

    double AVyNormal();
    double AVzNormal();

    QString NClass();
    QString MyMaxClass();
    QString MyMinClass();
    QString MzMaxClass();
    QString MzMinClass();

    QGraphicsItem * qGraphicsItem();
    Point2DModel * sectionPoints();

    void updateSectionSteelProfile();

    void classDiagram(QList<QPolygonF *> *, double NULSMax, double MyULSMax, double MzULSMax);

    void point( int, double *, double * );
    int nPoints();

private:
    SectionSteelOmegaPrivate * m_d;

    void updateSectionPoints();
    void updateSectionThin();
    void updateQGraphics();

    /** Class della sezione per momento di componenti My e Mz*/
    QString MClass( double My, double Mz);

    /** Calcola il modulo resistente flessionale per un momento di componenti My e Mz */
    double WEffMyMzNormal( double My, double Mz, double prec = 1.0e-3 );

    /** Aggiorna il calcolo della sezione efficace sotto l'azione di uno sforzo normale N
        e di un momento di componenti My ed Mz ed considerando solo gli effetti dell'instabilità  locale
        Al termine del calcolo A * fyd e W * fyd ci danno lo sforzo normale massimo ed il momento agente massimo,
        proporzionali ai valori di N, My ed Mz dati come parametri
        @param A area della sezione efficace al termine del calcolo
        @param W modulo di resistenza efficace al termine del calcolo
        @param sectEff la sezione efficace, composta da: nArc + nLine * 2
        @param sectProj la sezione con i tratti rettilinei proiettati
        @param u i versori dei tratti rettilinei (da P1 a P2 di ciascun tratto
        @param My componente y della direzione del momento agente
        @param Mz componente z della direzione del momento agente */
    void calcSectEffLocalBuckling( SectionThin & sectEff,
                                   double N,
                                   double My,
                                   double Mz );

    /** Aggiorna il calcolo della sezione efficace sotto l'azione di uno sforzo normale
        di compressione e un momento di componenti y e z,
        considerando gli effetti dell'instabilità  distorsionale
        @param sectEff la sezione efficace, composta da: nArc + nLine * 2
        @param N sforzo normale di compressione
        @param My componente y del momento
        @param Mz componente z del momento */
    void calcSectEffDistorsionBuckling( SectionThin & sectEff,
                                        double N,
                                        double My,
                                        double Mz );

    /** Calcola il valore del modulo resistente efficace in presenza di uno sforzo normale N,
        considerando un momento agente di componenti My e Mz */
    double WEff( SectionThin & sectEff,
                 double N,
                 double My,
                 double Mz );

    /** Aggiorna sectEff sulla scorta dei valori di bEff calcolati */
    void calcSectEff( SectionThin &sectEff, int i, double bEff[] );

private slots:
    void setRi();
};

#endif // SECTIONSTEELOMEGA_H
