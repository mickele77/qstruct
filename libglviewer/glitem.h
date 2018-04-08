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
#ifndef GLITEM_H
#define GLITEM_H

#include <Qt>

class GLGroup;
class QColor;
class QVector4D;
class QMatrix4x4;
class QRectF;
class QPointF;
class QPainter;
class QOpenGLShaderProgram;

class QOpenGLFunctions;

class GLItem {
public:
    enum PickingMode{
        window,
        cross
    };

    GLItem( GLGroup * parent = 0);
    virtual ~GLItem();

    QColor color();
    void setColor ( const QColor & c );
    void setColor ( Qt::GlobalColor c );

    void setGroup (GLGroup *);
    GLGroup * group();

    int width();
    void setWidth( int );

    double printWidth();
    void setPrintWidth( double );

    virtual void boundingBoxGL( QVector4D bounds[2], QMatrix4x4 & mView ) = 0;
    virtual void paintGL(QOpenGLShaderProgram * program, bool hasVAOSup = false ) = 0;
    virtual void paintGL(QOpenGLShaderProgram * program, QMatrix4x4 & matrixView, double zoom, bool hasVAOSupport = false );

    virtual void paint(QPainter *painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height ) = 0;
    virtual QList<GLItem *> pickedObjects( QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int w, int h, QRectF selectionRect, PickingMode pm );

    void initializeOpenGLFunctions();

protected:
    GLGroup * m_group;
    QColor * m_color;
    int m_width;
    double m_printWidth;
    bool m_updatePending;

    QPointF project(double posX, double posY, double posZ, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height);

    static QOpenGLFunctions m_functions;
};

#endif // GLITEM_H
