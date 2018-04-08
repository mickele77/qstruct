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
#ifndef SOILSHEETGUI_H
#define SOILSHEETGUI_H

class SoilSheet;
class Soil;
class SoilModel;
class UnitMeasure;
class VarPlus;

#include <QWidget>

class SoilSheetGUIPrivate;

class SoilSheetGUI : public QWidget
{
    Q_OBJECT

public:
    explicit SoilSheetGUI( UnitMeasure * ump,
                               SoilModel * soilModel,
                               QWidget *parent = 0);
    ~SoilSheetGUI();

    void setSoilSheet(SoilSheet *sheet);

    void showEvent(QShowEvent *);

private:
    SoilSheetGUIPrivate * m_d;

private slots:
    void populateSoilComboBox();
    void setSoilDown(int);
    void setSoilUp(int);
};

#endif // SOILSHEETGUI_H
