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
#ifndef BEAMQMODEL_H
#define BEAMQMODEL_H

#include <QAbstractTableModel>
#include "beamq.h"
class BeamQModelPrivate;

class BeamQModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BeamQModel(Hypothesis * hyp, UnitMeasure * ump, QObject * parent = 0);

    UnitMeasure * unitMeasure();
    Hypothesis * hypothesis();

    int rowCount( const QModelIndex & = QModelIndex() ) const;
    int columnCount( const QModelIndex &  = QModelIndex() ) const;
    QVariant data( const QModelIndex &, int ) const;
    QVariant headerData(int, Qt::Orientation,
                        int = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role );

    BeamQ * q(int);
    int nQ();
    bool insert( int, int = 1 );
    bool append();
    bool remove(int, int = 1 );
    bool clear();

private:
    BeamQModelPrivate * m_d;
};

#endif // BEAMQMODEL_H
