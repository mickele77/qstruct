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
#ifndef SECTIONPROFILE_H
#define SECTIONPROFILE_H


class Point2DPlus;

#include "sectionmodel.h"
#include "section.h"

class EXPORT_SECTION_LIB_OPT SectionProfile : public Section {
    Q_OBJECT
public:
    explicit SectionProfile( UnitMeasure * ump, Material * mat, const QString & n, QObject * parent=0 );
    explicit SectionProfile( UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent = 0 );

    /** La denominzione della sezione all'interno del programma (Section, SectionCncr, etc)
    @return la denominazione */
    SectionSpace::SectionType sectionType();

    /** Punto rispetto al quale è definita la sezione */
    Point2DPlus * D;
    /** Rotazione della sezione */
    DoublePlus * thetaD;

    /** Momento statico rispetto all'asse yD */
    DoublePlus * SDy;
    /** Momento statico rispetto all'asse zD */
    DoublePlus * SDz;
    /** Momento di inerzia rispetto all'asse yD */
    DoublePlus * IDyy;
    /** Momento di inerzia rispetto all'asse zD */
    DoublePlus * IDzz;
    /** Momento centrifugo rispetto agli assi yD e zD */
    DoublePlus * IDyz;
    /** Integrale di yD^3 sulla superficie della sezione */
    DoublePlus * SDy3DyDz;
    /** Integrale di zD^3 sulla superficie della sezione */
    DoublePlus * SDz3DyDz;
    /** Integrale di yD^2zD sulla superficie della sezione */
    DoublePlus * SDy2zDyDz;
    /** Integrale di yDzD^2 sulla superficie della sezione */
    DoublePlus * SDyz2DyDz;

    double SyNormal();
    double SzNormal();
    double IyyNormal();
    double IzzNormal();
    double IyzNormal();
    double Sy3DyDzNormal();
    double Sz3DyDzNormal();
    double Sy2zDyDzNormal();
    double Syz2DyDzNormal();

    void translateNormal(double yTrans, double zTrans);
    void rotateNormal(double rot);

private:
    void initVar();
};

#endif // SECTIONPROFILE_H
