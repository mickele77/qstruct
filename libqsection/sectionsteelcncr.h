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
#ifndef SECTIONSTEELCNCR_H
#define SECTIONSTEELCNCR_H

class UnitMeasure;
class SteelCncr;

#include "section.h"

class SectionSteelCncrPrivate;

class EXPORT_SECTION_LIB_OPT SectionSteelCncr : public Section
{
    Q_OBJECT
private:
    SectionSteelCncrPrivate * m_d;
    void initVar();

public:
    explicit SectionSteelCncr( UnitMeasure * ump, SteelCncr *s, const QString & n, QObject * = 0 );
    explicit SectionSteelCncr(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent = 0 );
    ~SectionSteelCncr();

    void loadFromXML(const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * loadModel );

    void translateNormal(double yTrans, double zTrans, bool suspSignals = true );
    void rotateNormal( double rot, bool suspSignals = true );

    SteelCncr * steelCncr();
    void setSteelCncr( SteelCncr * );
    void setMaterial( Material * );

    Point2DPlus * P;

    /** Tensione alle SLE */
    double fSLSNormal(double l, double my, double mz);

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + my * z), unità  di misura interne al programma
    * @param l
    * @param my
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unità  di misura interne del programma)
    */
    double NULSNormal(double l, double my, QList<Point2DModel *> * sects = NULL);

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unità  di misura interne al programma
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unità  di misura interne del programma)
    */
    double NULSNormal(double l, double my, double mz, QList<Point2DModel *> * sects = NULL);

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z), unità  di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità  di misura interne al programma)
    */
    void MULSNormal(double *yRet, double * zRet, double l, double my, QList<Point2DModel *> * sects = NULL);

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unità  di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità  di misura interne al programma)
    */
    void MULSNormal(double *yRet, double * zRet, double l, double my, double mz, QList<Point2DModel *> * sects = NULL);

    double SyNormal();
    double SzNormal();
    double IyyNormal();
    double IzzNormal();
    double IyzNormal();
    double Sz3DyDzNormal();
    double Sy3DyDzNormal();
    double Syz2DyDzNormal();
    double Sy2zDyDzNormal();

    QGraphicsItem * qGraphicsItem();

public slots:
    void updateQGraphics();
};

#endif // SECTIONSTEELCNCR_H
