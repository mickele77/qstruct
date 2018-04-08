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
#ifndef HYPOTHESIS_H
#define HYPOTHESIS_H

#include <Eigen/Core>

#include "unitmeasure.h"

class HypothesisPrivate;

/**
* @class Hypothesis
*
* @brief Classe usata per modellizzare le ipotesi alla base del problema elastico
*
* Questa classe viene impiegata per modellizzare le ipotesi alla base del problema elastico
*
* @author Michele Mocciola
*
*/

class Hypothesis
{
public:

    /** Costruttore
        @param dim dimensione dello spazio (numero di coordinate di un vertice)
        @param dof gradi di libertà di ciascun vertice */
    Hypothesis( int dimCaption,
                int dof );
    virtual ~Hypothesis();

    /** Dimensione dello spazio (numero di coordinate di un vertice) */
    int spaceDim();
    /** Nome assegnate a ciascuna dimensione */
    virtual QString dimCaption(int);
    /** Gradi di libertà di ciascun vertice */
    int nDOFVert();

    /** Unità di misura dello spostamento associato a ciascun grado di libertà */
    UnitMeasure::unitMeasure unitMeasureEta( int );
    /** Imposta l'unità di misura dello spostamento associato a ciascun grado di libertà
    @param i il grado di libertà di cui si vuole impostare l'unità di misura
    @param um l'unità di misura che si vuole impostare */
    void setUnitMeasureEta( int i, UnitMeasure::unitMeasure um );

    /** Unità di misura della forza associata a ciascun grado di libertà */
    UnitMeasure::unitMeasure unitMeasureF( int );
    /** Imposta l'unità di misura della forza associata a ciascun grado di libertà
    @param i il grado di libertà di cui si vuole impostare l'unità di misura
    @param um l'unità di misura che si vuole impostare */
    void setUnitMeasureF( int i, UnitMeasure::unitMeasure um );

    /** Matrice di rotazione associata a un sistema di coordinate */
    virtual Eigen::MatrixXd N( Eigen::MatrixXd & LCS) = 0;

    /** Matrice di rotazione associata a un sistema di coordinate */
    virtual Eigen::MatrixXd N( Eigen::Map<Eigen::MatrixXd> & LCS) = 0;

private:
    HypothesisPrivate * m_d;
};

#endif // HYPOTHESIS_H
