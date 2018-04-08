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
#include "hypothesis2d.h"

Hypothesis2D::Hypothesis2D():
    Hypothesis( 2, 3 ) {
    setUnitMeasureEta( 0, UnitMeasure::sectL );
    setUnitMeasureEta( 1, UnitMeasure::sectL );
    setUnitMeasureEta( 2, UnitMeasure::angle );
    setUnitMeasureF( 0, UnitMeasure::loadF );
    setUnitMeasureF( 1, UnitMeasure::loadF );
    setUnitMeasureF( 2, UnitMeasure::loadM );
}

Eigen::MatrixXd Hypothesis2D::N( Eigen::MatrixXd & LCS ){
    Eigen::MatrixXd ret =  Eigen::MatrixXd::Identity(3,3);
    ret.block(0,0,2,2) = LCS;

    return ret;
}

Eigen::MatrixXd Hypothesis2D::N( Eigen::Map<Eigen::MatrixXd> & LCS){
    Eigen::MatrixXd ret =  Eigen::MatrixXd::Identity(3,3);
    ret.block(0,0,2,2) = LCS;

    return ret;
}
