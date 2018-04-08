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
#ifndef BEAMFMODEL_H
#define BEAMFMODEL_H

#include <QAbstractTableModel>
#include "beamf.h"
class BeamFModelPrivate;

class BeamFModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BeamFModel(Hypothesis * hyp, UnitMeasure * ump, QObject * parent = 0);

    UnitMeasure * unitMeasure();
    Hypothesis * hypothesis();

    int rowCount( const QModelIndex & = QModelIndex() ) const;
    int columnCount( const QModelIndex &  = QModelIndex() ) const;
    QVariant data( const QModelIndex &, int ) const;
    QVariant headerData(int, Qt::Orientation,
                        int = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role );

    BeamF * f(int);
    int nF();
    bool insert( int, int = 1 );
    bool append();
    bool remove(int, int = 1 );
    bool clear();

private:
    BeamFModelPrivate * m_d;
};

#endif // BEAMFMODEL_H
