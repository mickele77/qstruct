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
#ifndef RETAININGWALLEQQSCOMBINATIONGUI_H
#define RETAININGWALLEQQSCOMBINATIONGUI_H

#include "retainingwallqscombinationgui.h"

class RetainingWallEQQSCombinationGUIPrivate;
class RWEarthQuakeDirection;

class RetainingWallEQQSCombinationGUI : public RetainingWallQSCombinationGUI
{
    Q_OBJECT
public:
    explicit RetainingWallEQQSCombinationGUI( UnitMeasure * ump,
                                              const QString & cn,
                                              RWEarthQuakeDirection * EQDirection,
                                              DoublePlus * kv,
                                              DoublePlus * kh,
                                              DoublePlus * theta,
                                              DoublePlus * Gk2, DoublePlus * Qk, DoublePlus * psi2Q,
                                              DoublePlus * gammaTanPhiPrime, DoublePlus * gammaCPrime,
                                              DoublePlus *gammaSliding, DoublePlus *gammaQLim,
                                              RetainingWallGUI *parent = 0);
public slots:
    void calculateQSCombination();

private:
    RetainingWallEQQSCombinationGUIPrivate *m_dd;

private slots:
    void copyClip();
};

#endif // RETAININGWALLEQQSCOMBINATIONGUI_H
