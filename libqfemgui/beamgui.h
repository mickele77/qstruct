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
#ifndef BEAMGUI_H
#define BEAMGUI_H

#include "elementgui.h"

class Element;
class Beam;
class SectionModel;
class VertexModel;
class BeamGUIPrivate;

/**
* @class BeamGUI
*
* @brief Classe usata per visualizzare  parametri di un elemento Beam
*
* Questa classe viene impiegata per visualizzare e modificare i parametri associati
* ad un elemento beam
*
* @author Michele Mocciola
*
*/

class BeamGUI : public ElementGUI
{
    Q_OBJECT
public:
    explicit BeamGUI(Beam * b, VertexModel *, SectionModel *, QWidget *parent = 0);

    QString typeElement();
    void setElement( Element * );

    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

signals:

private:
    BeamGUIPrivate * m_d;
    void updateSectionCB();

public slots:
    void updateData();
    void updateResults();

private slots:
    void setSection( int );
    void addFLoad();
    void removeFLoad();
    void addQLoad();
    void removeQLoad();
};

#endif // BEAMGUI_H
