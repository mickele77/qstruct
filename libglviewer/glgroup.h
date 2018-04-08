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
#ifndef GLGROUP_H
#define GLGROUP_H

#include <QObject>
#include "glitem.h"

class GLGroupPrivate;
class GLItem;
class QVector3D;

class GLGroup: public QObject, public GLItem {
    Q_OBJECT
public:
    GLGroup( GLGroup * = NULL );

    void paintGL(QOpenGLShaderProgram *, bool hasVAOSupport = false );
    void paintGL(QOpenGLShaderProgram * program, QMatrix4x4 & matrixView , double zoom, bool hasVAOSupport);
    void boundingBoxGL( QVector4D bounds[2], QMatrix4x4 & mView );

    void paint(QPainter *painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height);

    void addGLItem( GLItem * );
    void removeGLItem( GLItem * );

    QList<GLItem *> pickedObjects( QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int w, int h, QRectF selectionRect, PickingMode pm );

    void setGroupChanged();

private:
    GLGroupPrivate * m_d;

signals:
    void groupChanged();

};

#endif // GLGROUP_H
