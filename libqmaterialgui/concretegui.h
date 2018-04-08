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
#ifndef CONCRETEGUI_H
#define CONCRETEGUI_H

#include "materialgui.h"
#define CONCRETE_PLOT_ACCURACY 50

namespace Ui {
class ConcreteGUI;
}
class ConcreteGUIPrivate;

class UnitMeasure;
class DoublePlus;
class Concrete;
class QString;

class ConcreteGUI : public MaterialGUI {
    Q_OBJECT

public:
    ConcreteGUI(UnitMeasure *, const QString & settingsFile, QWidget *parent = 0);
    ~ConcreteGUI();

public slots:
    void setMaterial( Material * );

private slots:
    void setFb( );
    void plot( void );
    void replot( void );
    void updatePlotData( void );
    void loadStandardData( void );


private:
    ConcreteGUIPrivate * m_d;

    void loadStandardComboBox();
};

#endif // CONCRETEGUI_H
