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
#ifndef SECTIONTIMBERCNCR_H
#define SECTIONTIMBERCNCR_H

#define MAX_ITER 100

#include "section.h"

class UnitMeasure;
class MaterialModel;
class SectionCncr;
class SectionTimber;

class QGraphicsItemGroup;
class QPolygonF;

class SectionTimberCncrPrivate;

class EXPORT_SECTION_LIB_OPT SectionTimberCncr: public Section
{
    Q_OBJECT

private:
    SectionTimberCncrPrivate * m_d;

    /** calcola lambda e mu in funzione di due punti di passaggio del piano di deformazione */
    bool lmP1P2( double *, double *, double, double, double, double );

public:
    SectionTimberCncr( UnitMeasure *,
                  MaterialModel *,
                  const QString & mm,
                  QObject * = 0);
    ~SectionTimberCncr();

    SectionTimberCncr & operator=( SectionTimberCncr &);
    Section & operator=( Section &);

    void translateNormal(double, double, bool suspSignals=true);
    void rotateNormal( double rot, bool suspSignals=true);

    /** La denominzione del materiale all'interno del programma (Concrete, SteelCncr, etc)
    @return la denominazione */
    QString typeSectionNormal();

    /** La denominzione del materiale per l'utente (Calcestruzzo, Acciaio per c.a., etc)
    @return la denominazione */
    QString typeSection();

    /** Sezione in cls */
    SectionCncr * sectionCncr();
    /** Sezione in acciaio */
    SectionTimber * sectionTimber();

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
    double pWNormal();


    /** massima sollecitazione di compressione nella sezione (con segno negativo) */
    double NULSMinNormal();
    /** massima sollecitazione di trazione nella sezione*/
    double NULSMaxNormal();

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + m * y), unità  di misura interne al programma
    * @param l
    * @param m
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    * @return lo sforzo normale allo stato limite ultimo (unità  di misura interne del programma)
    */
    double NULSNormal( double l, double m,
                       QList<Point2DModel *> * sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + m * y), unità  di misura interne al programma
    * @param xRet puntatore alla variabile su cui viene scritta la componente x del momento allo SLU
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param l
    * @param m
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    */
    void MULSNormal( double *xRet, double * yRet,
                     double l, double m,
                     QList<Point2DModel *> * sects = NULL );

    /** Momento resistente alla stato limite ultimo
    * @param xRet puntatore alla variabile su cui viene scritta la componente x del momento allo SLU
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param NSd sforzo normale agente
    * @param xCen ascissa punto attorno a cui si calcola il momento (udm interne)
    * @param yCen ordinata punto attorno a cui si calcola il momento (udm interne)
    * @param rot rotazione della sezione (in radianti
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    * @param accuracy accuratezza di calcolo del momento
    */
    void MULSNormal( double * xRet, double *yRet,
                     double NSd,
                     double xCen, double yCen,
                     double rot,
                     QList<Point2DModel *> * sects = NULL,
                     double accuracy = 1.0e-6);

    /** Momento resistente alla stato limite ultimo
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param NSd sforzo normale agente
    * @param cen punto attorno a cui si calcola il momento (udm interne)
    * @param rot rotazione della sezione (in radianti)
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    * @param accuracy accuratezza di calcolo del momento
    */
    void MULS( DoublePlus * yRet, DoublePlus *zRet,
               DoublePlus * NSd,
               Point2DPlus * cen,
               DoublePlus * rot,
               QList<Point2DModel *> * sects = NULL,
               DoublePlus * accuracy = NULL);

    /** Diagramma di interazione
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    QPolygonF MULSyMULSz( DoublePlus * NSd, Point2DPlus * cen, int propNP);

    /** Diagramma di interazione
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param propNP numero punti diagramma
    * @param rot rotazione della sezione
    * @return i punti del diagramma di interazione */
    QPolygonF MULSN( Point2DPlus * cen, DoublePlus * rot, int propNP);

    // **** GUI ****
    QGraphicsItem * qGraphicsItem();
};

#endif // SECTIONTIMBERCNCR_H
