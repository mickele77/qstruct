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
#ifndef LOAD_H
#define LOAD_H

class BoolPlus;
class QStringPlus;
class DoublePlus;
class UnitMeasure;

#include "loaddurationclass.h"
#include "loadtype.h"

#include "varpluscontainer.h"
#include <QObject>

#define LOADCOMPL_NP 8

class LoadCompl: public QObject, public VarPlusContainer {
    Q_OBJECT

private:
    double m_value;
    double m_gamma;

public:
    LoadCompl( UnitMeasure * ump,
               const QString & nn,
               double gg,
               LoadType::Type lType,
               LoadDurationClass::Class ldc,
               QObject *parent = 0);

    LoadCompl(UnitMeasure *ump, const QXmlStreamAttributes &attrs, QObject *parent = 0);

    int nPMax();
    int nP();
    DoublePlus * P( int );
    DoublePlus * gamma;
    DoublePlus * psi(int);
    BoolPlus * variable;
    BoolPlus * permute;
    LoadType * loadType;
    LoadDurationClass * durationClass;

private:
    int m_nP;
    DoublePlus * m_P[LOADCOMPL_NP];
    DoublePlus * m_psi[3];

    void initVar();

private slots:
    void updateP();
    void updatePsi();

signals:
    void loadChanged();
};

#endif // LOAD_H
