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
#ifndef MULTISECTION_H
#define MULTISECTION_H

class MultiSectionModelBase;

#include "section.h"

class EXPORT_SECTION_LIB_OPT MultiSection : public Section
{
    Q_OBJECT

public:
    explicit MultiSection( UnitMeasure * ump,
                           Material * mat,
                           const QString & nn,
                           QObject *parent=0);

    MultiSection & operator=( MultiSection & val );
    Section & operator=( Section & val );

    MultiSectionModelBase * model();

    void suspendSignals( bool susp = true );

    void translateNormal( double yTrans, double zTrans, bool suspSignals = true );
    void rotateNormal(double rot, bool suspSignals = true );

    int nSection();
    Section * section(int);
    Section * sectionId( unsigned int);

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + m * y), unità di misura interne al programma
    * @param l
    * @param m
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unità di misura interne del programma)
    */
    double NULSNormal( double l, double my, double mz, QList<Point2DModel *> * sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + m * z), unità di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param m
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità di misura interne al programma)
    */
    void MULSNormal( double *yRet, double *zRet, double l, double my, double mz, QList<Point2DModel *> * sects = NULL );

    /** Restituisce l'oggetto grafico che rappresenta la sezione
    Se non c'e' oggetto grafico specifico restituisce NULL */
    QGraphicsItem * qGraphicsItem();

    /** Area */
    double ANormal();
    /** Momento statico rispetto all'asse y */
    double SyNormal();
    /** Momento statico rispetto all'asse z */
    double SzNormal();
    /** Momento di inerzia rispetto all'asse y */
    double IyyNormal();
    /** Momento di inerzia rispetto all'asse z */
    double IzzNormal();
    /** Momento centrifugo rispetto agli assi y e z */
    double IyzNormal();
    /** Integrale di y^3 sulla superficie della sezione */
    double Sy3DyDzNormal();
    /** Integrale di z^3 sulla superficie della sezione */
    double Sz3DyDzNormal();
    /** Integrale di y^2z sulla superficie della sezione */
    double Sy2zDyDzNormal();
    /** Integrale di yz^2 sulla superficie della sezione */
    double Syz2DyDzNormal();
    /** Peso specifico della sezione */
    double pWNormal();

private:
    void initVar();

protected:
    MultiSectionModelBase * m_model;
};

#endif // MULTISECTION_H
