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
#ifndef SECTIONTHINCONTPOINTSMODEL_H
#define SECTIONTHINCONTPOINTSMODEL_H

class Point2DPlus;
class DoublePlus;
class SectionThinContPointsModelPrivate;

#include "section_export.h"
#include "tablemodelplus.h"

class EXPORT_SECTION_LIB_OPT SectionThinContPointsModel : public TableModelPlus {
    Q_OBJECT

private:
    SectionThinContPointsModelPrivate * m_dd;

private slots:
    void updateHeaders();

public:
    SectionThinContPointsModel( UnitMeasure * ump, QObject *parent = NULL );
    ~SectionThinContPointsModel();

    SectionThinContPointsModel &operator =(SectionThinContPointsModel &val);

    void writeXml(QXmlStreamWriter *writer);
    void readXml(QXmlStreamReader *reader);

    void insertRows(int pos, int count = 1);
    void removeRows(int pos, int count = 1);

    int size();
    Point2DPlus * P(int i);
    DoublePlus * r(int i);
};


#endif // SECTIONTHINCONTPOINTSMODEL_H
