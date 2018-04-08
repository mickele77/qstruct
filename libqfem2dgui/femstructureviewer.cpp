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
#include "femstructureviewer.h"

#include "femstructure.h"

#include <QMouseEvent>
#include <QPointF>
#include <QPoint>
#include <QRectF>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>

FEMStructureViewer::FEMStructureViewer( FEMStructure * str , QWidget * parent ):
    QGraphicsView( parent),
    m_structure( str ),
    m_basePanning(NULL){

    setScene( new QGraphicsScene(this));

    scale( 1.0,-1.0 );
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    m_root = new QGraphicsItemGroup();
    scene()->addItem( m_root );
    m_structure->setVertexRadius( vertexRadius() );
    m_structure->graphicsItem()->setParentItem( m_root );

    setMouseTracking( true );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
}

FEMStructureViewer::~FEMStructureViewer(){
}

void FEMStructureViewer::viewAll(){
    QPointF PStart = mapToRoot( QPoint(width() / 2, height() / 2 ) );
    QPointF PEnd = m_structure->graphicsItemBoudingRect().center();
    QPointF DP = PEnd - PStart;
    m_root->setTransform(QTransform::fromTranslate(- DP.x(), - DP.y()), true);

    double v = 0.0;
    QRectF sceneR = visibleRect();

    // m_structure->setNullPointRadius();
    // QRectF itemsRect =  m_root->childrenBoundingRect();
    QRectF itemsRect =  m_structure->graphicsItemBoudingRect();

    if( itemsRect.width() != 0.0 ){
        v = 0.85 * sceneR.width() / itemsRect.width();
    }
    if( itemsRect.height() != 0.0 ){
        double tmp = 0.85 * sceneR.height() / itemsRect.height();
        if( tmp < v ){
            v = tmp;
        }
    }
    if( v == 0.0 ){
        v = 1.0;
    }

    m_root->setTransform(QTransform::fromTranslate(PEnd.x(), PEnd.y()), true);
    zoom(v);
    m_root->setTransform(QTransform::fromTranslate(-PEnd.x(), -PEnd.y()), true);
}

void FEMStructureViewer::viewCenter(){
    QPointF PStart = mapToRoot( QPoint(width() / 2, height() / 2 ) );
    QPointF PEnd = m_structure->graphicsItemBoudingRect().center();
    QPointF DP = PEnd - PStart;
    m_root->setTransform(QTransform::fromTranslate(- DP.x(), - DP.y()), true);
}


void FEMStructureViewer::mousePressEvent(QMouseEvent* event) {
    if( event->button() == Qt::MiddleButton ){
        m_basePanning = new QPoint( event->pos() );
    }

    QGraphicsView::mousePressEvent( event );
}

void FEMStructureViewer::mouseReleaseEvent(QMouseEvent* event) {
    if( m_basePanning != NULL ){
        QPointF DP = mapToRoot( event->pos() ) - mapToRoot( *m_basePanning );
        delete m_basePanning;
        m_basePanning = NULL;
        m_root->setTransform(QTransform::fromTranslate(DP.x(), DP.y()), true);
    }
    QGraphicsView::mouseReleaseEvent( event );
}

void FEMStructureViewer::mouseMoveEvent(QMouseEvent* event) {
    if( m_basePanning != NULL ){
        QPointF DP = mapToRoot( event->pos() ) - mapToRoot( *m_basePanning );
        (*m_basePanning) = event->pos();
        m_root->setTransform(QTransform::fromTranslate(DP.x(), DP.y()), true);
    }

    QGraphicsView::mouseMoveEvent( event );
}

void FEMStructureViewer::mouseDoubleClickEvent ( QMouseEvent * event ){
    if( event->button() == Qt::MiddleButton ){
        viewAll();
    }

    QGraphicsView::mouseDoubleClickEvent( event );
}

void FEMStructureViewer::wheelEvent(QWheelEvent* event) {
    QPointF P = mapToRoot( event->pos() );

    int sign = event->delta() / 120;
    double v = 1.0 + sign * 0.20 ;

    m_root->setTransform(QTransform::fromTranslate(P.x(), P.y()), true);
    zoom( v );
    m_root->setTransform(QTransform::fromTranslate(-P.x(), -P.y()), true);
}

void FEMStructureViewer::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent( event );
}

QPointF FEMStructureViewer::mapToRoot( const QPoint & p ){
    return m_root->mapFromScene( mapToScene( p ) );
}

QRectF FEMStructureViewer::visibleRect() {
    QPointF topLeft = mapToRoot ( QPoint(0, this->height()) );
    QPointF bottomRight = mapToRoot ( QPoint(this->width(), 0) );

    return QRectF (topLeft, bottomRight);
}

void FEMStructureViewer::zoom( double v ){
    m_structure->setVertexRadius( vertexRadius() / v );
    m_root->setTransform(QTransform::fromScale(v, v), true);
}

double FEMStructureViewer::vertexRadius( int p ){
    QLineF l( mapToRoot( QPoint(0,0) ), mapToRoot( QPoint(0,p) ) );
    return l.length();
}
