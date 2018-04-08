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

#include "glitem.h"

#include "glgroup.h"

#include <QOpenGLFunctions>

#include <QColor>
#include <QPointF>
#include <QVector4D>
#include <QMatrix4x4>

QOpenGLFunctions GLItem::m_functions;

GLItem::GLItem( GLGroup * parent ):
    m_group(0),
    m_color( new QColor(Qt::white) ),
    m_width( 3 ),
    m_printWidth(0.5),
    m_updatePending(false){
    setGroup(parent);
}

GLItem::~GLItem(){
    if( m_group ){
        m_group->removeGLItem( this );
    }
}

QColor GLItem::color(){
    return *m_color;
}

void GLItem::setColor ( const QColor & c ){
    if( c != !m_color ){
        *m_color = c;
        m_updatePending = true;
        if( m_group ){
            m_group->setGroupChanged();
        }
    }
}

void GLItem::setColor (  Qt::GlobalColor c ){
    if( c != !m_color ){
        *m_color = c;
        m_updatePending = true;        if( m_group ){
            m_group->setGroupChanged();
        }
    }
}

void GLItem::setGroup ( GLGroup * s ){
    if( m_group ){
        m_group->removeGLItem( this );
    }
    m_group = s;
    if( m_group ){
        m_group->addGLItem( this );
    }
}

GLGroup *GLItem::group(){
    return m_group;
}

int GLItem::width(){
    return m_width;
}

void GLItem::setWidth( int v ){
    if( v >= 0 && m_width != v ){
        m_width = v;
        m_updatePending = true;
        if( m_group ){
            m_group->setGroupChanged();
        }
    }
}

double GLItem::printWidth() {
    return m_printWidth;
}

void GLItem::setPrintWidth(double v) {
    if( v >= 0 && v != m_printWidth ){
        m_printWidth = v;
        if( m_group ){
            m_group->setGroupChanged();
        }
    }
}

void GLItem::paintGL(QOpenGLShaderProgram *program, QMatrix4x4 &matrixView, double zoom, bool hasVAOSupport ) {
    Q_UNUSED(program);
    Q_UNUSED(matrixView);
    Q_UNUSED(zoom);
    Q_UNUSED(hasVAOSupport);
}

QList<GLItem *> GLItem::pickedObjects(QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int w, int h, QRectF selectionRect, GLItem::PickingMode pm) {
    Q_UNUSED( matrixProj );
    Q_UNUSED( matrixView );
    Q_UNUSED( w );
    Q_UNUSED( h );
    Q_UNUSED( selectionRect );
    Q_UNUSED( pm );
    QList<GLItem *> ret;
    return ret;
}

void GLItem::initializeOpenGLFunctions() {
    m_functions.initializeOpenGLFunctions();
}

QPointF GLItem::project( double posX, double posY, double posZ, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height ){
    QVector4D PClip = matrixProj * matrixView * QVector4D(posX, posY, posZ,1.0);
    double winX = 0.0 + (width * (PClip.x() + 1.0) ) / 2.0;
    double winY = 0.0 + (height * (PClip.y() + 1.0) ) / 2.0;

    // riporta a coordinate Widget
    winY = (double)height - winY;

    return QPointF( winX, winY);
}
