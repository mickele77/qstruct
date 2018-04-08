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
#ifndef QSTRINGPLUS_H
#define QSTRINGPLUS_H

#include "varplus.h"
#include <QString>

class EXPORT_VARPLUS_LIB_OPT QStringPlus: public VarPlus
{
    Q_OBJECT
public:
    QStringPlus();

    QStringPlus( const QString & vv,
                const QString & nn,
                bool rOnly = false );
    QStringPlus( QStringPlus & );

    QString typeVar();

    QString valueNormalStr();
    QString valueStr();

    QStringPlus & operator=( QStringPlus &);
    VarPlus & operator=( VarPlus &);

public slots:
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true);

private:
    QString m_value;
    ~QStringPlus();

};

#endif // QSTRINGPLUS_H
