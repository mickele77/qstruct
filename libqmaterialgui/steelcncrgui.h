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
#ifndef STEELCNCRGUI_H
#define STEELCNCRGUI_H

#include "materialgui.h"

namespace Ui {
class SteelCncrGUI;
}
class SteelCncrGUIPrivate;

class UnitMeasure;
class SteelCncr;

class SteelCncrGUI : public MaterialGUI {
    Q_OBJECT

public:
    SteelCncrGUI(UnitMeasure *, const QString & settingsFile, QWidget *parent = 0);
    ~SteelCncrGUI();

public slots:
    void setMaterial( Material * steelCncr );

private slots:
    void updateStandardNameCBox( );
    void loadStandardData();

    void plot( void );
    void replot( void );
    void updatePlotData( void );

    void changeFykLabel();
    void changeFtkLabel();

private:
    SteelCncrGUIPrivate * m_d;

    void loadStandardComboBox();
};

#endif // STEELCNCRGUI_H
