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
#ifndef POINT2DPLUSMODEL_H
#define POINT2DPLUSMODEL_H


class VarPlus;
class Point2DPlus;
class DoublePlus;
template <typename T> class QList;
class QPolygonF;
class QXmlStreamAttributes;
class Point2DModel;

class Point2DPlusModelPrivate;
#include "unitmeasure.h"
#include "tablemodelplus.h"

class EXPORT_VARPLUS_LIB_OPT Point2DPlusModel : public TableModelPlus {
    Q_OBJECT
public:
    explicit Point2DPlusModel(UnitMeasure * ump, UnitMeasure::unitMeasure um, const QString &yH, const QString &zH, QObject *parent = NULL);

    Point2DPlusModel( Point2DPlusModel & val );

    /** Funzione statica che richiama il distruttore della classe
    @param var la variabile da terminare
    */
    static void deleteModel( Point2DPlusModel* var );

    Point2DPlusModel & operator=( Point2DPlusModel &);

    void writeXml(QXmlStreamWriter *writer);
    void readXml( QXmlStreamReader * reader );

    Point2DPlus * P( const int & );
    int pCount();

    void insertRows( int position , int count=1 );
    void insertRows( int position , int count, double yVal, double zVal );
    void insertRows( const QXmlStreamAttributes &attrs, int position = -1, int count = 1);
    void appendRows( int count, double yVal= 0.0, double zVal= 0.0 );
    void appendPoint( double = 0.0, double = 0.0 );
    void removeRows( int position, int count=1 );

    void translate( Point2DPlus * );
    void translateNormal( double, double );
    void rotate( DoublePlus * );
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
    QPolygonF qPolygonF( bool closed = true );

    /** Taglia il modello di punti in due modelli di punti, sectPlus e sectMinus.
        Su sectPlus l + my * y + mz * z è maggiore o uguale a t,
        su sectMinus l + my * y + mz * z è minore o uguale a t */
    void cut( Point2DPlusModel * sectPlus, Point2DPlusModel * sectMinus, double l, double my, double mz, double t = 0.0);

    void suspendSignals( bool susp );

    void calculateSects(const double &l, const double &my, const double &mz, QList<double> &eVal, QList<Point2DModel *> sects);

private slots:
    void updateHeaders();
    void updateHeadersUM(UnitMeasure::unitMeasure u);

private:
    /** Distruttore */
    ~Point2DPlusModel();

    Point2DPlusModelPrivate * m_dd;

    Point2DPlus * createPoint(double yVal = 0.0, double zVal = 0.0);
    void insertRows(Point2DPlus *addedP, int position=-1);

    static int where(double y, double z, double l, double my, double mz, QList<double> &eVal);
};

#endif // POINT2DPLUSMODEL_H
