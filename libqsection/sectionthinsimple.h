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
#ifndef SECTIONTHINSIMPLE_H
#define SECTIONTHINSIMPLE_H

#include "section.h"

class DoublePlus;

class EXPORT_SECTION_LIB_OPT SectionThinSimple : public Section
{
    Q_OBJECT
public:
    explicit SectionThinSimple(UnitMeasure * ump, Material * mat, const QString &n, QObject *parent = 0);

    virtual SectionThinSimple & operator=( SectionThinSimple & ) = 0;

    /** Spessore delle sezione */
    DoublePlus * t;
    /** Lunghezza del segmento */
    DoublePlus * L;

    /** Calcola la lunghezza del segmento */
    virtual double LNormal() = 0;
    /** Fattore di rigidezza torsionale */
    virtual double ITNormal() = 0;

    /** Calcola l'area del segmento */
    double ANormal();

private:
    void initVar();

    void setL( bool emitAuto );
};

#endif // SECTIONTHINSIMPLE_H
