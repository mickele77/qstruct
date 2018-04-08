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
#ifndef STEELCNCR_H
#define STEELCNCR_H

#include "material.h"

class Section;
class DoublePlus;
class SteelCncrModelfEpsPlus;
class VarPlus;

/**
* @class SteelCncr
*
* @brief Classe usato per modellizzare il materiale acciaio per c.a.
*
* Questa classe viene impiegata per modellizzare l'acciaio per c.a., inteso come materiale di sezioni
*
* @author Michele Mocciola
*
*/

class EXPORT_MATERIAL_LIB_OPT SteelCncr: public Material {
public:
    enum ModelFEps {
        horizontalTopBranch,
        inclinedTopBranch
    };

    SteelCncr(UnitMeasure *, const QString & n = QString(), QObject * parent = 0 );
    SteelCncr( SteelCncr & );
    SteelCncr( SteelCncr * );
    SteelCncr( UnitMeasure *, const QXmlStreamAttributes & attrs, QObject * parent = 0  );

    ~SteelCncr();

    MaterialModel::MaterialType materialType();

    bool cracked( const double & );
    double fULSNormal( double );

    DoublePlus * fyk;
    DoublePlus * fyd;
    DoublePlus * ftk;
    DoublePlus * k;
    DoublePlus * fud;
    DoublePlus * fRare;
    DoublePlus * gammaS;
    DoublePlus * epsYd;
    DoublePlus * epsUk;
    DoublePlus * epsUd;
    DoublePlus * ratioEpsUdEpsUk;
    SteelCncrModelfEpsPlus * modelfEps;

private:
    void initVar();

    void setFyd( bool emitAuto );
    void setFud( bool emitAuto );
    void setFRare( bool emitAuto );
    void setEpsYd( bool emitAuto );
    void setEpsUd( bool emitAuto );

};

#include "enumplus.h"

class SteelCncrModelfEpsPlusPrivate;

class EXPORT_MATERIAL_LIB_OPT SteelCncrModelfEpsPlus : public EnumPlus
{
    Q_OBJECT
public:
    SteelCncrModelfEpsPlus( SteelCncr::ModelFEps vv,
                           const QString & nn,
                           bool ro = false );
    SteelCncrModelfEpsPlus( SteelCncrModelfEpsPlus & val );


    SteelCncrModelfEpsPlus & operator=( SteelCncrModelfEpsPlus &);
    VarPlus & operator=( VarPlus &);

    QString typeVar();

    SteelCncr::ModelFEps value();
    SteelCncr::ModelFEps valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( SteelCncr::ModelFEps, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( SteelCncr::ModelFEps, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    SteelCncrModelfEpsPlusPrivate * m_d;

    ~SteelCncrModelfEpsPlus();
};

#endif // STEELCNCR_H
