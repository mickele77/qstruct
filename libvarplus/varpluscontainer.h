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
#ifndef VARPLUSCONTAINER_H
#define VARPLUSCONTAINER_H

class UIntPlus;
class QStringPlus;
class VarPlus;
class UnitMeasure;
class IntPlus;
class QXmlAttributes;
class QXmlStreamWriter;
class QXmlStreamAttributes;
class QString;
template <class Key, class T> class QMap;
template <class T> class QList;

#include "varplus_export.h"

/**
* @class VarPlusContainer
*
* @brief Classe da usarsi come base per classi che contengono oggetti di tipo VarPlus
*
* Questa classe viene impiegata come base per definire classe che contengono oggetti di tipo
* VarPlus
*
* @author Michele Mocciola
*
*/

class EXPORT_VARPLUS_LIB_OPT VarPlusContainer {
public:
    /** Costruttore */
    VarPlusContainer( UnitMeasure * ump, const QString & t, const QString & n );

    /** Distruttore */
    virtual ~VarPlusContainer();

    /** Overloading dell'oepratore di assegnamento */
    VarPlusContainer & operator=( VarPlusContainer & val);

    /** Il puntatore alla classe UnitMeasure di riferimento
    @return il puntatore */
    UnitMeasure * unitMeasure();

    /** Reimposta la classe UnitMeasure del materiale
    @param um la nuova classe UnitMeasure */
    void setUnitMeasure( UnitMeasure * um );

    /** id del contenitore
    * identifica univocamente il contenitore
    *  E' readOnly. */
    UIntPlus * id;
    /** Denominazione del contenitore per l'utente. */
    QStringPlus * name;

    /** Ricalcola l'id, aumentando di 1 il precedente valore. */
    virtual void nextId();

    /** Scrive le variabili contenute nel contenitore sul flusso XML. */
    virtual void writeXml( QXmlStreamWriter * writer);

    /** Carica i valori degli attributi pubblici da un oggetto di tipo QXmlStreamAttributes
    @param attrs gli attributi da analizzare */
    virtual void loadFromXML( const QXmlStreamAttributes &attrs );

    /** Se una classe contenitore otherContainer vuole che, il presente contenitore non venga
     * distrutto, si deve usare il presente metodo. */
    void setIsUsedBy( VarPlusContainer * otherContainer );

    /** Se la classe contenitore otherContainer non necessita più che il presente contenitore
     * non venga distrutto, si usa il presente metodo. */
    void removeIsUsedBy( VarPlusContainer * otherContainer );

    /** Ci dice se la presente classe è usata o meno (se è usata non deve essere distrutta). */
    bool isUsed();

    /** Imposta i valori delle variabili readOnly.
     * Le grandezze readOnly sono quelle il cui valore viene calcolato a partire
     * dalle rimanenti mediante metodi creati ad hoc.
     * Di default il metodo ha una implementazione non molto performante.
     * Può essere eimplementato con l'elenco di tutti i metodi che permettono di
     * calcolare le variabili readonly. */
    virtual void updateReadOnlyVars();

    /** Sospende l'emissione dei segnali delle variabili contenute */
    virtual void suspendSignals( bool susp = true );

protected:
    /** Nome dell contenitore. Sarà il nome che comparirà, ad esempio, nel file XML. */
    QString * m_typeNameInternal;

    /** Puntatore alla classe UnitMeasure contenente le informazioni sulle unita' di misura della classe */
    UnitMeasure * m_unitMeasure;

    /** Contenitore degli attributi pubblici (del tipo VarPlus) presenti nella classe */
    QMap<QString, VarPlus *> * m_varContainer;

    /** List dei contenitori che utilizzano il contenitore corrente  */
    QList<VarPlusContainer *> * m_usingContainer;

    /** Aggiunge la variabile al contenitore delle variabili della classe
    @param var la variabile che viene aggiunta */
    virtual void addVarToContainer( VarPlus * var );

};

#endif // VARPLUSCONTAINER_H
