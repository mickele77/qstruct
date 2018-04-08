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
#ifndef BOOLPLUS_H
#define BOOLPLUS_H

#include "varplus.h"

class EXPORT_VARPLUS_LIB_OPT BoolPlus : public VarPlus
{
    Q_OBJECT
public:
    BoolPlus( bool vv,
             const QString & nn,
             bool rOnly = false );
    BoolPlus( BoolPlus & );

    QString typeVar();

    bool valueNormal();
    bool value();
    QString valueNormalStr();
    QString valueStr();

    BoolPlus & operator=( BoolPlus &);
    VarPlus & operator=( VarPlus &);

public slots:
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValue( bool, bool emitAuto = true );
    void setValueNormal( bool, bool emitAuto = true );

signals:
    void valueChangedBool( bool );

private:
    ~BoolPlus();

    bool m_value;
};

#endif // BOOLPLUS_H
