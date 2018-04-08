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
#ifndef INTPLUS_H
#define INTPLUS_H

#include "varplus.h"

/**
* @class DoublePlus
*
* @brief Classe usata per modellizzare grandezze fisiche rappresentate con un valore intero (int)
*
* Questa classe viene impiegata per modellizzare grandezze fisiche rappresentabili con un singolo
* valore intero (int)
*
* @author Michele Mocciola
*
*/

class EXPORT_VARPLUS_LIB_OPT IntPlus : public VarPlus
{
    Q_OBJECT
public:
    IntPlus( int vv,
             const QString & nn,
             bool em = false );

    IntPlus( IntPlus & );

    QString typeVar();

    unsigned int valueNormal();
    unsigned int value();
    QString valueNormalStr();
    QString valueStr();

    IntPlus & operator=( IntPlus &);
    VarPlus & operator=( VarPlus &);

    /** Imposta il valore minimo della variabile */
    void setMinNormal( int );
    /** Resetta il valore minimo (non se ne tiene più conto) */
    void resetMin();
    /** Imposta il valore massimo della variabile */
    void setMaxNormal( int );
    /** Resetta il valore massimo (non se ne tiene più conto) */
    void resetMax();

public slots:
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValue( int, bool emitAuto = true );
    void setValueNormal( int, bool emitAuto = true );

private:
    int m_value;
    int m_min;
    int m_max;
    bool m_wasMinSet;
    bool m_wasMaxSet;
    ~IntPlus();
};

#endif // UINTPLUS_H
