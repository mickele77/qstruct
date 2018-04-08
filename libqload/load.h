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

#include "varplus.h"

#include "loadtype.h"

class LoadPhase;
class LoadGroup;

class Load: public VarPlus
{
    Q_OBJECT
public:

    Load( double vv, const QString & nn, LoadType::Type lType, LoadPhase * lph, LoadGroup * lg, UnitMeasure * ump, bool ro = false);

    QString typeVar();

    Load & operator=( Load &);
    VarPlus & operator=( VarPlus &);

    double value();
    double valueNormal();
    QString valueStr();
    QString valueNormalStr();

    LoadType * loadType;

public slots:
    void setValue( const QString & );
    void setValueNormal( const QString & );
    void setValue( double );
    void setValue( double, bool emitAuto );
    void setValueNormal( double );
    void setValueNormal( double, bool emitAuto );

private:
    double m_value;
    LoadPhase * m_phase;
    LoadGroup * m_group;

private:
    void updateUnitMeasure();
};

#endif // LOAD_H
