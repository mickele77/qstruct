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
#ifndef SECTIONSTEELCIRC_H
#define SECTIONSTEELCIRC_H

#include "sectionsteelprofile.h"

class SectionSteelCircPrivate;
class DoublePlus;

class EXPORT_SECTION_LIB_OPT SectionSteelCirc : public SectionSteelProfile {
public:
    explicit SectionSteelCirc( DoublePlus **p, Steel *st, UnitMeasure * ump );
    ~SectionSteelCirc();
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
    void EffNMinusNormal( double * AEff, double * yGEff, double * zGEff, double = 1.0e-3);
    double WEffMyMaxNormal( double = 1.0e-3 );
    double WEffMyMinNormal( double = 1.0e-3 );
    double WEffMzMaxNormal( double = 1.0e-3 );
    double WEffMzMinNormal( double = 1.0e-3 );

    double AVyNormal();
    double AVzNormal();

    QString NClass();
    QString MyMaxClass();
    QString MyMinClass();
    QString MzMaxClass();
    QString MzMinClass();

    QGraphicsItem * qGraphicsItem();
    void updateSectionSteelProfile();

    void classDiagram(QList<QPolygonF *> *, double NULSMax, double MyULSMax, double MzULSMax);

    void point(int, double *, double * zCopy);
    int nPoints();

private:
    SectionSteelCircPrivate * m_d;

    QString MyClass();
    QString MzClass();


    void updateQGraphics();
    void updateSectionPoints();
};

#endif // SECTIONSTEELCIRC_H
