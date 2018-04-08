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
#ifndef MULTISECTIONSTEELMODEL_H
#define MULTISECTIONSTEELMODEL_H

class SectionLoadPhaseModel;
class SectionSteel;
class MultiSectionSteelModel;
class MaterialModel;

#include "concrete.h"

#include "multisectionmodelbase.h"

class EXPORT_SECTION_LIB_OPT MultiSectionSteelModel: public MultiSectionModelBase {
    Q_OBJECT

public:
    MultiSectionSteelModel(UnitMeasure * ump, MaterialModel * matModel, QObject * parent = 0);

    SectionSteel * sectionSteel( int i );
    SectionSteel * sectionSteelId( unsigned int i);

    double yMaxNormal();
    double yMinNormal();
    double zMaxNormal();
    double zMinNormal();
    bool sMaxNormal(double *ret, double my, double mz);
    bool sMinNormal(double *ret, double my, double mz);

    double epsYdMax();

    void insertSection(Section *addedSectGen, int position);

    static int materialColumn();
    static int loadPhaseColumn();
private:
    Section * createSection( );
    Section * createSection(const QXmlStreamAttributes& attrs, SectionLoadPhaseModel *lModel);
    void updateHeaders();
};

#endif // MULTISECTIONSTEELMODEL_H
