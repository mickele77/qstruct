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
#ifndef HYPOTHESIS3D_H
#define HYPOTHESIS3D_H

#include "hypothesis.h"

class Hypothesis3D : public Hypothesis
{
public:
    Hypothesis3D();

    Eigen::MatrixXd N( Eigen::MatrixXd & LCS);
    Eigen::MatrixXd N( Eigen::Map<Eigen::MatrixXd> & LCS);
};

#endif // HYPOTHESIS3D_H
