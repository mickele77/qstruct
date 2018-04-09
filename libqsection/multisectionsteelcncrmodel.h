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
#ifndef MULTISECTIONSTEELCNCR_H
#define MULTISECTIONSTEELCNCR_H

class SectionSteelCncr;
class MaterialModel;
class MultiSectionSteelCncrModel;

#include "steelcncr.h"

#include "multisectionmodelbase.h"

class EXPORT_SECTION_LIB_OPT MultiSectionSteelCncrModel: public MultiSectionModelBase {
    Q_OBJECT

    friend class MultipleSectionSteelCncr;

public:
    MultiSectionSteelCncrModel( UnitMeasure * ump,
                                MaterialModel *matModel,
                                QObject * parent = 0);

    double yMaxNormal();
    double yMinNormal();
    double zMaxNormal( double * corrEpsUd = NULL );
    double zMinNormal();

    double epsYdMax();
    double epsUdMin();

    void fSLSNormal( QList<double> *fSteelCncr,
                     double l, double my, double mz);

    /** Se tutte le sezioni hanno SteelCncr::inclinedTopBranch restituisce SteelCncr::inclinedTopBranch,
      * altrimenti resitituisce SteelCncr::horizontalTopBranch */
    SteelCncr::ModelFEps steelCncrModelfEps();

    void insertSection(Section *addedSectGen, int position);

    bool sMaxNormal(double *ret, double my, double mz, double *corrEpsUd = NULL );
    bool sMinNormal(double *ret, double my, double mz);

    static int materialColumn();
    static int loadPhaseColumn();

private:
    Section *createSection( );
    void updateHeaders();
};

#endif // MULTISECTIONSTEELCNCR_H
