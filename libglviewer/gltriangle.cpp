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
#include "gltriangle.h"

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

class GLTrianglePrivate{
public:
    GLTrianglePrivate( double x0Tmp, double y0Tmp, double z0Tmp, double x1Tmp, double y1Tmp, double z1Tmp, double x2Tmp, double y2Tmp, double z2Tmp ):
        wasInitialized(false) {
        x[0] = x0Tmp;
        x[1] = y0Tmp;
        x[2] = z0Tmp;
        x[3] = x1Tmp;
        x[4] = y1Tmp;
        x[5] = z1Tmp;
        x[6] = x2Tmp;
        x[7] = y2Tmp;
        x[8] = z2Tmp;
    }
    ~GLTrianglePrivate(){
    }

    double x[9];

    QOpenGLVertexArrayObject vertexArrayObject;
    QOpenGLVertexArrayObject vertexHatchArrayObject;

    QOpenGLBuffer vertexPositionBuffer;
    QOpenGLBuffer vertexColorBuffer;
    QOpenGLBuffer vertexHatchColorBuffer;

    bool wasInitialized;
};

GLTriangle::GLTriangle( double x0Tmp, double y0Tmp, double z0Tmp,
                        double x1Tmp, double y1Tmp, double z1Tmp,
                        double x2Tmp, double y2Tmp, double z2Tmp,
                        GLGroup * s) :
    GLItem(s),
    m_d( new GLTrianglePrivate( x0Tmp, y0Tmp, z0Tmp, x1Tmp, y1Tmp, z1Tmp, x2Tmp, y2Tmp, z2Tmp )){
}

void GLTriangle::paintGL( QOpenGLShaderProgram * program, bool hasVAOSupport ){
    if( !(m_d->wasInitialized) ){
        initializeGLData( program, hasVAOSupport );
    }
    if( m_updatePending ){
        updateGLData();
    }

    m_functions.glLineWidth( m_width );

    if( hasVAOSupport ){
        m_d->vertexArrayObject.bind();
        m_functions.glDrawArrays(GL_LINE_LOOP, 0, 3);
        m_d->vertexArrayObject.release();

        m_d->vertexHatchArrayObject.bind();
        m_functions.glDrawArrays(GL_TRIANGLES, 0, 3);
        m_d->vertexHatchArrayObject.release();

    } else {
        m_d->vertexColorBuffer.bind();
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);
        m_d->vertexColorBuffer.release();

        m_d->vertexPositionBuffer.bind();
        program->enableAttributeArray("vertexPosition");
        program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);
        m_d->vertexPositionBuffer.release();

        m_functions.glDrawArrays(GL_LINE_LOOP, 0, 3);

        m_d->vertexHatchColorBuffer.bind();
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);
        m_d->vertexHatchColorBuffer.release();

        m_functions.glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

void GLTriangle::setP(int i, double x, double y, double z){
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

void GLTriangle::setP0(double x, double y, double z){
    setP( 0, x, y, z);
}

void GLTriangle::setP1(double x, double y, double z){
    setP( 1, x, y, z);
}

void GLTriangle::setP2(double x, double y, double z){
    setP( 2, x, y, z);
}

void GLTriangle::boundingBoxGL(QVector4D bounds[2], QMatrix4x4 & mView ) {
    QVector4D p0(m_d->x[0], m_d->x[1], m_d->x[2], 1.0);
    QVector4D p1(m_d->x[3], m_d->x[4], m_d->x[5], 1.0);
    QVector4D p2(m_d->x[6], m_d->x[7], m_d->x[8], 1.0);
    QVector4D p0m = mView * p0;
    QVector4D p1m = mView * p1;
    QVector4D p2m = mView * p2;
    bounds[0].setX( qMin(qMin( p0m.x(), p1m.x() ), p2m.x() ) );
    bounds[0].setY( qMin(qMin( p0m.y(), p1m.y() ), p2m.y() ) );
    bounds[0].setZ( qMin(qMin( p0m.z(), p1m.z() ), p2m.z() ) );
    bounds[0].setW( 1.0 );
    bounds[1].setX( qMax(qMax( p0m.x(), p1m.x() ), p2m.x() ) );
    bounds[1].setY( qMax(qMax( p0m.y(), p1m.y() ), p2m.y() ) );
    bounds[1].setZ( qMax(qMax( p0m.z(), p1m.z() ), p2m.z() ) );
    bounds[1].setW( 1.0 );
}

void GLTriangle::paint( QPainter * painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height ){
    painter->save();

    QPen pen;
    pen.setWidthF( m_printWidth );
    pen.setColor( *m_color );
    painter->setPen( pen );
    painter->drawLine( project(m_d->x[0], m_d->x[1], m_d->x[2], matrixProj, matrixView, width, height ), project(m_d->x[3], m_d->x[4], m_d->x[5], matrixProj, matrixView, width, height ) );
    painter->drawLine( project(m_d->x[3], m_d->x[4], m_d->x[5], matrixProj, matrixView, width, height ), project(m_d->x[6], m_d->x[7], m_d->x[8], matrixProj, matrixView, width, height ) );
    painter->drawLine( project(m_d->x[6], m_d->x[7], m_d->x[8], matrixProj, matrixView, width, height ), project(m_d->x[0], m_d->x[1], m_d->x[2], matrixProj, matrixView, width, height ) );
    painter->restore();
}

void GLTriangle::initializeGLData(QOpenGLShaderProgram * program, bool hasVAOSupport ) {

    // Array contenente i colori del contorno sterno
    GLfloat colorData[] = {
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f
    };

    GLdouble vertexData[] = {
        GLdouble(m_d->x[0]), GLdouble(m_d->x[1]), GLdouble(m_d->x[2]),
        GLdouble(m_d->x[3]), GLdouble(m_d->x[4]), GLdouble(m_d->x[5]),
        GLdouble(m_d->x[6]), GLdouble(m_d->x[7]), GLdouble(m_d->x[8])
    };

    // crea i VertexBuffer
    m_d->vertexPositionBuffer.create();
    m_d->vertexPositionBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_d->vertexColorBuffer.create();
    m_d->vertexColorBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_d->vertexHatchColorBuffer.create();
    m_d->vertexHatchColorBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);

    if( hasVAOSupport ){

        // Disegno contorno esterno
        m_d->vertexArrayObject.create();
        m_d->vertexArrayObject.bind();

        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.allocate(vertexData, 3 * 3 * sizeof(GLdouble));
        program->enableAttributeArray("vertexPosition");
        program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);

        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.allocate(colorData, 3 * 4 * sizeof(GLfloat));
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);

        if( m_d->vertexArrayObject.isCreated() ){
            m_d->vertexArrayObject.release();
        }

        // Colore riempimento interno (cambia alpha channel)
        for(int i=0; i < 3; ++i){
            colorData[3+i*4] = 0.1f;
        }

        // Disegno riempimento interno, riutilizziamo il VBO dei vertici del contorno
        m_d->vertexHatchArrayObject.create();
        m_d->vertexHatchArrayObject.bind();

        m_d->vertexPositionBuffer.bind();
        program->enableAttributeArray("vertexPosition");
        program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);

        m_d->vertexHatchColorBuffer.bind();
        m_d->vertexHatchColorBuffer.allocate(colorData, 3 * 4 * sizeof(GLfloat));
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);

        m_d->vertexHatchArrayObject.release();
    } else {
        // Contorno esterno
        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.allocate(vertexData, 3 * 3 * sizeof(GLdouble));
        m_d->vertexPositionBuffer.release();

        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.allocate(colorData, 3 * 4 * sizeof(GLfloat));
        m_d->vertexColorBuffer.release();

        // Colore riempimento interno (cambia alpha channel)
        for(int i=0; i < 3; ++i){
            colorData[3+i*4] = 0.1f;
        }

        m_d->vertexHatchColorBuffer.bind();
        m_d->vertexHatchColorBuffer.allocate(colorData, 3 * 4 * sizeof(GLfloat));
        m_d->vertexHatchColorBuffer.release();
    }
    m_d->wasInitialized = true;
    m_updatePending = false;
}

void GLTriangle::updateGLData() {
    // coordinate punti
    if( m_d->vertexPositionBuffer.isCreated() ){
        GLdouble vertexData[] = {
            GLdouble(m_d->x[0]), GLdouble(m_d->x[1]), GLdouble(m_d->x[2]),
            GLdouble(m_d->x[3]), GLdouble(m_d->x[4]), GLdouble(m_d->x[5]),
            GLdouble(m_d->x[6]), GLdouble(m_d->x[7]), GLdouble(m_d->x[8])
        };

        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.write(0, vertexData, 3 * 3 * sizeof(GLdouble));
        m_d->vertexPositionBuffer.release();
    }

    // Colore contorno
    GLfloat colorData[] = {
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f
    };
    if( m_d->vertexColorBuffer.isCreated() ){
        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.write(0, colorData, 3 * 4 * sizeof(GLfloat));
        m_d->vertexColorBuffer.release();
    }

    // Colore riempimento interno (cambia alpha channel)
    if( m_d->vertexHatchColorBuffer.isCreated() ){
        for(int i=0; i < 3; ++i){
            colorData[3+i*4] = 0.3f;
        }
        m_d->vertexHatchColorBuffer.bind();
        m_d->vertexHatchColorBuffer.write(0, colorData, 3 * 4 * sizeof(GLfloat));
        m_d->vertexHatchColorBuffer.release();
    }
    m_updatePending = false;
}
