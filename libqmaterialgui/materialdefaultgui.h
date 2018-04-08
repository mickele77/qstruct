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
#ifndef MATERIALDEFAULTGUI_H
#define MATERIALDEFAULTGUI_H

#include "materialgui.h"

namespace Ui {
    class MaterialDefaultGUI;
}

class MaterialDefaultGUI : public MaterialGUI {
    Q_OBJECT
public:
    MaterialDefaultGUI(UnitMeasure *, QWidget *parent = 0);
    ~MaterialDefaultGUI();

public slots:
    /** Imposta il materiale corrente nella GUI */
    void setMaterial( Material * );

private:
    Material * m_material;
    Ui::MaterialDefaultGUI *m_ui;
};

#endif // MATERIALDEFAULTGUI_H
