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
#ifndef BEAMSTRESSGUI_H
#define BEAMSTRESSGUI_H

class Beam;

#include <QWidget>

class BeamStressGUIPrivate;

/**
* @class BeamStressesGUI
*
* @brief Classe usata per visualizzare le sollecitazioni interne dell'elemento Beam
*
* Questa classe viene impiegata per visualizzare le caratteristiche di sollecitazione
* di un elemento beam a seguito dell'analisi FEM eseguita sullo stesso
*
* @author Michele Mocciola
*
*/

class BeamStressGUI : public QWidget
{
    Q_OBJECT
    
public:
    explicit BeamStressGUI( QWidget *parent = 0);
    ~BeamStressGUI();
    
    void setBeam(Beam * b);

public slots:
    void updateData();

private:
    BeamStressGUIPrivate * m_d;
};

#endif // BEAMSTRESSESGUI_H
