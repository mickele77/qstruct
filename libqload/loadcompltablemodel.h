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
#ifndef LOADCOMPLTABLEMODEL_H
#define LOADCOMPLTABLEMODEL_H

#include "tablemodelplus.h"

#include "loadcompl.h"
class UnitMeasure;
class LoadComplTableModelPrivate;

class LoadComplTableModel : public TableModelPlus
{
    Q_OBJECT
public:
    explicit LoadComplTableModel(UnitMeasure *, QObject *parent = 0);
    ~LoadComplTableModel();

    void writeXml(QXmlStreamWriter *writer);
    void readXml( QXmlStreamReader * reader );

    LoadCompl * load(int);
    int count();
    void insertRows(LoadType::Type lType , int position, int count = 1 );
    void removeRows(int position, int count = 1 );

private:
    LoadComplTableModelPrivate * m_dd;
};

#endif // LOADCOMPLTABLEMODEL_H
