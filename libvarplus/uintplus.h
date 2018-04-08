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
#ifndef UINTPLUS_H
#define UINTPLUS_H

#include "varplus.h"

class EXPORT_VARPLUS_LIB_OPT UIntPlus : public VarPlus
{
    Q_OBJECT
public:
    UIntPlus(unsigned int vv,
             const QString & nn,
             bool rOnly = false );

    UIntPlus( UIntPlus & );

    QString typeVar();

    unsigned int valueNormal();
    unsigned int value();
    QString valueNormalStr();
    QString valueStr();

    UIntPlus & operator=( UIntPlus &);
    VarPlus & operator=( VarPlus &);

public slots:
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValue( unsigned int, bool emitAuto = true );
    void setValueNormal( unsigned int, bool emitAuto = true );

private:
    unsigned int m_value;
    ~UIntPlus();
};

#endif // UINTPLUS_H
