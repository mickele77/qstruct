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
#ifndef SECTIONSTEELPROFILE_H
#define SECTIONSTEELPROFILE_H

class Steel;
class UnitMeasure;
class QString;
class QGraphicsItem;
class Point2DModel;

template <typename T> class QList;
class QPolygonF;

#include "sectionsteel.h"

#include <QObject>

#include "section_export.h"

class EXPORT_SECTION_LIB_OPT SectionSteelProfile: public QObject {
    Q_OBJECT
public:
    SectionSteelProfile( Steel *, UnitMeasure * );
    virtual ~SectionSteelProfile();

    virtual SectionSteel::SectionShape sectionShape() = 0;

    /** Classe di parti interne soggette a compressione
    @param c lunghezza della parte interna
    @param t spessore della parte interna
    @param e1 deformazione al primo estremo, segni come da EC3 (compressione +, trazione -)
    @param e2 deformazione al secondo estremo, segni come da EC3 (compressione +, trazione -)
    @return la classe della parte interna */
    int internalPartClass( double c, double t, double e1, double e2);

    /** Classe di flange sporgenti soggette a compressione
    @param c lunghezza della flangia
    @param t spessore della flangia
    @param e1 deformazione all'estremo vincolato, segni come da EC3 (compressione +, trazione -)
    @param e2 deformazione all'estremo libero, segni come da EC3 (compressione +, trazione -)
    @return la classe della flangia */
    int outstandFlangeClass( double c, double t, double e1, double e2);

    virtual int np() = 0;
    virtual double ANormal() = 0;
    virtual double SyNormal() = 0;
    virtual double SzNormal() = 0;
    virtual double IyyNormal() = 0;
    virtual double IzzNormal() = 0;
    virtual double IyzNormal() = 0;
    virtual double Sz3DyDzNormal() = 0;
    virtual double Sy3DyDzNormal() = 0;
    virtual double Syz2DyDzNormal() = 0;
    virtual double Sy2zDyDzNormal() = 0;
    virtual double tyNormal() = 0;
    virtual double tzNormal() = 0;
    virtual double tyzNormal() = 0;

    /** Spessore maggiore presente nella sezione */
    virtual double maxTNormal() = 0;

    virtual double ITNormal() = 0;
    virtual double IwNormal() = 0;

    virtual double WElMyNormal() = 0;
    virtual double WElMzNormal() = 0;

    virtual double WPlMyNormal() = 0;
    virtual double WPlMzNormal() = 0;

    /** Nel caso di compressione, calcola area efficace e coordinate del
        baricentro della sezione efficace */
    virtual void EffNMinusNormal( double * AEff, double * yGEff, double * zGEff, double prec = 1.0e-3) = 0;
    virtual double WEffMyMaxNormal( double prec = 1.0e-3 ) = 0;
    virtual double WEffMyMinNormal( double prec = 1.0e-3 ) = 0;
    virtual double WEffMzMaxNormal( double prec = 1.0e-3 ) = 0;
    virtual double WEffMzMinNormal( double prec = 1.0e-3 ) = 0;

    virtual double AVyNormal() = 0;
    virtual double AVzNormal() = 0;

    virtual QString NClass() = 0;
    virtual QString MyMaxClass() = 0;
    virtual QString MyMinClass() = 0;
    virtual double MyClassLimitNormal();
    virtual QString MzMaxClass() = 0;
    virtual QString MzMinClass() = 0;
    virtual double MzClassLimitNormal();

    virtual QGraphicsItem * qGraphicsItem() = 0;
    virtual Point2DModel * sectionPoints();

    virtual void updateSectionSteelProfile() = 0;
    virtual void updateSectionPoints() = 0;

    void setSteel( Steel * st );
    Steel * steel();

    virtual void classDiagram(QList<QPolygonF *> *, double NULSMax, double MyULSMax, double MzULSMax ) = 0;

    virtual void NMULSElastPlastNormal( double * NRet, double * MyRet, double * MzRet,
                                        double l, double my, double mz,
                                        double yCen, double zCen );
    virtual void NMULSPlastNormal( double *NRet, double *MyRet, double * MzRet,
                                   double l, double my, double mz,
                                   double yCen, double zCen);

    /** Copia in y e z un punto della sezione */
    virtual void point( int, double * y, double * z ) = 0;
    /** Il numero di punti della sezione che point() mette a disposizione */
    virtual int nPoints() = 0;

signals:
    void sectionSteelProfileChanged();


protected:
    UnitMeasure * m_unitMeasure;
    Steel * m_steel;
    Point2DModel * m_sectionPoints;

    double fykNormal();
    double fydNormal( int i );

    double Theta( double N, double My, double Mz, double NULSMax, double MyULSMax, double MzULSMax);
    double xi( double My, double Mz, double MyULSMax, double MzULSMax);
};

#endif // SECTIONSTEELSHAPE_H
