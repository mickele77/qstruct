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
#ifndef POINT2DMODEL_H
#define POINT2DMODEL_H

#include <QPolygonF>

#include "qstructutility_export.h"

class EXPORT_QSTRUCTUTILITY_LIB_OPT Point2DModel: private QPolygonF {
public:
    explicit Point2DModel();

    Point2DModel( Point2DModel & val );

    // Point2DModel & operator=( const Point2DModel & cp );

    /** Distruttore */
    ~Point2DModel();

    double yNormal( int i );
    double zNormal( int i );
    void setYNormal( int i, double value );
    void setZNormal(int i, double value );
    void setYZNormal( int i, double yValue, double zValue );

    int nPoints();
    void resize( int newSize );
    void insertPoints( int position , int count=1 );
    void insertPoints( int position , int count, double yVal, double zVal );
    void appendPoints( int count, double yVal= 0.0, double zVal= 0.0 );
    void appendPoint(double yVal = 0.0 , double zVal = 0.0);
    void removePoints( int position, int count=1 );

    void translateNormal(double Dy, double Dz);
    void rotateNormal( double );

    double yMaxNormal();
    double yMinNormal();
    double zMaxNormal();
    double zMinNormal();
    bool sMinNormal(double *ret, double my, double mz);
    bool sMaxNormal(double *ret, double my, double mz);

    double ANormal();
    double SyNormal();
    double SzNormal();
    double IyyNormal();
    double IzzNormal();
    double IyzNormal();
    double Sz3DyDzNormal();
    double Sy3DyDzNormal();
    double Syz2DyDzNormal();
    double Sy2zDyDzNormal();

    /**
    @return Restituisce un oggetto di tipo QPolygonF contenente i punti del modello convertiti in QPointF
    @param closed se è vero in coda viene copiato il primo punto del modello per chiudere la linea
    */
    QPolygonF qPolygonF( double scale, bool closed = true );

    /** Taglia il modello di punti in due modelli di punti, sectPlus e sectMinus.
        Su sectPlus l + my * y + mz * z è maggiore o uguale a t,
        su sectMinus l + my * y + mz * z è minore o uguale a t */
    void cut( Point2DModel * sectPlus, Point2DModel * sectMinus, double l, double my, double mz, double t = 0.0);

    /** Taglia la sezione con piani e0, e1, e2 rispetto allo stato deformativo e = l + my * z + mz * y
     * @param sect1 sezione tra e0 ed e1
     * @param sect2 sezione tra e1 ed e2 */
    void calculateSects(const double &l, const double &my, const double &mz,
                        const double &eMinus, const double &ePlus,
                        Point2DModel *sectMinus, Point2DModel *sectEl, Point2DModel *sectPlus);

    void calculateSects(const double &l, const double &my, const double &mz,
                        QList<double> &eVal, QList<Point2DModel *> &sects );

private:

    /** Restituisce un numero corrispondente alla situzione tensionale nel punto di ordinata y
     * se e1 < e2
     * 0 - e < e1
     * 1 - e1 < e < e2
     * 2 - e > e2
     * se e1 > e2
     * 0 - e > e1
     * 1 - e2 < e < e1
     * 2 - e < e2 */
    static int where(double y, double z, double l, double my, double mz, double e1, double e2);

    static int where(double y, double z, double l, double my, double mz, QList<double> &eVal );

};

#endif // POINT2DMODEL_H
