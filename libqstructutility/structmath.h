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
#ifndef STRUCTMATH_H
#define STRUCTMATH_H

#include "qstructutility_export.h"

/** Risolve un equazione trigonometrica del tipo
    a * sin(theta) + b * cos(theta) + c = 0
    @part ret vettore di dimensione maggiore uguale di 2 su cui vengono scritte le radici reali
    @return il numero di radici reali */
int EXPORT_QSTRUCTUTILITY_LIB_OPT solveTrigonometric( double ret[], double a, double b, double c );

/** Risolve un equazione polinomiale di secondo grado del tipo
    a * x^2 + b * x + c = 0
    @part ret vettore di dimensione maggiore uguale di 2 su cui vengono scritte le radici reali
    @return il numero di radici reali */
int EXPORT_QSTRUCTUTILITY_LIB_OPT solvePoly2( double ret[], double a, double b, double c );

/** Risolve un equazione polinomiale di secondo grado, in campo complesso, del tipo
    a * x^2 + b * x + c = 0
    in cui a, b, e c sono reali
    @part realRet vettore di dimensione maggiore uguale di 2 su cui viene scritta la parte reale delle radici
    @part imRet vettore di dimensione maggiore uguale di 2 su cui viene scritta la parte immaginaria delle radici
    @return il numero di radici reali */
int EXPORT_QSTRUCTUTILITY_LIB_OPT solvePoly2( double realRet[], double imRet[], double a, double b, double c );

/** Radice reale di un equazione di terzo grado del tipo
    x^3 + p * x = q
    Si dimostra che una generica equazione di terzo grado può essere ricondotta a questa forma.
    Se l'equazione ammette almeno una radice reale, restitusce true e copia in ret
    il valore di una radice. Se l'equazione non ammette radici reali, restitusce false. */
bool EXPORT_QSTRUCTUTILITY_LIB_OPT solveCardano( double * ret, double p, double q);

/** Risolve un equazione polinomiale di terzo grado, in campo reale, del tipo
    a * x^3 + b * x^2 + c * x + d = 0
    in cui a, b, c e d sono reali
    @part ret vettore di dimensione maggiore uguale di 3 su cui vengono scritte le radici reali
    @return il numero di radici reali (non considera soluzioni con molteplicita' maggiore di 1)*/
int EXPORT_QSTRUCTUTILITY_LIB_OPT solvePoly3( double ret[], double a, double b, double c, double d );

/** Risolve un equazione polinomiale di terzo grado, in campo complesso, del tipo
    a * x^3 + b * x^2 + c * x + d = 0
    in cui a, b, c e d sono reali
    @part realRet vettore di dimensione maggiore uguale di 3 su cui viene scritta la parte reale delle radici
    @part imRet vettore di dimensione maggiore uguale di 3 su cui viene scritta la parte immaginaria delle radici
    @return il numero di radici */
int EXPORT_QSTRUCTUTILITY_LIB_OPT solvePoly3( double realRet[], double imRet[], double a, double b, double c, double d );

/** rotazione di un punto di coordinate (x,y) di un angolo a (in radianti) */
void EXPORT_QSTRUCTUTILITY_LIB_OPT rotate2D(double *x, double *y, double a );

/** Soluzione di un sistema in 2 incognite
    a11 * x + a12 * y = c1
    a21 * x + a22 * y = c2
*/
bool EXPORT_QSTRUCTUTILITY_LIB_OPT system2D( double *x, double *y, double a11, double a12, double c1, double a21, double a22, double c2 );

/** Lunghezza segmento in piano 2d */
double EXPORT_QSTRUCTUTILITY_LIB_OPT length2D( double x1, double y1, double x2, double y2);

/** punto medio */
void EXPORT_QSTRUCTUTILITY_LIB_OPT middle2D( double *x, double *y, double x1, double y1, double x2, double y2);

/** Lunghezza segmento in piano 3d */
double EXPORT_QSTRUCTUTILITY_LIB_OPT length3D( double x1, double y1, double z1, double x2, double y2, double z2);

/** punto medio */
void EXPORT_QSTRUCTUTILITY_LIB_OPT middle3D( double *x, double *y, double *z, double x1, double y1, double z1, double x2, double y2, double z2);

/** Riporta l'angolo ad un valore compreso tra -PI e +PI */
double EXPORT_QSTRUCTUTILITY_LIB_OPT normalAngle( double val, double limit = 0.0  );

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class QPointF;

double EXPORT_QSTRUCTUTILITY_LIB_OPT DyDz(const QPointF& p1, const QPointF& p2);
double EXPORT_QSTRUCTUTILITY_LIB_OPT yDyDz(const QPointF& p1, const QPointF& p2);
double EXPORT_QSTRUCTUTILITY_LIB_OPT y2DyDz(const QPointF& p1, const QPointF& p2);
double EXPORT_QSTRUCTUTILITY_LIB_OPT y3DyDz(const QPointF& p1, const QPointF& p2);
double EXPORT_QSTRUCTUTILITY_LIB_OPT zDyDz(const QPointF& p1, const QPointF& p2);
double EXPORT_QSTRUCTUTILITY_LIB_OPT z2DyDz(const QPointF& p1, const QPointF& p2);
double EXPORT_QSTRUCTUTILITY_LIB_OPT z3DyDz(const QPointF& p1, const QPointF& p2);
double EXPORT_QSTRUCTUTILITY_LIB_OPT yzDyDz(const QPointF& p1, const QPointF& p2);
double EXPORT_QSTRUCTUTILITY_LIB_OPT yz2DyDz(const QPointF& p1, const QPointF& p2);
double EXPORT_QSTRUCTUTILITY_LIB_OPT y2zDyDz(const QPointF& p1, const QPointF& p2);

double EXPORT_QSTRUCTUTILITY_LIB_OPT roundOff(double p_number, double p_precision, double p_anchor = 0);


#endif
