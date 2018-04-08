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
#ifndef RETAININGWALLMODELGUI_H
#define RETAININGWALLMODELGUI_H

#include <QWidget>

namespace Ui {
class RetainingWallModelGUI;
}

class RetainingWallModel;

class RetainingWallModelGUI : public QWidget
{
    Q_OBJECT
public:
    explicit RetainingWallModelGUI( RetainingWallModel *,
                                    QWidget *parent = 0);
    ~RetainingWallModelGUI();

signals:

private:
    Ui::RetainingWallModelGUI * m_ui;
    RetainingWallModel * m_model;

private slots:
    void insert();
    void remove();
    void setDefault();
    void setCurrent();
};

#endif // RETAININGWALLMODELGUI_H
