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
#ifndef VERTEXMODEL_H
#define VERTEXMODEL_H


class VertexModelPrivate;

class Hypothesis;
class QGraphicsItem;
class GLGroup;
class UnitMeasure;

#include <QList>
#include "tablemodelplus.h"
#include "vertex.h"

/**
* @class VertexModel
*
* @brief Classe usata per gestire i vertici della struttura
*
* Questa classe viene impiegata per gestire i vertici definiti all'interno della nostra struttura
*
* @author Michele Mocciola
*
*/

class VertexModel : public TableModelPlus {
    Q_OBJECT
public:
    typedef QList<Vertex *>::iterator VertexIterator;

    /** Costruttore della classe VertexModel
    @param dimension dimensione dello spazio in cui è definita la struttura
    @param ndof gradi di libertà del vertice */
    VertexModel( Hypothesis * hyp, UnitMeasure * ump, QObject *parent = 0 );

    Vertex * vertex(int);
    Vertex * vertexId( unsigned int );
    int vertexCount();
    Vertex * vertex(const QModelIndex & );
    Vertex * lastVertex();
    VertexIterator begin();
    VertexIterator end();

    void insertRows( int position, int count= 1 );
    void appendRow();
    void removeRows(int position, int count= 1 );

    Hypothesis * hypothesis();

    QGraphicsItem * graphicsItem();
    QRectF graphicsItemBoudingRect();
    GLGroup * glItem();

    void writeXml(QXmlStreamWriter *writer);
    void readXml( QXmlStreamReader * reader );

private:
    VertexModelPrivate * m_dd;

    QList<QString> headersList();
    void insertVertex(Vertex *addedVert, int position);
    QPointF vertexToQPointF( Vertex * );
    static QRectF qRectFromVertexes( Vertex * p1, Vertex * p2);

public slots:
    void setGraphicsRadius( double );

signals:
    void vertexChanged( Vertex * );
};

#endif // VERTEXMODEL_H
