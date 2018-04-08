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
#ifndef SECTIONTIMBERDATASETMODEL_H
#define SECTIONTIMBERDATASETMODEL_H

class UnitMeasure;
class DoublePlus;
class Point2DPlus;
class QStringPlus;
class VarPlus;
class BoolPlus;

#include "section_export.h"
#include "varpluscontainer.h"

class EXPORT_SECTION_LIB_OPT SectionTimberData: public VarPlusContainer {
public:
    SectionTimberData( UnitMeasure * ump );
    ~SectionTimberData();

    QStringPlus * IdBeam;
    QStringPlus * IdSection;
    DoublePlus * N;
    DoublePlus * Vy;
    DoublePlus * Vz;
    Point2DPlus * cenM;
    DoublePlus * Mx;
    DoublePlus * My;
    DoublePlus * Mz;
    DoublePlus * result;
    BoolPlus * verified;

private:
   void initVar( UnitMeasure * ump );
};

#include "tablemodelplus.h"

template <typename T> class QList;
class SectionTimber;
class LSType;
class ServiceClass;
class LoadDurationClass;
class QFile;

class EXPORT_SECTION_LIB_OPT SectionTimberDataSetModel : public TableModelPlus {
    Q_OBJECT
public:
    explicit SectionTimberDataSetModel(UnitMeasure * ump,
                                       LSType * lsType,
                                       ServiceClass * sClass,
                                       LoadDurationClass * ldClass,
                                       DoublePlus * kCritNy,
                                       DoublePlus * kCritNz,
                                       DoublePlus * kCritMy,
                                       DoublePlus * kCritMz,
                                       SectionTimber *sect = NULL );

    void writeXml( QXmlStreamWriter * writer );
    void readXml( QXmlStreamReader * reader );

    void insertRows( int, int = 1 );
    void appendRows( int = 1 );
    void removeRows(int, int = 1 );

    SectionTimberData * data( int i );
    int dataCount();

    void setSection( SectionTimber * sect );

public slots:
    void loadFromFile( QFile & );

    void verifyULS( int row );
    void verifyULSAll();

private slots:
    void updateHeaders();
private:
    SectionTimber * m_section;
    QList<SectionTimberData *> * m_dataContainer;
    bool isCentMMOrigin;

    LSType * m_lsType;
    ServiceClass * m_sClass;
    LoadDurationClass * m_ldClass;

    DoublePlus * m_kCritNy;
    DoublePlus * m_kCritNz;
    DoublePlus * m_kCritMy;
    DoublePlus * m_kCritMz;

    void insertData(SectionTimberData *data, int position);
};

#endif // SECTIONTIMBERDATASETMODEL_H
