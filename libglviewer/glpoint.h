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
#ifndef GLPOINT_H
#define GLPOINT_H

#include "glitem.h"

class GLPointPrivate;

class GLPoint : public GLItem
{
public:
    explicit GLPoint(double xTmp = 0.0, double yTmp = 0.0, double zTmp = 0.0, GLGroup * gr = NULL);

    void setP(double x, double y, double z = 0.0);

    void boundingBoxGL( QVector4D bounds[2], QMatrix4x4 & mView );
    void paintGL(QOpenGLShaderProgram * program, bool hasVAOSupport = false );
    void paintGL(QOpenGLShaderProgram * program, QMatrix4x4 & modelView );
    void paint(QPainter *painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height );

    QList<GLItem *> pickedObjects(QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int w, int h, QRectF selectionRect, GLItem::PickingMode pm);

private:
    GLPointPrivate * m_d;

    void initializeGLData(QOpenGLShaderProgram *program, bool hasVAOSupport = false );
    void updateGLData();
    void updateGLProjectedData();
};

#endif // GLPOINT_H
