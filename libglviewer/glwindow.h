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
#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>

class GLWindowPrivate;
class GLGroup;

class GLWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLWindow( GLGroup *r, QWindow *parent = 0);
    ~GLWindow();
    
    void setAnimating(bool animating);

    void setCameraLookAt(double posX, double posY, double posZ, double targetX, double targetY, double targetZ);
    void setCameraPos(double posX, double posY, double posZ);
    void setCameraPosRot(double posX, double posY, double posZ, double rotX, double rotY);

    void setScene(GLGroup *s);

    QColor bgColor() const;
    void setBgColor(const QColor &value);

    double sysRefIconPosition() const;
    void setSysRefIconPosition(double value);

    double sysRefIconModelSize() const;
    void setSysRefIconModelSize(double value);

    bool isSysRefIconModelSize() const;
    void setIsSysRefIconModelSize(bool value = true );

    bool isSysRefIconOnOrigin() const;
    void setIsSysRefIconOnOrigin(bool value = true );

    bool hasVAOSupport();

public slots:
    void update();

    void zoomExtent();
    void zoomWindow();

protected:
    bool event(QEvent *event);
    void exposeEvent(QExposeEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    virtual void initialize();
    virtual void initShaders();
    virtual void paint();
    virtual void paintGL();
    virtual void paint(QPainter *painter);

    double normalizeAngle(double a);
    void loadMatrix(QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView);
    void updateMatrixProj();
    void calcMatrixView(QMatrix4x4 &matrixView, QVector3D &cameraPosition, QVector2D &cameraRotation);
    void updateMatrixView();

    void project(float *winX, float *winY, float *winZ, float posX, float posY, float posZ, QMatrix4x4 *mProj=NULL, QMatrix4x4 *mView=NULL);
    void unProject(float *posX, float *posY, float *posZ, float winX, float winY, float winZ, QMatrix4x4 *mProj=NULL, QMatrix4x4 *mView=NULL);

    void drawSysRefIcon();
    void initializeSysRefIconGLData();
    void updateSysRefIconGLData();

protected slots:
    void renderLater();
    void renderNow();
    void updateSceneCenter();


private:
    GLWindowPrivate * m_d;

signals:
    void matrixViewChanged( float * );
    void matrixProjChanged( float * );
};

#endif // GLWINDOW_H
