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
#ifndef SECTIONCNCR_H
#define SECTIONCNCR_H

#include "sectionpoints.h"

class UnitMeasure;
class Concrete;
class Point2D;

class EXPORT_SECTION_LIB_OPT SectionCncr : public SectionPoints
{
    Q_OBJECT

public:
    explicit SectionCncr( UnitMeasure *, Concrete *, const QString &, QObject * = 0 );
    explicit SectionCncr(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent = 0 );
    virtual ~SectionCncr();

    Concrete * concrete();
    void setMaterial( Material *, bool emitAuto = true );
    void setConcrete( Concrete *, bool emitAuto = true );

    /** La denominzione della sezione all'interno del programma (Section, SectionCncr, etc)
    @return la denominazione */
    SectionSpace::SectionType sectionType();

    /** La denominzione del materiale per l'utente (Calcestruzzo, Acciaio per c.a., etc)
    @return la denominazione */
    QString typeSection();

    double fMinSLSNormal( double l, double my, double mz);

    /**
    * Momento allo SLE con deformazione data (deformazione piana e = l + my * z + mz * y), unità di misura interne al programma
    * @param xRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param yRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param mz
    * @param sects zone in compressione con andamento parabolico e costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità di misura interne al programma)
    */
    void NMSLSNormal(double *NRet, double *MyRet, double * MzRet,
                    double l, double my, double mz,
                    QList<Point2DModel *> *sectsOut = NULL );

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + my * z), unità di misura interne al programma
    * @param l
    * @param my
    * @param sects zone con andamento parabolico e costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unità di misura interne del programma)
    */
    double NULSNormal(double l, double my, QList<Point2DModel *> * sects = NULL );

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unità di misura interne al programma
    * @param l
    * @param my
    * @param mz
    * @param sects zone con andamento parabolico e costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unità di misura interne del programma)
    */
    double NULSNormal( double l, double my, double mz, QList<Point2DModel *> * sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z), unità di misura interne al programma
    * @param xRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param yRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param m
    * @param sects zona con andamento parabolico e costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità di misura interne al programma)
    */
    void MULSNormal(double *yRet, double * zRet, double l, double m, QList<Point2DModel *> *sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unità di misura interne al programma
    * @param xRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param yRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param mz
    * @param sects zona con andamento parabolico e costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità di misura interne al programma)
    */
    void MULSNormal(double *yRet, double * zRet,
                    double l, double my, double mz,
                    QList<Point2DModel *> *sects = NULL );

    void loadFromXML(const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel *loadModel);

public slots:
    void setRectangular( DoublePlus *, DoublePlus * );
    void setRectangularNormal( double, double );

private:
    void initVar();
};

#endif // SECTIONCNCR_H
