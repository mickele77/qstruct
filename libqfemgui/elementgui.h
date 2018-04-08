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
#ifndef ELEMENTGUI_H
#define ELEMENTGUI_H

#include <QWidget>

class Element;

class ElementGUI : public QWidget
{
    Q_OBJECT
public:
    explicit ElementGUI(QWidget *parent = 0);

    virtual QString typeElement() = 0;
    virtual void setElement( Element * ) = 0;

signals:

public slots:
    virtual void updateData() = 0;
    virtual void updateResults() = 0;
};

#endif // ELEMENTGUI_H
