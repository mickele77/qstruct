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
#ifndef STEEL_H
#define STEEL_H

#include "material.h"

class DoublePlus;

/**
* @class Steel
*
* @brief Classe usata per modellizzare il materiale acciaio da carpenteria
*
* Questa classe viene impiegata per modellizzare l'acciaio da carpenteria, inteso come materiale di sezioni
*
* @author Michele Mocciola
*
*/

class  EXPORT_MATERIAL_LIB_OPT Steel: public Material {
    Q_OBJECT
public:
    Steel( UnitMeasure * um, const QString &np = QString(), QObject * parent = 0 );
    Steel( UnitMeasure *, const QXmlStreamAttributes &,  QObject * parent = 0  );

    MaterialModel::MaterialType materialType();

    double fULSNormal( double e );

    DoublePlus * fyk;
    DoublePlus * fyk40;
    DoublePlus * ftk;
    DoublePlus * ftk40;

    DoublePlus * gammaS( int );
    DoublePlus * fyd( int );
    DoublePlus * fyd40( int );

    /** Rapporto tra tensione snervamento acciaio S235 e tensione effettiva acciaio */
    double epsilon( double t );

private:
    DoublePlus * m_gammaS[3];
    DoublePlus * m_fyd[3];
    DoublePlus * m_fyd40[3];

    void initVar();

    void setFyd( int i, bool emitAuto );
    void setFyd40( int i, bool emitAuto );
    void setFyd_0( bool emitAuto );
    void setFyd40_0( bool emitAuto );
    void setFyd_1( bool emitAuto );
    void setFyd40_1( bool emitAuto );
    void setFyd_2( bool emitAuto );
    void setFyd40_2( bool emitAuto );
};

#endif // STEEL_H
