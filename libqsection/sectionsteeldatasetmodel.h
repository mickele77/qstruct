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
#ifndef SECTIONSTEELDATASETMODEL_H
#define SECTIONSTEELDATASETMODEL_H

class UnitMeasure;
class QStringPlus;
class DoublePlus;
class Point2DPlus;
class SectionSteelTorsionalDeformability;
class SectionSteelMomentDistribution;
class VarPlus;
class BoolPlus;
class QString;

#include "section_export.h"
#include "varpluscontainer.h"

class EXPORT_SECTION_LIB_OPT SectionSteelData: public VarPlusContainer {
public:
    SectionSteelData( UnitMeasure * um );
    ~SectionSteelData();

    QStringPlus * IdBeam;
    QStringPlus * IdSection;
    DoublePlus * N;
    DoublePlus * Vz;
    DoublePlus * Vy;
    Point2DPlus * cenM;
    DoublePlus * My;
    DoublePlus * Mz;
    DoublePlus * Mx;
    SectionSteelMomentDistribution * beamMyDistribution;
    DoublePlus * beamMyH1;
    DoublePlus * beamMyS;
    DoublePlus * beamMyH2;
    SectionSteelMomentDistribution * beamMzDistribution;
    DoublePlus * beamMzH1;
    DoublePlus * beamMzS;
    DoublePlus * beamMzH2;
    SectionSteelMomentDistribution * beamMLTDistribution;
    DoublePlus * beamMLTH1;
    DoublePlus * beamMLTS;
    DoublePlus * beamMLTH2;
    DoublePlus * beamMCr;
    DoublePlus * beamlRelLT;
    DoublePlus * beamChiLT;
    DoublePlus * result;
    BoolPlus * verified;

private:
    void initVar();
};

#include "tablemodelplus.h"

template <typename T> class QList;
class SectionSteel;
class QFile;

class EXPORT_SECTION_LIB_OPT SectionSteelDataSetModel : public TableModelPlus
{
    Q_OBJECT
public:
    explicit SectionSteelDataSetModel(UnitMeasure * ump,
                                      DoublePlus * lRelY,
                                      DoublePlus * chiY,
                                      DoublePlus * lRelz,
                                      DoublePlus * chiZ,
                                      SectionSteelTorsionalDeformability * torsDef,
                                      DoublePlus * L0LT,
                                      SectionSteel *sect = NULL );

    /** Scrive su file XML i risultati dell'analisi */
    void writeXml( QXmlStreamWriter * writer );
    /** */
    void readXml( QXmlStreamReader * reader );

    void insertRows( int, int = 1 );
    void appendRows( int = 1 );
    void removeRows(int, int = 1 );

    int dataCount();
    SectionSteelData * data( int i );
    void setSection( SectionSteel * sect );

public slots:
    void loadFromFile( QFile & );

    void verifyULS( int row );
    void verifyULSAll();

private slots:
    void updateHeaders();
private:
    SectionSteel * m_section;
    QList<SectionSteelData *> * m_dataContainer;

    DoublePlus * m_lRelY;
    DoublePlus * m_chiY;
    DoublePlus * m_lRelZ;
    DoublePlus * m_chiZ;
    SectionSteelTorsionalDeformability * m_torsDef;
    DoublePlus * m_L0LT;

    void insertData(SectionSteelData *data, int position);
};

#endif // SECTIONSTEELDATASETMODEL_H
