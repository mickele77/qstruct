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
#ifndef SSBEAM_H
#define SSBEAM_H

#include <QObject>

class LoadComplTableModel;
class Section;
class DoublePlus;
class UnitMeasure;

class BeamS{
public:
    BeamS( UnitMeasure *, double );
    DoublePlus * s;
    DoublePlus * N;
    DoublePlus * Vy;
    DoublePlus * Vz;
    DoublePlus * Mx;
    DoublePlus * My;
    DoublePlus * Mz;
    ~BeamS();
};

class SSBeam : public QObject {
    Q_OBJECT
public:
    SSBeam(UnitMeasure *, Section * ss );

    /** Inclinazione porzione trave tra appoggi */
    DoublePlus * alpha;
    /** Inclinazione sbalzo sinistro trave */
    DoublePlus * alpha1;
    /** Inclinazione sbalzo destro trave */
    DoublePlus * alpha2;
    /** Lunghezza porzione trave tra appoggi */
    DoublePlus * L;
    /** Lunghezza sbalzo sinistro trave */
    DoublePlus * L1;
    /** Lunghezza sbalzo destro trave */
    DoublePlus * L2;
    /** Lunghezza proiezone su orizzontale porzione trave tra appoggi */
    DoublePlus * LHor;
    /** Lunghezza proiezone su orizzontale sbalzo sinistro trave*/
    DoublePlus * LHor1;
    /** Lunghezza proiezone su orizzontale sbalzo destro trave*/
    DoublePlus * LHor2;
    /** Lunghezza proiezone su verticale porzione trave tra appoggi */
    DoublePlus * LVer;
    /** Lunghezza proiezone su verticale sbalzo sinistro trave*/
    DoublePlus * LVer1;
    /** Lunghezza proiezone su verticale sbalzo destro trave */
    DoublePlus * LVer2;
    /** Distanza tra ritegni torsionali */
    DoublePlus * L0LT;

    void setSection( Section * );
    Section * section();

    virtual bool verify( QString * result, QString * messages, LoadComplTableModel * lModel );

protected:
    UnitMeasure * m_unitMeasure;
    Section * m_section;

    void initVar();
    void calcS( LoadComplTableModel * lModel );

private slots:
    void setL();
    void setL1();
    void setL2();
    void setL0LT();

signals:
    void sectionChanged();

private:
    void setL( DoublePlus * l, DoublePlus * lHor, DoublePlus * lVer, DoublePlus * a);
};

#endif // SSBEAM_H
