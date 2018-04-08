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
#include "glline.h"

#include "glgroup.h"

#include <QPen>
#include <QColor>
#include <QVector4D>
#include <QMatrix4x4>

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>

#include <QPainter>

class GLLinePrivate{
public:
    GLLinePrivate( double x0Tmp, double y0Tmp, double z0Tmp, double x1Tmp, double y1Tmp, double z1Tmp ):
        wasInitialized(false){
        x[0] = x0Tmp;
        x[1] = y0Tmp;
        x[2] = z0Tmp;
        x[3] = x1Tmp;
        x[4] = y1Tmp;
        x[5] = z1Tmp;
    }
    ~GLLinePrivate(){
    }

    double x[6];

    QOpenGLVertexArrayObject vertexArrayObject;
    QOpenGLBuffer vertexPositionBuffer;
    QOpenGLBuffer vertexColorBuffer;
    bool wasInitialized;
};

GLLine::GLLine( double x0Tmp, double y0Tmp, double z0Tmp, double x1Tmp, double y1Tmp, double z1Tmp, GLGroup * s) :
    GLItem(s),
    m_d( new GLLinePrivate( x0Tmp, y0Tmp, z0Tmp, x1Tmp, y1Tmp, z1Tmp )){
}

void GLLine::paintGL( QOpenGLShaderProgram * program, bool hasVAOSupport ){
    if( !(m_d->wasInitialized) ){
        initializeGLData( program, hasVAOSupport );
    }

    if( m_updatePending ){
        updateGLData();
    }

    m_functions.glLineWidth( m_width );

    if( hasVAOSupport  ){
        m_d->vertexArrayObject.bind();
        m_functions.glDrawArrays(GL_LINES, 0, 2);
        m_d->vertexArrayObject.release();
    } else {
        m_d->vertexPositionBuffer.bind();
        program->enableAttributeArray("vertexPosition");
        program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);
        m_d->vertexPositionBuffer.release();

        m_d->vertexColorBuffer.bind();
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);
        m_d->vertexColorBuffer.release();

        m_functions.glDrawArrays(GL_LINES, 0, 2);
    }
}

QList<GLItem *> GLLine::pickedObjects(QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int w, int h, QRectF selectionRect, GLItem::PickingMode pm) {
    QPointF p1 = project( m_d->x[0], m_d->x[1], m_d->x[2], matrixProj, matrixView, w, h );
    QPointF p2 = project( m_d->x[0], m_d->x[1], m_d->x[2], matrixProj, matrixView, w, h );
    QList<GLItem *> ret;
    if( pm == window ){
        if( selectionRect.contains(p1) && selectionRect.contains(p2) ){
            ret.append( this );
        }
    } else {
        if( selectionRect.contains(p1) || selectionRect.contains(p2) ){
            ret.append( this );
        }
    }
    return ret;
}

void GLLine::paintGL(QOpenGLShaderProgram *, QMatrix4x4 &) {

}

void GLLine::setP(int i, double x, double y, double z){
    if( i < 2 && i >= 0 ){
        m_d->x[3*i] = x;
        m_d->x[3*i+1] = y;
        m_d->x[3*i+2] = z;
        m_updatePending = true;
        if( m_group ){
            m_group->setGroupChanged();
        }
    }
}

void GLLine::setP0(double x, double y, double z){
    setP( 0, x, y, z);
}

void GLLine::setP1(double x, double y, double z){
    setP( 1, x, y, z);
}

void GLLine::boundingBoxGL(QVector4D bounds[2], QMatrix4x4 & mView) {
    QVector4D p0(m_d->x[0], m_d->x[1], m_d->x[2], 1.0);
    QVector4D p1(m_d->x[3], m_d->x[4], m_d->x[5], 1.0);
    QVector4D p0m = mView * p0;
    QVector4D p1m = mView * p1;
    bounds[0].setX( qMin( p0m.x(), p1m.x() ) );
    bounds[0].setY( qMin( p0m.y(), p1m.y() ) );
    bounds[0].setZ( qMin( p0m.z(), p1m.z() ) );
    bounds[0].setW( 1.0 );
    bounds[1].setX( qMax( p0m.x(), p1m.x() ) );
    bounds[1].setY( qMax( p0m.y(), p1m.y() ) );
    bounds[1].setZ( qMax( p0m.z(), p1m.z() ) );
    bounds[1].setW( 1.0 );
}

void GLLine::paint( QPainter * painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height){
    painter->save();

    QPen pen;
    pen.setWidthF( m_printWidth );
    pen.setColor( *m_color );
    painter->setPen( pen );
    painter->drawLine( project(m_d->x[0], m_d->x[1], m_d->x[2], matrixProj, matrixView, width, height ), project(m_d->x[3], m_d->x[4], m_d->x[5], matrixProj, matrixView, width, height ) );

    painter->restore();
}

void GLLine::initializeGLData(QOpenGLShaderProgram * program, bool hasVAOSupport ) {
    // crea i VertexBuffer
    m_d->vertexPositionBuffer.create();
    m_d->vertexPositionBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_d->vertexColorBuffer.create();
    m_d->vertexColorBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    // Array contenente i colori
    GLfloat colorData[] = {
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f
    };

    GLdouble vertexData[] = {
        GLdouble(m_d->x[0]), GLdouble(m_d->x[1]), GLdouble(m_d->x[2]),
        GLdouble(m_d->x[3]), GLdouble(m_d->x[4]), GLdouble(m_d->x[5])
    };

    if(hasVAOSupport){
        m_d->vertexArrayObject.create();
        m_d->vertexArrayObject.bind();

        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.allocate(vertexData, 2 * 3 * sizeof(GLdouble));
        program->enableAttributeArray("vertexPosition");
        program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);
        m_d->vertexPositionBuffer.release();

        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.allocate(colorData, 2 * 4 * sizeof(GLfloat));
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);
        m_d->vertexColorBuffer.release();

        m_d->vertexArrayObject.release();
    } else {
        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.allocate(vertexData, 2 * 3 * sizeof(GLdouble));
        m_d->vertexPositionBuffer.release();

        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.allocate(colorData, 2 * 4 * sizeof(GLfloat));
        m_d->vertexColorBuffer.release();
    }

    m_d->wasInitialized = true;
    m_updatePending = false;
}

void GLLine::updateGLData() {
    // coordinate punti
    if( m_d->vertexPositionBuffer.isCreated() ){
        GLdouble vertexData[] = {
            GLdouble(m_d->x[0]), GLdouble(m_d->x[1]), GLdouble(m_d->x[2]),
            GLdouble(m_d->x[3]), GLdouble(m_d->x[4]), GLdouble(m_d->x[5])
        };
        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.write(0, vertexData, 2 * 3 * sizeof(double));
        m_d->vertexPositionBuffer.release();
    }

    // Colore
    if( m_d->vertexColorBuffer.isCreated() ){
        GLfloat colorData[] = {
            GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
            GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f
        };
        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.write(0, colorData, 2 * 4 * sizeof(GLfloat));
        m_d->vertexColorBuffer.release();
    }
    m_updatePending = false;
}
