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
#ifndef TABLEMODELPLUS_H
#define TABLEMODELPLUS_H

class VarPlus;
class UnitMeasure;
class QString;
class QXmlStreamWriter;
class QXmlStreamReader;

#include "varplus_export.h"

#include <QAbstractTableModel>

class TableModelPlusPrivate;

/**
* @class TableModelPlus
*
* @brief Classe virtuale per raccogliere variabili fisiche di tipo VarPlus
*
* Questa classe viene impiegata per raccogliere in forma tabellare variabili di tipo VarPlus
* Reimplementando il metodo insertRows ci si deve ricordare di aggiungere setVar()
*
* @author Michele Mocciola
*
*/

class EXPORT_VARPLUS_LIB_OPT TableModelPlus: public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DisplayedData{
        DisplayValue,
        DisplayValueRO,
        DisplayValueROInv,
        DisplayName,
        DisplayRichName,
        DisplayUnitMeasureString,
        DisplayUnitMeasureRichString,
        DisplayNameUnitMeasure,
        DisplayRichNameUnitMeasure,
        DisplayPointer
    };

    /**
     * @arg modName il nome del modello */
    explicit TableModelPlus(const QString & modName, UnitMeasure * ump, QObject *parent = 0);

    /** Nome del modello */
    QString modelName();
    virtual void writeXml(QXmlStreamWriter * writer) = 0;
    virtual void readXml(QXmlStreamReader * reader) = 0;

    int rowCount( const QModelIndex & = QModelIndex() ) const;
    int columnCount( const QModelIndex &  = QModelIndex() ) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /** Rimuove le righe nel modello */
    virtual void removeRows(int position, int count = 1) = 0;

    /** Pulisce il modello */
    void clear();

protected:
    /** Inserisce count righe vuote in position
        Metodo privato che deve essere chiamato nel metodo insertRows
        della classe derivata.
        Il metodo si preoccupa di di lanciare i metodi QAbstractTableModel::beginInsertRows()
        ed QAbstractTableModel::endInsertRows() */
    void insertRowsPrivate(int position, int count = 1);
    /** Rimuove count righe vuote in position
        Metodo privato che deve essere chiamato nel metodo removeRows
        della classe derivata.
        Il metodo si preoccupa di di lanciare i metodi QAbstractTableModel::beginRemoveRows()
        ed QAbstractTableModel::endRemoveRows() */
    void removeRowsPrivate(int position, int count = 1);

    /** Fa in modo che la cella (r,c) visulizzi la variabile var.
     *  Cosa viene visualizzato dipende dalla variabile dataType.  */
    bool setVar(DisplayedData dataType, int r, int c, VarPlus * var);
    /** Metodo di comodo equivalente a setVar(DisplayValue, ...)*/
    bool setVarValue(int r, int c, VarPlus *var );
    /** Metodo di comodo. Imposta le variabili var della riga r, secondo l'ordine di inserimento. */
    void setVarValueRow(int row, VarPlus * var, ... );
    /** Metodo di comodo equivalente a setVar(DisplayNameUnitMeasure, ...)*/
    bool setVarNameUnitMeasure(int r, int c, VarPlus *var );
    /** Imposta la variabile come readOnly. L'impostazione ha senso solo nel caso il tipo visualizzato
     *  sia DisplayValue, e annulla la possibilità di modificare il contenuto della variabile. */
    void setVarReadOnly(int r, int c, bool ro );
    /** Imposta gli header delle colonne */
    void setHeaders( const QList<QString> & hC );

protected slots:
    void updateData(const QString & rowColString );

signals:
    void modelChanged();

protected:
    TableModelPlusPrivate * m_d;
};

#endif // TABLEMODELPLUS_H
