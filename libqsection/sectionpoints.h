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
#ifndef SECTIONPOINTS_H
#define SECTIONPOINTS_H

#include "section.h"

class Point2DPlusModel;

class QGraphicsItem;
class QGraphicsPolygonItem;
template <typename T> class QList;

/**
* @class SectionLines (
*
* @brief Classe usata per modellizzare una sezione compatta
*
* Questa classe viene impiegata per modellizzare sezioni le cui
* superfici sono delimitate dalla poligonale definita da un insieme di punti
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionPoints : public Section
{
    Q_OBJECT
private:
    void initVar();

public:
    explicit SectionPoints( UnitMeasure *, Material *mat, const QString & n, QObject * = 0 );
    explicit SectionPoints( UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent = 0 );
    ~SectionPoints();

    virtual SectionPoints & operator=( SectionPoints &);
    Section & operator=( Section &);

    /** Scrive su file XML la sezione. */
    virtual void writeXml( QXmlStreamWriter *writer );

    /** Legge da file XML la sezione. */
    virtual void readXml( QXmlStreamReader * reader );

    void suspendSignals( bool susp = true );

    /** Trasla i punti di cui si compone la sezione
    * @param yTrans componente della traslazione
    * @param zTrans componente della traslazione */
    void translateNormal(double yTrans, double zTrans, bool suspSignals = true );

    /** Ruota attorno all'origine i punti di cui si compone la sezione
    * @param rot angolo della rotazione */
    void rotateNormal( double rot, bool suspSignals = true );

    /** Modello contenente i punti che delimitano la sezione
        Convensione adottata:
        Punti in senso antiorario -> area positiva
        Punti in senso orario -> area negativa */
    Point2DPlusModel * pointsModel();

    virtual QGraphicsItem * qGraphicsItem();

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

protected:
    /** Modello contenente i punti che delimitano la sezione
        Convenzione adottata:
        - Punti in senso antiorario -> area positiva
        - Punti in senso orario -> area negativa */
    Point2DPlusModel * m_pointsModel;

    QGraphicsPolygonItem * m_qGraphicsItem;

    /** Restituisce un numero corrispondente alla situzione tensionale nel punto di ordinata y
    * 0 - trazione
    * 1 - sezione 1
    * 2 - sezione 2
    * 3 - compressione oltre limite */
    int where( double z, double z0, double z1, double z2);

    void calculateSects(const double & l,
                        const double & m,
                        const double & eps1,
                        const double & eps2,
                        Point2DModel *sect1,
                        Point2DModel *sect2);

protected slots:
    void updateQGraphics();

signals:
    void graphicsChanged();
};

#endif // SECTIONPOINTS_H
