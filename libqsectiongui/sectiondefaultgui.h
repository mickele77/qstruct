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
#ifndef SECTIONDEFAULTGUI_H
#define SECTIONDEFAULTGUI_H

class UnitMeasure;
class MaterialModel;
class Section;

#include "sectiongui.h"

class SectionDefaultGUIPrivate;

class SectionDefaultGUI : public SectionGUI
{
    Q_OBJECT

public:
    explicit SectionDefaultGUI(UnitMeasure * ump,
                               MaterialModel * matModel,
                               QWidget *parent = 0);
    ~SectionDefaultGUI();

public slots:
    void setSection( Section * );

private slots:
    void setMaterialFromComboBox();

private:
    SectionDefaultGUIPrivate * m_d;

    Material *material();
};

#endif // SECTIONDEFAULTGUI_H
