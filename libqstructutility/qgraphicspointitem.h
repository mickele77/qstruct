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
#ifndef QGRAPHICSPOINTITEM_H
#define QGRAPHICSPOINTITEM_H

#include "qstructutility_export.h"

#include <QGraphicsEllipseItem>

class QGraphicsPointItemPrivate;

class EXPORT_QSTRUCTUTILITY_LIB_OPT QGraphicsPointItem : public QGraphicsEllipseItem
{
public:
    explicit QGraphicsPointItem(const QPointF & p, double r, QGraphicsItem  *parent  = 0);

    QRectF boundingRect() const;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *,
               QWidget *);

    void setRadius( double );
    void setPoint( double, double );
    void setPoint( const QPointF & );

private:
    QGraphicsPointItemPrivate * m_d;
};

#endif // QGRAPHICSPOINTITEM_H
