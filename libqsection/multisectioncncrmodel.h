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
#ifndef MULTISECTIONCNCRMODEL_H
#define MULTISECTIONCNCRMODEL_H

class SectionLoadPhaseModel;
class SectionCncr;
class MultiSectionCncrModel;
class MaterialModel;

#include "concrete.h"

#include "multisectionmodelbase.h"

class EXPORT_SECTION_LIB_OPT MultiSectionCncrModel: public MultiSectionModelBase {
    Q_OBJECT

public:
    MultiSectionCncrModel(UnitMeasure * ump, MaterialModel * matModel, QObject * parent = 0);

    SectionCncr * sectionCncr( int i );
    SectionCncr * sectionCncrId( unsigned int i);

    bool sMaxNormal( double * ret, double my, double mz );
    bool sMinNormal( double *ret, double my, double mz, double *corrEpsC=NULL, double *corrEpsCU=NULL );

    double yMaxNormal();
    double yMinNormal();
    double zMaxNormal();
    double zMinNormal(double *corrEpsC=NULL, double *corrEpsCU=NULL);

    double epsCMin();
    double epsCUMin();

    static int materialColumn();
    static int loadPhaseColumn();

private slots:
    void updateHeaders();

private:
    Section * createSection( );
    Section * createSection(const QXmlStreamAttributes& attrs , SectionLoadPhaseModel *lModel);
};

#endif // MULTISECTIONCNCRMODEL_H
