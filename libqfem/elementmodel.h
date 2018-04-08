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
#ifndef ELEMENTMODEL_H
#define ELEMENTMODEL_H

#include <QAbstractTableModel>

class ElementModelPrivate;
class VertexModel;
class SectionModel;
class Section;
class Hypothesis;
class QGraphicsItem;
class GLGroup;

#include "element.h"
#include <QList>

/**
* @class ElementModel
*
* @brief Classe usata per gestire gli elementi della struttura
*
* Questa classe viene impiegata per gestire gli elementi definiti all'interno della nostra struttura
*
* @author Michele Mocciola
*
*/

class ElementModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    // aggiungere qui i nuovi elementi
    enum ElementType{
        elementBeam
    };

    typedef QList<Element *>::iterator ElementIterator;

    /** Costruttore della classe ElementModel */
    ElementModel( VertexModel * vm, SectionModel * sm, Hypothesis * hyp, UnitMeasure * ump, QObject *parent = 0 );
    /** Costruttore della classe ElementModel */
    ElementModel( VertexModel * vm, Section * s, Hypothesis * hyp, UnitMeasure * ump, QObject *parent = 0 );

    int rowCount( const QModelIndex & = QModelIndex() ) const;
    int columnCount( const QModelIndex &  = QModelIndex() ) const;
    QVariant data( const QModelIndex &, int ) const;
    QVariant headerData(int, Qt::Orientation,
                        int = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &) const;

    Element * element(int);
    Element * elementId( unsigned int );
    Element * element(const QModelIndex & );
    Element * lastElement();
    QModelIndex elementIndex( int );
    QModelIndex elementIndexId( unsigned int );
    ElementIterator begin();
    ElementIterator end();

    int nElements();
    bool insert( ElementType, int, int = 1 );
    bool append( ElementType );
    bool remove(int, int = 1 );

    bool clear();

    Hypothesis * hypothesis();

    QGraphicsItem * graphicsItem();
    GLGroup * glItem();

    SectionModel * sectionModel();

private:
    ElementModelPrivate * m_d;

    int mapId( unsigned int );
    Element * createElement( ElementType );
};

#endif // ELEMENTMODEL_H
