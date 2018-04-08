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

#include "qgraphicsitemviewer.h"

#include <QGraphicsScene>
#include <QList>
#include <QGraphicsItem>

class QGraphicsItemViewerPrivate {
public:
    QGraphicsItemViewerPrivate(){
        ;
    }

    QList<QGraphicsItem *> itemsList;
};

QGraphicsItemViewer::QGraphicsItemViewer( QWidget * parent):
    QGraphicsView( parent ),
    m_d( new QGraphicsItemViewerPrivate() ){
    setScene( new QGraphicsScene( this ) );

    connect( scene(), SIGNAL(changed(QList<QRectF>)), this, SLOT(viewAll()));

    QTransform trans;
    trans.scale( 1.0, -1.0 );
    trans.rotate( 180.0 );
    setTransform( trans );
}

QGraphicsItemViewer::~QGraphicsItemViewer(){
    removeAllQGraphicsItems();
}

void QGraphicsItemViewer::viewAll(){
    updateSceneRect();
    fitInView( scene()->sceneRect(), Qt::KeepAspectRatio );
}

void QGraphicsItemViewer::resizeEvent( QResizeEvent * event ) {
    QGraphicsView::resizeEvent( event);
    viewAll();
}

void QGraphicsItemViewer::updateSceneRect(){
    scene()->setSceneRect( scene()->itemsBoundingRect() );
}

void QGraphicsItemViewer::addQGraphicsItem( QGraphicsItem * item ){
    if( item ){
        scene()->addItem( item );
        m_d->itemsList.append( item );
    }
}

void QGraphicsItemViewer::removeQGraphicsItem( QGraphicsItem * item ){
    scene()->removeItem( item );
    m_d->itemsList.removeAll( item );
}

void QGraphicsItemViewer::removeAllQGraphicsItems(){
    QList<QGraphicsItem *> list = m_d->itemsList;
    QList<QGraphicsItem *>::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
        if ( *it ){
            scene()->removeItem( *it );
            m_d->itemsList.removeAll(*it );
        }
    }
}
