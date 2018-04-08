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
#ifndef SECTIONLINE_H
#define SECTIONLINE_H

#include "sectionthinsimple.h"

class SectionLinePrivate;

/**
* @class SectionLine
*
* @brief Classe usata per modellizzare una sezione rettangolare sottile
*
* Questa classe viene impiegata per modellizzare una sezione di forma
* rettangolare sottile (una dimensione trasvesale << dell'altra)
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionLine : public SectionThinSimple {
    Q_OBJECT
public:
    /** Costruttore */
    SectionLine( UnitMeasure * ump, Material * mat, const QString &nn, QObject * parent = 0 );
    /** Costruttore da file XML */
    SectionLine(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel *lModel, QObject *parent = 0 );

    ~SectionLine();

    SectionLine & operator=( SectionLine & );
    SectionThinSimple & operator=( SectionThinSimple & );

    /** Punto iniziale del segmento */
    Point2DPlus * P1;
    /** Punto finale del segmento */
    Point2DPlus * P2;

    void translateNormal( double yTrans, double zTrans, bool suspSignals = true );
    void rotateNormal(double rot, bool suspSignals = true );

    double LNormal();
    double GyNormal();
    double GzNormal();
    double SyNormal();
    double SzNormal();
    double IyyNormal();
    double IzzNormal();
    double IyzNormal();
    double Sz3DyDzNormal();
    double Sy3DyDzNormal();
    double Syz2DyDzNormal();
    double Sy2zDyDzNormal();
    double ITNormal();
    double IGyyNormal();
    double IGzzNormal();
    double IGyzNormal();
    double ICyyNormal();
    double ICzzNormal();
    double ICyzNormal();
    double thetaCNormal();

    double yMaxNormal();
    double yMinNormal();
    double zMaxNormal();
    double zMinNormal();

    QGraphicsItem * qGraphicsItem();

private:
    SectionLinePrivate * m_d;

    void initVar();
    void updateQGraphics();
};

#endif // SECTIONLINE_H
