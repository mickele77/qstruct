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
#ifndef POINT3DPLUS_H
#define POINT3DPLUS_H

#include "varplus.h"
#include "unitmeasure.h"

class QString;
class DoublePlus;

class EXPORT_VARPLUS_LIB_OPT Point3DPlus: public VarPlus
{
    Q_OBJECT
public:
    Point3DPlus(  const QString & n,
                const QString & xn,
                double xVal,
                const QString & yn,
                double yVal,
                const QString &zn,
                double zVal,
                UnitMeasure * ump,
                UnitMeasure::unitMeasure um,
                bool em = false );
    Point3DPlus( Point3DPlus & );

    QString typeVar();

    DoublePlus * x;
    DoublePlus * y;
    DoublePlus * z;

    QString valueNormalStr();
    QString valueStr();

    Point3DPlus & operator=( Point3DPlus & val);
    VarPlus & operator=( VarPlus &);

public slots:
    void setValue(const QString &, bool emitAuto = true );
    void setValueNormal(const QString &, bool emitAuto = true );
    void setValue(double, double, double, bool emitAuto = true );
    void setValueNormal(double, double, double, bool emitAuto = true );

    void emitValueChanged();

    /** Setta il valore del flag booleano associato */
    virtual void setReadOnly( bool f );

    void translate( Point3DPlus * );
    void translateNormal( double, double, double );

    // void rotate( DoublePlus * );
    // void rotateNormal( double );

protected:
    ~Point3DPlus();
};

#endif // POINT3DPLUS_H
