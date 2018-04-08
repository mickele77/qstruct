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
#ifndef TIMBERCONNECTIONMODEL_H
#define TIMBERCONNECTIONMODEL_H

#include "timberconnection_export.h"
#include "tablemodelplus.h"

class TimberConnectionModelPrivate;
class MaterialModel;

#include "timberconnection.h"

class EXPORT_TIMBERCONNECTION_LIB_OPT TimberConnectionModel : public TableModelPlus {
    Q_OBJECT
public:
    explicit TimberConnectionModel(UnitMeasure * ump, MaterialModel * matModel, QObject *parent = 0);

    void writeXml( QXmlStreamWriter * writer );
    void readXml( QXmlStreamReader * reader );

    TimberConnection * timberConnection(int);
    TimberConnection * timberConnectionId( unsigned int);
    TimberConnection * timberConnection(const QModelIndex & );
    int timberConnectionCount();

    void insertRows( int position, int count = 1 );
    void removeRows(int position, int count = 1 );

    TimberConnection * currentTimberConnection();
    int currentTimberConnectionIndex();
    void setCurrentTimberConnection( int );

private:
    TimberConnectionModelPrivate * m_dd;
    void insertConnection(TimberConnection *connection, int position);

signals:
    void currentChanged( TimberConnection * );
};

#endif // TIMBERCONNECTIONMODEL_H
