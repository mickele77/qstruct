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
#ifndef ELEMENTMODELGUI_H
#define ELEMENTMODELGUI_H

#include <QWidget>

namespace Ui {
class ElementModelGUI;
}

#include "elementmodel.h"
class ElementGUI;

class ElementModelGUI : public QWidget
{
    Q_OBJECT
public:
    explicit ElementModelGUI( ElementModel *,
                              VertexModel *,
                              QWidget *parent = 0);
    ~ElementModelGUI();

public slots:
    void updateData();
    void updateResults();

signals:

private:
    Ui::ElementModelGUI * m_ui;
    ElementModel * m_model;
    VertexModel * m_vertexModel;
    ElementGUI * m_currentElementGUI;

private slots:
    void insertElement( ElementModel::ElementType type );
    void insertBeam();

    void remove();

    void setCurrent();
};

#endif // ELEMENTMODELGUI_H
