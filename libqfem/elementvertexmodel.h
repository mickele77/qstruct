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
#ifndef ELEMENTVERTEXMODEL_H
#define ELEMENTVERTEXMODEL_H

#include <QAbstractTableModel>

class Element;
class VertexModel;

/**
* @class ElementVertexModelVertexModel
*
* @brief Classe usata per gestire i vertici di un singolo elemento
*
* Questa classe viene impiegata per gestire i vertici definiti all'interno di un elemento
*
* @author Michele Mocciola
*
*/

class ElementVertexModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /** Costruttore della classe VertexModel */
    ElementVertexModel( VertexModel * gvm, Element *);

    int rowCount( const QModelIndex & = QModelIndex() ) const;
    int columnCount( const QModelIndex &  = QModelIndex() ) const;
    QVariant data( const QModelIndex &, int ) const;
    QVariant headerData(int, Qt::Orientation,
                        int = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role );


private:
    Element * m_element;
    VertexModel * m_vertexModel;
};

#endif // ELEMENTVERTEXMODEL_H
