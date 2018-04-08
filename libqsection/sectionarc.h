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
#ifndef SECTIONARC_H
#define SECTIONARC_H

#include "sectionthinsimple.h"

class SectionArcPrivate;

/**
* @class SectionLine
*
* @brief Classe usata per modellizzare una sezione rettangolare ad arco
*
* Questa classe viene impiegata per modellizzare una sezione avente la forma
* di un arco di circonferenza, sottile (una dimensione trasvesale << dell'altra)
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionArc : public SectionThinSimple
{
    Q_OBJECT
public:
    /** Costruttore */
    SectionArc(UnitMeasure *ump, Material *mat, const QString &nn, QObject *parent = 0);
    /** Costruttore da file XML */
    SectionArc(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent = 0);

    ~SectionArc();

    SectionArc & operator=( SectionArc & );
    SectionThinSimple & operator=( SectionThinSimple & );

    /** La denominzione della sezione all'interno del programma (Section, SectionCncr, etc)
    @return la denominazione */
    SectionSpace::SectionType sectionType();

    /** Angolo iniziale dell'arco */
    DoublePlus * startAngle;

    /** Angolo spazzato dall'arco */
    DoublePlus * sweepAngle;

    /** Centro dell'arco */
    Point2DPlus * C;

    /** Raggio dell'arco */
    DoublePlus * R;

    void translateNormal( double yTrans, double zTrans, bool suspSignals = true );

    void rotateNormal(double rot, bool suspSignals = true );

    double LNormal();
    double SyNormal();
    double SzNormal();
    double IzzNormal();
    double IyyNormal();
    double IyzNormal();
    double Sz3DyDzNormal();
    double Sy3DyDzNormal();
    double Syz2DyDzNormal();
    double Sy2zDyDzNormal();
    double ITNormal();

    QGraphicsItem * qGraphicsItem();

public slots:
    void updateQGraphics();

private:
    SectionArcPrivate * m_d;

    void initVar();
};

#endif // SECTIONARC_H
