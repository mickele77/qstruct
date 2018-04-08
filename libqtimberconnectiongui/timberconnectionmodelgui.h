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
#ifndef TIMBERCONNECTIONMODELGUI_H
#define TIMBERCONNECTIONMODELGUI_H

#include <QWidget>

namespace Ui {
class TimberConnectionModelGUI;
}

#include "timberconnectionmodel.h"

class TimberConnectionModelGUI : public QWidget
{
    Q_OBJECT
public:
    explicit TimberConnectionModelGUI( TimberConnectionModel *,
                               QWidget *parent = 0);
    ~TimberConnectionModelGUI();

signals:

private:
    Ui::TimberConnectionModelGUI * m_ui;
    TimberConnectionModel * m_model;

private slots:
    void insertTimberConnection();

    void remove();
    void setCurrent();
};

#endif // TIMBERCONNECTIONMODELGUI_H
