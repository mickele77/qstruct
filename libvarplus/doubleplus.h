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
#ifndef DOUBLEPLUS_H
#define DOUBLEPLUS_H

#include "varplus.h"

/**
* @class DoublePlus
*
* @brief Classe usata per modellizzare grandezze fisiche rappresentate con un valore reale (double)
*
* Questa classe viene impiegata per modellizzare grandezze fisiche rappresentabili con un singolo
* valore reale (double)
*
* @author Michele Mocciola
*
*/


class EXPORT_VARPLUS_LIB_OPT DoublePlus : public VarPlus
{
    Q_OBJECT
public:
    /** Costruttore
    @param vv il valore iniziale della classe */
    DoublePlus( double vv,
                const QString & nn,
                UnitMeasure * ump,
                UnitMeasure::unitMeasure um,
                bool rOnly = false );
    /** Costruttore
    @param vv il puntatore al valore reale della classe */
    DoublePlus( double * v,
                const QString & nn,
                UnitMeasure * ump,
                UnitMeasure::unitMeasure um,
                bool em = false );
    DoublePlus( DoublePlus & );

    QString typeVar();

    double valueNormal();
    double value();
    QString valueNormalStr();
    QString valueStr();

    DoublePlus & operator=( DoublePlus &);
    DoublePlus & operator=( double );
    VarPlus & operator=( VarPlus &);
    DoublePlus & operator+( DoublePlus &);
    DoublePlus & operator+( double );
    DoublePlus & operator-( DoublePlus &);
    DoublePlus & operator-( double);
    DoublePlus & operator*( double );
    bool operator==( DoublePlus &);


    /** Imposta il valore minimo della variabile */
    void setMinNormal( double );
    /** Resetta il valore minimo (non se ne tiene più conto) */
    void resetMin();
    /** Imposta il valore massimo della variabile */
    void setMaxNormal( double );
    /** Resetta il valore massimo (non se ne tiene più conto) */
    void resetMax();

public slots:
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValue( double, bool emitAuto = true );
    void setValueNormal( double, bool emitAuto = true );

private:
    static int m_precision;
    double * m_value;
    bool m_valueWasCreated;
    ~DoublePlus();

    double m_min;
    bool m_wasMinSet;
    double m_max;
    bool m_wasMaxSet;
};

#endif // DOUBLEPLUS_H
