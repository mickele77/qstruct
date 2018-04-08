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
#ifndef CONCRETE_H
#define CONCRETE_H

#include "material.h"

class DoublePlus;
class ConcreteModelfEpsSection;

/**
* @class Concrete
*
* @brief Classe usata per modellizzare il materiale calcestruzzo
*
* Questa classe viene impiegata per modellizzare il calcestruzzo, inteso come materiale di sezioni
*
* @author Michele Mocciola
*
*/

class EXPORT_MATERIAL_LIB_OPT Concrete: public Material {
    Q_OBJECT
public:
    enum ModelfEps {
        ParabolaRectangle,         /**< diagramma composto da tratto parabolico e tratto costante (parabola-rettangolo) */
        EC2                        /**< diagramma di cui al par. 3.1.5 dell'EC2 */
    };
    enum ModelfEpsSection {
        SectionParabolaRectangle,  /**< diagramma composto da tratto parabolico e tratto costante (parabola-rettangolo) */
        SectionBiLinear,           /**< diagramma composto da tratto lineare e tratto costante (triangolo rettangolo) */
        SectionStressBlock         /**< diagramma composto da tratto nullo e tratto costante (stress block) */
    };

    Concrete( UnitMeasure *, const QString &n = QString(), QObject * parent = 0 );
    Concrete( UnitMeasure *, double rck, const QString &n, QObject * parent = 0 );
    Concrete( Concrete & );
    Concrete( Concrete * );
    Concrete( UnitMeasure * ump, const QXmlStreamAttributes & attrs, QObject * parent = 0 );

    MaterialModel::MaterialType materialType();

    double fSLSNormal(double e);
    double fULSNormal( double );

    void fbk( DoublePlus * ret, DoublePlus * e );
    double fbkNormal( double );
    void fbd( DoublePlus * ret, DoublePlus * e );
    double fbdNormal( double = 32.0 );

    DoublePlus * Rck;
    DoublePlus * fck;
    DoublePlus * fcm;
    DoublePlus * fcd;
    DoublePlus * fcdRed;
    DoublePlus * fctk;
    DoublePlus * fctm;
    DoublePlus * fctd;
    DoublePlus * fctdRed;
    DoublePlus * fctk95;
    DoublePlus * fcfm;
    DoublePlus * fcRare;
    DoublePlus * fcRareRed;
    DoublePlus * fcPermanent;
    DoublePlus * fcPermanentRed;

    DoublePlus * Ecm;
    DoublePlus * ratioEEcm;

    DoublePlus * epsC1;
    DoublePlus * epsC2;
    DoublePlus * epsC3;
    DoublePlus * epsC4;
    DoublePlus * epsCU1;
    DoublePlus * epsCU2;
    DoublePlus * epsCU3;
    DoublePlus * epsCU4;

    DoublePlus * alphaCC;
    DoublePlus * gammaC;

    ConcreteModelfEpsSection * modelfEpsSection;

    double epsCUNormal();
    double epsCNormal();

private slots:
    void setExisting(bool exist);

private:
    void initVar();

    void setFckFromRck( bool emitAuto );
    void setFckFromFcm( bool emitAuto );
    void setFcmFromFck( bool emitAuto );
    void setfctm( bool emitAuto );
    void setfctk( bool emitAuto );
    void setfctk95( bool emitAuto );
    void setfcfm( bool emitAuto );
    void setfcd( bool emitAuto );
    void setfcdExisting( bool emitAuto );
    void setfcdRed( bool emitAuto );
    void setfctd( bool emitAuto );
    void setfctdRed( bool emitAuto );
    void setfcRare( bool emitAuto );
    void setfcRareRed( bool emitAuto );
    void setfcPermanent( bool emitAuto );
    void setfcPermanentRed( bool emitAuto );
    void setEcm( bool emitAuto );
    void setE( bool emitAuto );
    void setEpsC1( bool emitAuto );
    void setEpsC2( bool emitAuto );
    void setEpsC3( bool emitAuto );
    void setEpsC4( bool emitAuto );
    void setEpsCU1( bool emitAuto );
    void setEpsCU2( bool emitAuto );
    void setEpsCU3( bool emitAuto );
    void setEpsCU4( bool emitAuto );

    // void updateReadOnlyVars(bool emitAuto = true);
};

#include "enumplus.h"

class ConcreteModelfEpsSectionPrivate;

/**
* @class ConcreteModelfEpsSectionPlus
*
* @brief Classe usata per indicare la legge costitutiva dell'acciaio per le verifiche
* locali della sezione a SLU
*
* Questa classe viene impiegata per indicare la legge costitutiva dell'acciaio per le verifiche
* locali della sezione a SLU
*
* @author Michele Mocciola
*
*/

class EXPORT_MATERIAL_LIB_OPT ConcreteModelfEpsSection : public EnumPlus {
    Q_OBJECT
public:
    ConcreteModelfEpsSection( Concrete::ModelfEpsSection vv,
                              const QString & nn,
                              bool ro = false );

    ConcreteModelfEpsSection & operator=( ConcreteModelfEpsSection & );
    VarPlus & operator=( VarPlus &);

    QString typeVar();

    Concrete::ModelfEpsSection value();
    Concrete::ModelfEpsSection valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( Concrete::ModelfEpsSection, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( Concrete::ModelfEpsSection, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    ConcreteModelfEpsSectionPrivate * m_d;

    ~ConcreteModelfEpsSection();
};

#endif
