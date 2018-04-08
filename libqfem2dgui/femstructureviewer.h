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
#ifndef FEMSTRUCTUREVIEWER_H
#define FEMSTRUCTUREVIEWER_H

#include <QGraphicsView>

class FEMStructure;

class FEMStructureViewer : public QGraphicsView
{
    Q_OBJECT
public:
    explicit FEMStructureViewer(FEMStructure * str, QWidget * parent = 0);
    ~FEMStructureViewer();

signals:

public slots:
    void viewAll();
    void viewCenter();

private:
    FEMStructure * m_structure;
    QPoint * m_basePanning;
    QGraphicsItemGroup * m_root;

    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent ( QMouseEvent * event );
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void resizeEvent(QResizeEvent* event);

    void zoom( double );
    double vertexRadius( int = 3 );
    QPointF mapToRoot( const QPoint & );
    QRectF visibleRect();
};

#endif // FEMSTRUCTUREVIEWER_H
