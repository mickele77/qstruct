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
#ifndef QGRAPHICSITEMVIEWER_H
#define QGRAPHICSITEMVIEWER_H

#include "qstructutility_export.h"

#include <QGraphicsView>

class QGraphicsItemViewerPrivate;

class EXPORT_QSTRUCTUTILITY_LIB_OPT QGraphicsItemViewer : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsItemViewer( QWidget * = 0 );
    ~QGraphicsItemViewer();

    void addQGraphicsItem( QGraphicsItem * );
    void removeQGraphicsItem( QGraphicsItem * );
    void removeAllQGraphicsItems();

public slots:
    void updateSceneRect();
    void viewAll();

private:
    QGraphicsItemViewerPrivate * m_d;

    void resizeEvent ( QResizeEvent * );
};

#endif // QGRAPHICSITEMVIEWER_H
