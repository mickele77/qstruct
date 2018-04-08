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
#ifndef CALCRESULTGUI_H
#define CALCRESULTGUI_H

#include <QWidget>

class CalcResultsGUIPrivate;
class FEMStructure;
class GLItem;

class CalcResultsGUI : public QWidget
{
    Q_OBJECT
    
public:
    explicit CalcResultsGUI( FEMStructure * str, QWidget *parent = 0);
    ~CalcResultsGUI();
    
private slots:
    void calculate();
    void addGLItemDeformed();
    void clearGLItem();
    void setFPLocalScale(int i);
    void addGLItemBeamFPLocal();

private:
    CalcResultsGUIPrivate * m_d;

signals:
    void resultsChanged();
    void addGLItem( GLItem * );
};

#endif // CALCRESULTGUI_H
