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

#ifndef RETAININGWALLQSCOMBINATIONGUI_H
#define RETAININGWALLQSCOMBINATIONGUI_H

#include <QWidget>

namespace Ui {
class RetainingWallQSCombinationGUI;
}

class RetainingWallQSCombinationGUIPrivate;

class RetainingWall;
class RetainingWallGUI;
class UnitMeasure;
class DoublePlus;
class VarPlus;
class QString;

class RetainingWallQSCombinationGUI : public QWidget
{
    Q_OBJECT
    
public:
    explicit RetainingWallQSCombinationGUI( UnitMeasure * ump,
                                          const QString & cn,
                                          DoublePlus * Gk2, DoublePlus * Qk,
                                          DoublePlus * gammaTanPhiPrime, DoublePlus * gammaCPrime,
                                          DoublePlus *gammaG1Min, DoublePlus *gammaG1Max,
                                          DoublePlus *gammaG2Min, DoublePlus *gammaG2Max,
                                          DoublePlus *gammaQMin, DoublePlus *gammaQMax,
                                          DoublePlus *gammaSliding, DoublePlus *gammaQLim,
                                          RetainingWallGUI *parent = 0);
    ~RetainingWallQSCombinationGUI();

    void setRetainingWall( RetainingWall * wall );

public slots:
    virtual void calculateQSCombination();

protected:
    Ui::RetainingWallQSCombinationGUI *m_ui;
    RetainingWallQSCombinationGUIPrivate * m_d;

    void setVar();

    QString printData( QList<VarPlus *> & );

private slots:
    virtual void copyClip();

};

#endif // RETAININGWALLQSCOMBINATIONGUI_H
