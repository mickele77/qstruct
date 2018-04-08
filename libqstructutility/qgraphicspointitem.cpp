#include "qgraphicspointitem.h"

#include <QPainter>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QRectF>

class QGraphicsPointItemPrivate {
public:
    QPointF center;
    double radius;
    QPen pen;
    QBrush brush;
};

QGraphicsPointItem::QGraphicsPointItem(const QPointF & p, double r, QGraphicsItem  *parent ) :
    QGraphicsEllipseItem( parent),
    m_d( new QGraphicsPointItemPrivate ){
    m_d->center = p;
    m_d->radius = r;
    m_d->pen.setColor( Qt::blue );
    m_d->brush.setStyle( Qt::NoBrush );
    m_d->brush.setColor( Qt::blue );
}

QRectF QGraphicsPointItem::boundingRect() const {
    double s = m_d->pen.widthF() / 2.0;
    double w = m_d->radius + s;
    return QRectF( m_d->center.x() - w, m_d->center.y() - w, 2.0 * w, 2.0 * w);
}

void QGraphicsPointItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *,
                               QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen( m_d->pen );
    painter->setBrush( m_d->brush );

    // painter->drawArc( boundingRect(), 0, 16 * 360);
    painter->drawRect( boundingRect() );
}

void QGraphicsPointItem::setRadius( double r){
    prepareGeometryChange();
    m_d->radius = r;
}

void QGraphicsPointItem::setPoint( double x, double y){
    prepareGeometryChange();
    m_d->center.setX( x );
    m_d->center.setY( y );
}

void QGraphicsPointItem::setPoint( const QPointF & p ){
    prepareGeometryChange();
    m_d->center = p;
}
