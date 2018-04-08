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
#ifndef POINT2DPLUS_H
#define POINT2DPLUS_H


class DoublePlus;
class QString;
class QXmlStreamAttributes;

#include "unitmeasure.h"
#include "varplus.h"


class EXPORT_VARPLUS_LIB_OPT Point2DPlus: public VarPlus
{
    Q_OBJECT
public:
    Point2DPlus(const QString & n,
                  const QString & yn,
                  double yVal,
                  const QString &zn,
                  double zVal,
                  UnitMeasure * ump,
                  UnitMeasure::unitMeasure um,
                  bool rOnly = false );
    Point2DPlus(const QXmlStreamAttributes &attrs, const QString &yN, const QString &zN, UnitMeasure *ump, UnitMeasure::unitMeasure um, bool em);

    Point2DPlus( Point2DPlus & );

    QString typeVar();

    DoublePlus * y;
    DoublePlus * z;

    QString valueNormalStr();
    QString valueStr();

    Point2DPlus & operator=( Point2DPlus & val);
    VarPlus & operator=( VarPlus &);
    Point2DPlus & operator+( Point2DPlus &);
    Point2DPlus & operator-( Point2DPlus &);
    Point2DPlus & operator*( double );
    bool operator==( Point2DPlus &);

    void normalize();

    void loadFromXML(const QXmlStreamAttributes &attrs);

    /** Scrive su file XML il contenuto del modello. */
    void writeXml(QXmlStreamWriter *writer);

public slots:
    void setValue(const QString &, bool emitAuto = true );
    void setValueNormal(const QString &, bool emitAuto = true );
    void setValue(double, double, bool emitAuto = true );
    void setValueNormal(double, double, bool emitAuto = true );

    void emitValueChanged();

    /** Setta il valore del flag booleano associato */
    virtual void setReadOnly( bool f );

    void translate( Point2DPlus * );
    void translateNormal( double, double );

    void rotate( DoublePlus * );
    void rotateNormal( double );

private:
    ~Point2DPlus();

    void updateValue(bool emitAuto);
};

#endif // POINT2DPLUS_H
