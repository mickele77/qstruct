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
#ifndef SECTIONTHINCONTSTEELGUI_H
#define SECTIONTHINCONTSTEELGUI_H

#include "sectiongui.h"

class MaterialModel;
class SectionThinContSteelGUIPrivate;

class SectionThinContSteelGUI : public SectionGUI
{
    Q_OBJECT

public:
    explicit SectionThinContSteelGUI(UnitMeasure * ump, MaterialModel * matModel, QWidget *parent = 0);
    ~SectionThinContSteelGUI();

public slots:
    void setSection( Section * sect );

private:
    SectionThinContSteelGUIPrivate * m_d;

    Material *material();

private slots:
    void addPoints();
    void removePoints();
    void setMaterialFromComboBox();
};

#endif // SECTIONTHINCONTSTEELGUI_H
