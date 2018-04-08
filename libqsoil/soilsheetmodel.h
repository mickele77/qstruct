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
#ifndef SOILSHEETMODEL_H
#define SOILSHEETMODEL_H

class SoilModel;
class SoilSheetModelPrivate;
class SoilSheet;

#include "tablemodelplus.h"

class SoilSheetModel : public TableModelPlus {
    Q_OBJECT
public:
    explicit SoilSheetModel(UnitMeasure * ump, SoilModel * sModel, QObject *parent = 0);

    void writeXml( QXmlStreamWriter * writer );
    void readXml( QXmlStreamReader * reader );

    SoilSheet * soilSheet(int);
    SoilSheet * soilSheetId( unsigned int);
    int soilSheetCount();

    void insertRows( int, int = 1 );
    void removeRows(int, int = 1 );

    SoilSheet * defaultSoilSheet();
    int defaultSoilSheetIndex();
    void setDefaultSoilSheet( int );

    SoilSheet * currentSoilSheet();
    int currentSoilSheetIndex();
    void setCurrentSoilSheet( int );

private:
    SoilSheetModelPrivate * m_dd;

    void insertSoilSheet(SoilSheet *addedRW, int position);

signals:
    void defaultChanged( SoilSheet * );
    void currentChanged( SoilSheet * );
    void soilSheetsAdded();
    void soilSheetsRemoved();
};

#endif // SOILSHEETMODEL_H
