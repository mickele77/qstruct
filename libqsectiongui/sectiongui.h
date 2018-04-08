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
#ifndef SECTIONGUI_H
#define SECTIONGUI_H

#include <QWidget>
#include "varpluscontainer.h"

class Section;
class Material;
class MaterialModel;
class QGraphicsItemGroup;
class QGraphicsItem;
class QComboBox;

/**
* @class SectionGUI
*
* @brief Classe virtuale usata per creare GUI di visualizzazione delle informazioni sulle sezioni
*
* Questa classe virtuale viene impiegata per creare oggetti QWidget usati per visualizzare e modificare
* le informazioni associate ad una sezione
*
* @author Michele Mocciola
*
*/

class SectionGUI : public QWidget, public VarPlusContainer
{
    Q_OBJECT
public:
    /** Costruttore
    @param ump puntatore alla classe contenente le informazioni sulle unità di misura correnti
    @param parent l'oggetto QWidget genitore */
    explicit SectionGUI(UnitMeasure * ump, MaterialModel * matModel, QWidget *parent = 0);

    QGraphicsItemGroup *extraGraphicsItem();

public slots:
    /** Imposta la sezione corrente nella GUI */
    virtual void setSection( Section * ) = 0;
    /** Imposta il materiale della sezione corrente quando la combobox
      * dei materiali cambia l'indice corrente. */
    virtual void setMaterialFromComboBox() = 0;

protected:
    /** Modello dei materiali */
    MaterialModel * m_materialModel;
    /** Contenitore degli oggetti grafici */
    QGraphicsItemGroup * m_graphicsItem;
    /** ComboBox dei materiali della sezione */
    QComboBox * m_materialComboBox;

    /** Aggiorna la combobox dei materiali */
    void refreshMaterialComboBox();
    /** Lita dei materiali impiegati nella GUI. */
    virtual QList<Material *> materialsList();
    /** Il materiale della sezione corrente. */
    virtual Material * material() = 0;

    /* Il metodo viene reimplementato per aggiornare la ComboBox dei materiali */
    void showEvent ( QShowEvent * event);
};

#endif // SECTIONGUI_H
