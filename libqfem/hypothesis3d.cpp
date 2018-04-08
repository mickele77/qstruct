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
#include "hypothesis3d.h"

Hypothesis3D::Hypothesis3D():
    Hypothesis(3,6) {
    setUnitMeasureEta( 0, UnitMeasure::sectL );
    setUnitMeasureEta( 1, UnitMeasure::sectL );
    setUnitMeasureEta( 2, UnitMeasure::sectL );
    setUnitMeasureEta( 3, UnitMeasure::angle );
    setUnitMeasureEta( 4, UnitMeasure::angle );
    setUnitMeasureEta( 5, UnitMeasure::angle );
    setUnitMeasureF( 0, UnitMeasure::loadF );
    setUnitMeasureF( 1, UnitMeasure::loadF );
    setUnitMeasureF( 2, UnitMeasure::loadF );
    setUnitMeasureF( 3, UnitMeasure::loadM );
    setUnitMeasureF( 4, UnitMeasure::loadM );
    setUnitMeasureF( 5, UnitMeasure::loadM );
}

Eigen::MatrixXd Hypothesis3D::N( Eigen::MatrixXd & LCS ){
    Eigen::MatrixXd ret = Eigen::MatrixXd::Identity(6,6);
    ret.block(0,0,3,3) = LCS;
    ret.block(3,3,3,3) = LCS;

    return ret;
}

Eigen::MatrixXd Hypothesis3D::N( Eigen::Map<Eigen::MatrixXd> & LCS){
    Eigen::MatrixXd ret = Eigen::MatrixXd::Identity(6,6);
    ret.block(0,0,3,3) = LCS;
    ret.block(3,3,3,3) = LCS;

    return ret;
}
