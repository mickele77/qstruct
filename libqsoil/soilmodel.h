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
#ifndef SOILMODEL_H
#define SOILMODEL_H

class Soil;

#include "tablemodelplus.h"

class SoilModelPrivate;

class SoilModel : public TableModelPlus
{
    Q_OBJECT
public:
    enum SoilType{
        clay,
        sand
    };

    explicit SoilModel(UnitMeasure * ump, QObject *parent = 0);

    void writeXml( QXmlStreamWriter * writer );
    void readXml( QXmlStreamReader * reader );

    Soil * soil(int);
    Soil * soilId( unsigned int);

    int soilCount();
    int soilCount( SoilType );
    void insertRows( SoilType , int, int = 1 );
    void removeRows(int position, int count = 1 );

    Soil * defaultSoil();
    int defaultSoilIndex();
    void setDefaultSoil( int );

    Soil * currentSoil();
    int currentSoilIndex();
    void setCurrentSoil( int );

private:
    SoilModelPrivate * m_dd;

    Soil * createSoil( UnitMeasure *, SoilType );
    void insertSoil(Soil * addedSoil, int position );

signals:
    void defaultChanged( Soil * );
    void currentChanged( Soil * );
};

#endif // SOILMODEL_H
