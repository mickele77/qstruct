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
#include "glpoint.h"

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

class GLPointPrivate{
public:
    GLPointPrivate( double xTmp, double yTmp, double zTmp ):
        wasInitialized(false){
        x[0] = xTmp;
        x[1] = yTmp;
        x[2] = zTmp;
    }
    ~GLPointPrivate(){
    }

    double x[3];
    QOpenGLVertexArrayObject vertexArrayObject;
    QOpenGLBuffer vertexPositionBuffer;
    QOpenGLBuffer vertexColorBuffer;
    bool wasInitialized;
};

GLPoint::GLPoint( double xTmp, double yTmp, double zTmp, GLGroup * s) :
    GLItem(s),
    m_d( new GLPointPrivate(xTmp, yTmp, zTmp) ){
}

void GLPoint::paintGL( QOpenGLShaderProgram * program, bool hasVAOSupport ){
    if( !(m_d->wasInitialized) ){
        initializeGLData( program, hasVAOSupport );
    }
    if( m_updatePending ){
        updateGLData();
    }

    m_functions.glLineWidth( m_width );
    if( hasVAOSupport ){
        m_d->vertexArrayObject.bind();
        m_functions.glDrawArrays(GL_POINTS, 0, 1);
        m_d->vertexArrayObject.release();
    } else {
        if( m_updatePending ){
            updateGLData();
            m_updatePending = false;
        }
        m_d->vertexColorBuffer.bind();
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);
        m_d->vertexColorBuffer.release();

        m_d->vertexPositionBuffer.bind();
        program->enableAttributeArray("vertexPosition");
        program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);
        m_d->vertexPositionBuffer.release();

        m_functions.glDrawArrays(GL_POINTS, 0, 1);
    }
}

void GLPoint::paintGL(QOpenGLShaderProgram *program, QMatrix4x4 &modelView) {
/*    QVector4D p( m_d->x[0], m_d->x[1], m_d->x[2], 1.0);
    QVector4D pV = modelView * p;*/
}

void GLPoint::setP(double x, double y, double z){
    m_d->x[0] = x;
    m_d->x[1] = y;
    m_d->x[2] = z;
    m_updatePending = true;
    if( m_group ){
        m_group->setGroupChanged();
    }
}

QList<GLItem *> GLPoint::pickedObjects(QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int w, int h, QRectF selectionRect, GLItem::PickingMode pm) {
    Q_UNUSED( pm );
    QPointF p = project( m_d->x[0], m_d->x[1], m_d->x[2], matrixProj, matrixView, w, h );
    QList<GLItem *> ret;
    if( selectionRect.contains(p) ){
        ret.append( this );
    }
    return ret;
}

void GLPoint::boundingBoxGL(QVector4D bounds[2], QMatrix4x4 & mView  ) {
    QVector4D p(m_d->x[0], m_d->x[1], m_d->x[2], 1.0);
    QVector4D pm = mView * p;
    bounds[0] = pm;
    bounds[1] = pm;
}

void GLPoint::paint(QPainter * painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height){
    painter->save();

    QPen pen;
    pen.setWidthF( m_printWidth );
    pen.setColor( *m_color );
    painter->setPen( pen );
    painter->drawPoint( project(m_d->x[0], m_d->x[1], m_d->x[2], matrixProj, matrixView, width, height ) );

    painter->restore();
}

void GLPoint::initializeGLData(QOpenGLShaderProgram * program, bool hasVAOSupport ) {
    // Array contenente il colore
    GLfloat colorData[] = {
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0
    };
    // Array contenente i dati sul punto
    GLdouble vertexData[] = { GLdouble(m_d->x[0]), GLdouble(m_d->x[1]), GLdouble(m_d->x[2]) };

    // crea i VertexBuffer
    m_d->vertexPositionBuffer.create();
    m_d->vertexPositionBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_d->vertexColorBuffer.create();
    m_d->vertexColorBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    if( hasVAOSupport ){
        m_d->vertexArrayObject.create();
        m_d->vertexArrayObject.bind();

        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.allocate(vertexData, 1 * 3 * sizeof(GLdouble));
        program->enableAttributeArray("vertexPosition");
        program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);
        m_d->vertexPositionBuffer.release();

        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.allocate(colorData, 1 * 4 * sizeof(GLfloat));
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);
        m_d->vertexColorBuffer.release();

        m_d->vertexArrayObject.release();
    } else {
        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.allocate(vertexData, 1 * 3 * sizeof(GLdouble));
        m_d->vertexPositionBuffer.release();

        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.allocate(colorData, 1 * 4 * sizeof(GLfloat));
        m_d->vertexColorBuffer.release();
    }

    m_d->wasInitialized = true;
    m_updatePending = false;
}

void GLPoint::updateGLData() {
    // coordinate punto
    if( m_d->vertexPositionBuffer.isCreated() ){
        m_d->vertexPositionBuffer.bind();
        GLdouble vertexData[] = { GLdouble(m_d->x[0]), GLdouble(m_d->x[1]), GLdouble(m_d->x[2]) };
        m_d->vertexPositionBuffer.write(0, vertexData, 1 * 3 * sizeof(GLdouble));
        m_d->vertexPositionBuffer.release();
    }

    // Colore
    GLfloat colorData[] = {
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f
    };
    if( m_d->vertexColorBuffer.isCreated() ){
        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.write(0, colorData, 1 * 4 * sizeof(GLfloat));
        m_d->vertexColorBuffer.release();
    }
    m_updatePending = false;
}
