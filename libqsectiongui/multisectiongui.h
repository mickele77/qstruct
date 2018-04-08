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
#ifndef MULTISECTIONGUI_H
#define MULTISECTIONGUI_H

#include "sectiongui.h"

class MaterialModel;
class MultiSectionGUIPrivate;

class MultiSectionGUI : public SectionGUI
{
    Q_OBJECT

public:
    explicit MultiSectionGUI(UnitMeasure * ump, MaterialModel * matModel, QWidget *parent = 0);
    ~MultiSectionGUI();

public slots:
    void setSection( Section * sect );

private:
    MultiSectionGUIPrivate * m_d;

    Material *material();

private slots:
    void addSectionLine();
    void removeSectionLine();
    void addSectionArc();
    void removeSectionArc();

    void setMaterialFromComboBox();
};

#endif // MULTISECTIONGUI_H
