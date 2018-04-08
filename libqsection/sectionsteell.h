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
#ifndef SECTIONSTEELL_H
#define SECTIONSTEELL_H

#include "sectionsteelprofile.h"

class SectionSteelLPrivate;
class DoublePlus;

/**
* @class Section
*
* @brief Classe usata per modellizzare una sezione in acciaio ad L
*
* Questa classe viene impiegata per modellizzare una sezione in acciaio ad L
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionSteelL : public SectionSteelProfile {
public:
    SectionSteelL(DoublePlus **p, Steel * st, UnitMeasure *ump);
    ~SectionSteelL();

    SectionSteel::SectionShape sectionShape();

    int np();
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

    double maxTNormal();
    double ITNormal();
    double IwNormal();

    double WElMyNormal();
    double WElMzNormal();

    double WPlMyNormal();
    double WPlMzNormal();

    /** Calcola l'area e la posizione dei baricentri della sezione efficace
        in caso di compressione pura */
    void EffNMinusNormal( double * AEff, double * yGEff, double * zGEff, double prec = 1.0e-3);
    double WEffMyMaxNormal( double prec = 1.0e-3 );
    double WEffMyMinNormal( double prec = 1.0e-3 );
    double WEffMzMaxNormal( double prec = 1.0e-3 );
    double WEffMzMinNormal( double prec = 1.0e-3 );

    double AVyNormal();
    double AVzNormal();

    QString NClass();
    QString MyMaxClass();
    QString MyMinClass();
    double MyClassLimitNormal();
    QString MzMaxClass();
    QString MzMinClass();
    double MzClassLimitNormal();

    QGraphicsItem * qGraphicsItem();
    void updateSectionSteelProfile();

    void classDiagram(QList<QPolygonF *> *, double NULSMax, double MyULSMax, double MzULSMax );

    void point( int, double *y, double *z );
    int nPoints();

private:
    SectionSteelLPrivate * m_d;

    QString MyClass();
    QString MzClass();

    void updateQGraphics();
    void updateSectionPoints();
};

#endif // SECTIONSTEELL_H
