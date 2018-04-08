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

#ifndef MATERIAL_H
#define MATERIAL_H

class Section;

class BoolPlus;
class QStringPlus;
class DoublePlus;

#include "material_export.h"
#include "materialmodel.h"

#include "varpluscontainerplus.h"

/**
* @class Material
*
* @brief Classe virtuale usata per i materiali
*
* Questa classe viene impiegata come base per definire i materiali impiegati nelle sezioni.
*
* @author Michele Mocciola
*
*/

class EXPORT_MATERIAL_LIB_OPT Material: public VarPlusContainerPlus {
    Q_OBJECT
protected:
    /** Contenitore delle Sezioni che utilizzano il materiale */
    QList<Section *> * m_sectionUsingContainer;

    /** Inizializza gli attributi pubblici (del tipo VarPlus) presenti nella classe */
    void initVar();

    void setG( bool emitAuto );

    // void updateReadOnlyVars( bool emitAuto );

public:
    /** Costruttore
    @param um unita' di misura
    @param parent genitore */
    Material( UnitMeasure *um, const QString &n, QObject * parent = 0 );

    /** Costruttore da parametri XML
    @param um unita' di misura
    @param attrs attributi XML
    @param parent genitore  */
    Material( UnitMeasure * ump, const QXmlStreamAttributes & attrs, QObject * parent = 0 );

    /** Costruttore
    @param val oggetto da copiare */
    Material ( Material & val );

    /** Costruttore
    @param val oggetto da copiare */
    Material ( Material * val );

    /** Distruttore */
    ~Material();

    /** Operatore assegnazione
    @param oggetto da assegnare */
    Material & operator=( Material & val);

    virtual MaterialModel::MaterialType materialType();

    /** La denominzione del materiale all'interno del programma (Concrete, SteelCncr, etc) */
    QStringPlus * typeName;

    /** Modulo di elasticità normale */
    DoublePlus * E;

    /** Modulo di elasticità tangenziale */
    DoublePlus * G;

    /** Modulo di Poisson */
    DoublePlus * nu;

    /** Coefficiente di dilatazione termica lineare */
    DoublePlus * alpha;

    /** Peso specifico */
    DoublePlus * gammaW;

    /** Ci dice se il materiale esistente */
    BoolPlus * isExisting;

    /** Fattore di Confidenza */
    DoublePlus * FC;

    /** Valore della tensione per un dato valore di deformaizone allo stato limire ultimo
        a livello di sezione
        @param e il valore della deformazione
        @param ret la variabile su cui viene scritto il valore della tensione */
    void fULS( DoublePlus * ret, DoublePlus * e );

    /** Valore della tensione per un dato valore di deformaizone allo stato limite di esercizio
        a livello di sezione
        @param e valore della deformazione
        @return il valore della tensione (udm normalizzate) */
    virtual double fSLSNormal( double e );

    /** Valore della tensione per un dato valore di deformaizone allo stato limite ultimo
        a livello di sezione
        @param e valore della deformazione
        @return il valore della tensione (udm normalizzate) */
    virtual double fULSNormal( double e );

signals:
    void materialChanged();
};

#endif // MATERIAL_H
