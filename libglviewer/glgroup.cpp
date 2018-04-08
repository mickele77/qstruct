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
#include "glgroup.h"

#include <QList>
#include <QVector3D>
#include <QOpenGLShaderProgram>

#include <QVector4D>

class GLGroupPrivate{
public:
    GLGroupPrivate(){
    }

    QList<GLItem *> items;
};

GLGroup::GLGroup(GLGroup * s):
    QObject( 0 ),
    GLItem(),
    m_d( new GLGroupPrivate()){
    setGroup(s);
}

void GLGroup::paintGL(QOpenGLShaderProgram * program, bool hasVAOSupport){
    QList<GLItem *>::iterator iter = m_d->items.begin();
    while( iter != m_d->items.end() ){
        (*iter)->paintGL( program, hasVAOSupport );
        iter++;
    }
}

void GLGroup::paintGL(QOpenGLShaderProgram *program, QMatrix4x4 &matrixView, double zoom, bool hasVAOSupport) {
    QList<GLItem *>::iterator iter = m_d->items.begin();
    while( iter != m_d->items.end() ){
        (*iter)->paintGL( program, matrixView, zoom, hasVAOSupport );
        iter++;
    }
}

void GLGroup::paint( QPainter * painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height ){
    QList<GLItem *>::iterator iter = m_d->items.begin();
    while( iter != m_d->items.end() ){
        (*iter)->paint( painter, matrixProj, matrixView, width, height );
        iter++;
    }
}

void GLGroup::addGLItem( GLItem *s){
    if( !(m_d->items.contains(s))){
        m_d->items.append(s);
        setGroupChanged();
    }
}

void GLGroup::removeGLItem( GLItem *s){
    int n = m_d->items.removeAll(s);
    if ( n > 0 ){
        setGroupChanged();
    }
}

QList<GLItem *> GLGroup::pickedObjects(QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int w, int h, QRectF selectionRect, GLItem::PickingMode pm) {
    QList<GLItem *> ret;
    for( QList<GLItem *>::iterator iter = m_d->items.begin(); iter != m_d->items.end(); ++iter ){
        ret.append( (*iter)->pickedObjects(matrixProj, matrixView, w, h, selectionRect, pm ) );
    }
    return ret;
}

void GLGroup::boundingBoxGL(QVector4D bounds[], QMatrix4x4 &mView ) {
    if( m_d->items.isEmpty() ){
        bounds[0] = QVector4D(0.0,0.0,0.0,1.0);
        bounds[1] = QVector4D(0.0,0.0,0.0,1.0);
    } else {
        QList<GLItem *>::iterator iter = m_d->items.begin();
        (*iter)->boundingBoxGL(bounds, mView );
        QVector4D tmpBounds[2];
        ++iter;
        while( iter != m_d->items.end() ){
            (*iter)->boundingBoxGL( tmpBounds, mView );
            if( tmpBounds[0].x() < bounds[0].x() ){
                bounds[0].setX( tmpBounds[0].x() );
            }
            if( tmpBounds[0].y() < bounds[0].y() ){
                bounds[0].setY( tmpBounds[0].y() );
            }
            if( tmpBounds[0].z() < bounds[0].z() ){
                bounds[0].setZ( tmpBounds[0].z() );
            }
            if( tmpBounds[1].x() > bounds[1].x() ){
                bounds[1].setX( tmpBounds[1].x() );
            }
            if( tmpBounds[1].y() > bounds[1].y() ){
                bounds[1].setY( tmpBounds[1].y() );
            }
            if( tmpBounds[1].z() > bounds[1].z() ){
                bounds[1].setZ( tmpBounds[1].z() );
            }
            ++iter;
        }
    }
}

void GLGroup::setGroupChanged() {
    if( m_group ){
        m_group->setGroupChanged();
    } else {
        emit groupChanged();
    }
}
