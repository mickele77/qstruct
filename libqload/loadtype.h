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
#ifndef LOADTYPE_H
#define LOADTYPE_H

class LoadTypePrivate;
#include "enumplus.h"

class LoadType : public EnumPlus
{
    Q_OBJECT
public:
    enum Type{
        loadPoint,
        loadLine,
        loadArea,
        loadVolume
    };
    LoadType( LoadType::Type tt,
             const QString & nn,
             bool ro = false );

    QString typeVar();

    LoadType & operator=( LoadType &);
    VarPlus & operator=( VarPlus &);

    Type value();
    Type valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    static int enumCountStatic();
    static QString enumStrStatic( int );

    int valueNormalInt();

public slots:
    void setValue( Type, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( Type, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    LoadTypePrivate * m_d;

    ~LoadType();
};


#endif // LOADTYPE_H
