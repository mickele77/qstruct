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
#ifndef SOILMODELGUI_H
#define SOILMODELGUI_H


namespace Ui {
class SoilModelGUI;
}

#include "soilgui_export.h"
#include <QWidget>
#include "soilmodel.h"

class EXPORT_SOILGUI_LIB_OPT SoilModelGUI : public QWidget
{
    Q_OBJECT
public:
    explicit SoilModelGUI( SoilModel *,
                               QWidget *parent = 0);
    ~SoilModelGUI();

signals:

private:
    Ui::SoilModelGUI * m_ui;
    SoilModel * m_model;

private slots:
    void insertSoil( SoilModel::SoilType );
    void insertClay();
    void insertSand();

    void remove();
    void setDefault();
    void setCurrent();
};

#endif // SOILMODELGUI_H
