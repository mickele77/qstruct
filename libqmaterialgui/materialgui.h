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
#ifndef MATERIALGUI_H
#define MATERIALGUI_H

#include <QWidget>
#include "varpluscontainer.h"

class UnitMeasure;
class Material;
class VarPlus;
template <class Key, class T> class QMap;

/**
* @class MaterialGUI
*
* @brief Classe virtuale usata per creare GUI di visualizzazione delle informazioni sui materiali
*
* Questa classe virtuale viene impiegata per creare oggetti QWidget usati per visualizzare e modificare
* le informazioni associate ad un materiale
*
* @author Michele Mocciola
*
*/

class MaterialGUI : public QWidget, public VarPlusContainer
{
    Q_OBJECT
public:
    /** Costruttore
    @param um puntatore alla classe contenente le informazioni sulle unità di misura correnti
    @param parent l'oggetto QWidget genitore */
    explicit MaterialGUI(UnitMeasure * um, QWidget *parent = 0);

public slots:
    /** Imposta il materiale corrente nella GUI */
    virtual void setMaterial( Material * ) = 0;

    /** Annulla il materiale corrente */
    void setMaterialNULL();
};

#endif // MATERIALGUI_H
