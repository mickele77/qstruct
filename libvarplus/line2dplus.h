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
#ifndef LINE2DPLUS_H
#define LINE2DPLUS_H

#include "varplus.h"
#include "unitmeasure.h"

class QString;
class DoublePlus;
class Point2DPlus;
class QGraphicsItem;

class Line2DPlusPrivate;

class EXPORT_VARPLUS_LIB_OPT Line2DPlus: public VarPlus
{
    Q_OBJECT
public:
    Line2DPlus(  const QString & nn,
               double y1Val,
               double z1Val,
               double y2Val,
               double z2Val,
               UnitMeasure * ump,
               UnitMeasure::unitMeasure um,
               bool em = false );
    Line2DPlus( Line2DPlus & );

    QString typeVar();

    Point2DPlus * P1();
    Point2DPlus * P2();

    QString valueNormalStr();
    QString valueStr();

    QGraphicsItem * qGraphicsItem();

    Line2DPlus & operator=( Line2DPlus & val);
    VarPlus & operator=( VarPlus &);

    void setGraphicsWidth( double );

public slots:
    void setValue(const QString & );
    void setValueNormal(const QString & );

    void emitValueChanged();

    /** Setta il valore del flag booleano associato */
    virtual void setReadOnly( bool f );

    void translate( Point2DPlus * );
    void translateNormal( double, double );

    void rotate( DoublePlus * );
    void rotateNormal( double );

    void updateGraphics();

private:
    ~Line2DPlus();
    Line2DPlusPrivate * m_d;

signals:
    void  graphicsChanged();
};

#endif // LINE2DPLUS_H
