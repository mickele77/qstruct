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
#ifndef GLLINE_H
#define GLLINE_H

#include "glitem.h"

class GLLinePrivate;

class GLLine : public GLItem
{
public:
    explicit GLLine( double x0 = 0.0, double y0 = 0.0, double z0 = 0.0, double x1 = 1.0, double y1 = 1.0, double z1 = 1.0, GLGroup * gr = NULL);

    void setP(int i, double x, double y, double z = 0.0);
    void setP0( double, double, double = 0.0);
    void setP1( double, double, double = 0.0);

    void boundingBoxGL(QVector4D bounds[2], QMatrix4x4 & mView );
    void paintGL(QOpenGLShaderProgram *, bool hasVAOSupport = false );
    void paintGL(QOpenGLShaderProgram *, QMatrix4x4 &);

    void paint(QPainter *painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height);

    QList<GLItem *> pickedObjects(QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int w, int h, QRectF selectionRect, GLItem::PickingMode pm);
private:
    GLLinePrivate * m_d;

    void initializeGLData(QOpenGLShaderProgram *program, bool hasVAOSupport);
    void updateGLData();
};

#endif // GLLINE_H
