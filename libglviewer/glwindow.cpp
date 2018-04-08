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
#include "glwindow.h"

#include "glgroup.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>
#include <QtCore/QCoreApplication>

#include <cmath>


class GLWindowIcon{
public:
    GLWindowIcon(const char xpm[], void (GLWindow::*act)() ):
        action(act){
        int iconSize = 20;
        int iconBorder = 4;

        iconOn = QPixmap( iconSize + 2*iconBorder, iconSize + 2*iconBorder );
        iconOn.fill( Qt::transparent );
        QPainter p( &iconOn );
        p.setRenderHint( QPainter::Antialiasing );
        p.setPen(Qt::darkGray);
        p.setBrush( QBrush(Qt::lightGray) );
        p.drawRoundedRect( iconBorder-1, iconBorder-1, iconSize+2, iconSize+2, 1.0, 1.0 );
        p.drawPixmap( iconBorder,iconBorder,QPixmap(xpm).scaledToHeight(iconSize, Qt::SmoothTransformation) );
        p.end();

        iconOff = QPixmap( iconSize + 2*iconBorder, iconSize + 2*iconBorder );
        iconOff.fill( Qt::transparent );
        p.begin( &iconOff );
        p.setRenderHint( QPainter::Antialiasing );
        p.setPen(Qt::darkGray);
        p.setBrush( QBrush(Qt::transparent) );
        p.drawRoundedRect( iconBorder-1, iconBorder-1, iconSize+2, iconSize+2, 1.0, 1.0 );
        p.drawPixmap( iconBorder,iconBorder,QPixmap(xpm).scaledToHeight(iconSize, Qt::SmoothTransformation) );
        p.end();

        isPressed = false;

        rect = QRect( 0, 0, iconOn.width(), iconOn.height() );
    };
    QPixmap *icon(){
        if( isPressed ){
            return &iconOn;
        } else {
            return &iconOff;
        }
    }
    void setTopLeft( int x, int y){
        rect = QRect( x, y, iconOn.width(), iconOn.height() );
    }

    QRect rect;
    QPixmap iconOn;
    QPixmap iconOff;
    bool isPressed;
    void (GLWindow::*action)();
};

class GLWindowPrivate{
public:
    GLWindowPrivate():
        program(0),
        context(0),
        openglFunctions(0),
        device(0),
        animating( false ),
        updatePending(false),
        cameraPosition( QVector3D(0.0,0.0,0.0) ),
        cameraRotation( QVector2D(0.0,0.0) ),
        cameraZoom(0.01),
        cameraNearPlan(-100.0),
        cameraFarPlan(100.0),
        cameraRotatingBP( QPoint(-1,0)),
        isCameraRotating( false ),
        isCameraPanning( false ),
        isZoomWinActivated( false ),
        zoomWinFirstClick( false ),
        bgColor( QColor(Qt::white ) ),
        root(0),
        sceneCenter( QVector3D( 0.0,0.0,0.0) ),
        wasSysRefIconInitialized(false),
        isSysRefIconModelSize(false),
        sysRefIconModelSize(1.0),
        sysRefIconPosition(50.0),
        isSysRefIconOnOrigin(false),
        sysRefIconUpdatePending(true),
        hasVAOSupport(false){
        for( int i=0; i<3; ++i){
            currentP[i] = 0.0;
        }

    };
    ~GLWindowPrivate(){
        qDeleteAll( icons );
        icons.clear();
    }

    QOpenGLShaderProgram * program;
    QOpenGLContext * context;
    QAbstractOpenGLFunctions * openglFunctions;
    QOpenGLPaintDevice *device;

    bool animating;
    bool updatePending;

    /** Posizione della matrice negli shaders */
    GLuint matrixLocation;
    /** Matrice Vista  */
    QMatrix4x4 matrixView;
    /** Matrice Proiezione */
    QMatrix4x4 matrixProj;

    /** Posizione della camera */
    QVector3D cameraPosition;
    /** Rotazione della camera */
    QVector2D cameraRotation;
    /** Zoom della camera */
    double cameraZoom;
    /** Piano vicino della camera (proiezione) */
    double cameraNearPlan;
    /** Piano lontano della della camera (proiezione) */
    double cameraFarPlan;

    /** E' in corso una rotazione della camera. Punto base */
    QPoint cameraRotatingBP;
    /** Flag per indicare se è in corso una rotazione della camera */
    bool isCameraRotating;
    /** Flag per indicare se è in corso uno spostamento della camera */
    bool isCameraPanning;
    /** E' in corso uno spostamento della camera. Punto base */
    QVector3D cameraPanningBP;

    /** Flag per indicare se è stato attivato il comando zoomWindow */
    bool isZoomWinActivated;
    /** Flag per indicare se il primo click è stato schiacciato */
    bool zoomWinFirstClick;
    /** Rettangolo usato per il zoomWin */
    QRect zoomWinRect;

    /** Colore dello sfondo */
    QColor bgColor;

    /** Scena */
    GLGroup * root;

    /** Centro della scena */
    QVector3D sceneCenter;

    /** Ci dice se i buffer necessari per disegnare l'icona del sistema di riferimento sono
        stati creati o no */
    bool wasSysRefIconInitialized;
    /** la dimensione dell'icona del sistema di riferimento è in unità del sistema di riferimento?
        se è false l'icona è in pixel */
    bool isSysRefIconModelSize;
    /** Dimensione dell'icona del sistema di riferimento in unità del sistema di riferimento.
        Viene utilizzato se isSysRefIconModelSize è vero */
    double sysRefIconModelSize;
    /** posizione dell'icona del sistema di riferimento () nel caso non sia nell'orgine */
    double sysRefIconPosition;
    /** L'icona viene mostrata nell'origine del sistema di riferimento (se visibile) */
    bool isSysRefIconOnOrigin;
    /** L'icona del sistema di riferimento richiede un aggiornamento */
    bool sysRefIconUpdatePending;
    /** VBO contenente i vertici degli assi dell'icona sistema di riferimento */
    QOpenGLBuffer sysRefIconAxisVertexVBO;
    /** VBO contenente i colori degli assi dell'icona sistema di riferimento */
    QOpenGLBuffer sysRefIconAxisColorVBO;
    /** VBO contenente i vertici dei piani dell'icona sistema di riferimento */
    QOpenGLBuffer sysRefIconPlanVertexVBO;
    /** VBO contenente i colori dei piani dell'icona sistema di riferimento */
    QOpenGLBuffer sysRefIconPlanColorVBO;
    /** VAO assi icona di sistema */
    QOpenGLVertexArrayObject sysRefIconAxisVAO;
    /** VAO piani icona di sistema */
    QOpenGLVertexArrayObject sysRefIconPlanVAO;

    /** Variabile che ci dice se il sistema supporta o meno i VAO */
    bool hasVAOSupport;

    /** Posizione corrente del cursore */
    float currentP[3];

    QList<GLWindowIcon *> icons;
};

GLWindow::GLWindow(GLGroup * r, QWindow *parent) :
    QWindow(parent),
    m_d( new GLWindowPrivate() ){

    setScene( r );
    setSurfaceType(QWindow::OpenGLSurface);

    m_d->icons << new GLWindowIcon( ":/glviewer-images/zoomwin.svg", &GLWindow::zoomWindow);
    m_d->icons << new GLWindowIcon( ":/glviewer-images/zoomall.svg", &GLWindow::zoomExtent );

}

GLWindow::~GLWindow() {
    delete m_d;
}

bool GLWindow::event(QEvent *event) {
    if( event->type() == QEvent::UpdateRequest ){
        renderNow();
        return true;
    } else if( event->type() == QEvent::MouseMove ){
        // Se si sta muovendo il cursore, ricalcola le sue coordinate
        QMouseEvent * mEvent = static_cast<QMouseEvent *>(event);
        if( mEvent->buttons() == Qt::NoButton ){
            unProject( &(m_d->currentP[0]), &(m_d->currentP[1]), &(m_d->currentP[2]), mEvent->x(), mEvent->y(), 0.5);
            if( m_d->zoomWinFirstClick ){
                m_d->zoomWinRect.setBottomRight( mEvent->pos() );
            }
            update();
        }
    }
    return QWindow::event(event);
}

void GLWindow::exposeEvent(QExposeEvent *event) {
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void GLWindow::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);

    if (!m_d->context) {
        initialize();
    }

    m_d->context->makeCurrent( this );
    glViewport(0, 0, width(), height());

    updateMatrixProj();
    loadMatrix( m_d->matrixProj, m_d->matrixView );

    int margin = 2;
    int x = width() - margin;
    for( QList<GLWindowIcon *>::iterator i = m_d->icons.begin(); i != m_d->icons.end(); ++i){
        (*i)->setTopLeft( x - (*i)->rect.width(), margin );
        int v = ((*i)->rect.width() + margin);
        x = x - v;
    }

    if (isExposed())
        renderNow();
}

void GLWindow::mousePressEvent(QMouseEvent *event) {
    if( m_d->isZoomWinActivated ){
        if( !m_d->zoomWinFirstClick ){
            m_d->zoomWinRect.setTopLeft( event->pos() );
            m_d->zoomWinRect.setBottomRight( event->pos() );
            m_d->zoomWinFirstClick = true;
        } else {
            float xTL, yTL, zTL;
            float xTR, yTR, zTR;
            float xBR, yBR, zBR;
            unProject( &xTL, &yTL, &zTL, m_d->zoomWinRect.left(), m_d->zoomWinRect.top(), 0.5 );
            unProject( &xTR, &yTR, &zTR, m_d->zoomWinRect.right(), m_d->zoomWinRect.top(), 0.5 );
            unProject( &xBR, &yBR, &zBR, m_d->zoomWinRect.right(), m_d->zoomWinRect.bottom(), 0.5 );
            m_d->cameraPosition = QVector3D( 0.5*(xTL+xBR), 0.5*(yTL+yBR), 0.5*(zTL+zBR) );
            QVector3D top( (xTR-xTL), (yTR-yTL), (zTR-zTL));
            QVector3D right( (xBR-xTR), (yBR-yTR), (zBR-zTR));
            m_d->cameraZoom = qMax( top.length() / width(), right.length() / height() );
            updateMatrixView();
            updateMatrixProj();
            m_d->isZoomWinActivated = false;
            m_d->zoomWinFirstClick = false;
            update();
        }
    }

    if(event->button() == Qt::LeftButton){
        for( QList<GLWindowIcon *>::iterator i = m_d->icons.begin(); i != m_d->icons.end(); ++i){
            if( (*i)->rect.contains(event->x(), event->y()) ){
                (*i)->isPressed = true;
                update();
                // esegue il puntatore al metodo
                (this->*((*i)->action))();
            }
        }
    }
    if( event->button() == Qt::MiddleButton ){
        m_d->isCameraPanning = true;
        float x, y, z;
        unProject( &x, &y, &z, event->x(), event->y(), 0.5 );
        m_d->cameraPanningBP = QVector3D( x, y, z );
    }
    if( (event->button() == Qt::LeftButton) && (event->modifiers() == Qt::ControlModifier) ){
        m_d->cameraRotatingBP = event->pos();
        m_d->isCameraRotating = true;
    }
}

void GLWindow::mouseMoveEvent(QMouseEvent *event) {
    if( m_d->isCameraPanning ){
        float x, y, z;
        unProject( &x, &y, &z, event->x(), event->y(), 0.5 );
        m_d->cameraPosition += m_d->cameraPanningBP - QVector3D(x, y, z);
        updateMatrixView();
        update();
    }
    if( m_d->isCameraRotating ){
        double DRotX = (double)(event->pos().x() - m_d->cameraRotatingBP.x()) / (double)width() * 360.0;
        double DRotY = (double)(event->pos().y() - m_d->cameraRotatingBP.y()) / (double)height() * 360.0;
        m_d->cameraRotation.setX( m_d->cameraRotation.x() - DRotX );
        m_d->cameraRotation.setY( m_d->cameraRotation.y() - DRotY );

        float x,y,z;
        project( &x, &y, &z, m_d->sceneCenter.x(), m_d->sceneCenter.y(), m_d->sceneCenter.z() );

        QMatrix4x4 matrixView;
        calcMatrixView( matrixView, m_d->cameraPosition, m_d->cameraRotation );

        float newX, newY,newZ;
        unProject( &newX, &newY, &newZ, x,y,z, &(m_d->matrixProj), &matrixView );
        QVector3D newP(newX,newY,newZ);

        QVector3D DFixedP = newP - m_d->sceneCenter;
        m_d->cameraPosition = m_d->cameraPosition - DFixedP;

        updateMatrixView();
        loadMatrix( m_d->matrixProj, m_d->matrixView );

        update();
        m_d->cameraRotatingBP = event->pos();
    }
}

void GLWindow::mouseReleaseEvent(QMouseEvent *event) {
    for( QList<GLWindowIcon *>::iterator i = m_d->icons.begin(); i != m_d->icons.end(); ++i){
        if( (*i)->rect.contains(event->x(), event->y()) && (*i)->isPressed ){
            (*i)->isPressed = false;
        }
        update();
    }

    if( event->button() == Qt::MiddleButton && m_d->isCameraPanning ){
        m_d->isCameraPanning = false;
    }
    if( m_d->isCameraRotating ){
        m_d->isCameraRotating = false;
    }
}

void GLWindow::wheelEvent(QWheelEvent *event) {
    double Dzoom = (1.0 - event->angleDelta().y() / 8.0 / 90.0);

    float x,y,z;
    unProject( &x, &y, &z, event->x(), event->y(), 0.5 );
    QVector3D oldFixedP( x,y,z);

    m_d->cameraZoom *= Dzoom;
    QMatrix4x4 newMatrixProj;
    newMatrixProj.ortho( - 0.50 * width() * m_d->cameraZoom, + 0.50 * width() * m_d->cameraZoom,
                         - 0.50 * height() * m_d->cameraZoom, + 0.50 * height() * m_d->cameraZoom,
                         m_d->cameraNearPlan, m_d->cameraFarPlan);
    unProject( &x, &y, &z, event->x(), event->y(), 0.5, &newMatrixProj, &(m_d->matrixView) );
    QVector3D newFixedP( x,y,z);

    QVector3D DFixedP = newFixedP - oldFixedP;
    m_d->cameraPosition -= DFixedP;

    updateMatrixView();
    updateMatrixProj();
    loadMatrix( m_d->matrixProj, m_d->matrixView );

    update();
}

void GLWindow::mouseDoubleClickEvent(QMouseEvent * event ){
    if( event->button() == Qt::MiddleButton ){
        m_d->isCameraPanning = false;
        zoomExtent();
    }
}

void GLWindow::renderLater() {
    if (!m_d->updatePending) {
        m_d->updatePending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

void GLWindow::renderNow() {
    if (!isExposed())
        return;

    m_d->updatePending = false;

    paint();

    if (m_d->animating)
        renderLater();
}


void GLWindow::paint() {
    m_d->device->setSize( size() );
    QPainter painter(m_d->device);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.beginNativePainting();
    // Visualizza solo gli oggetti la cui normale è rivolta verso l'osservatore
    // glEnable(GL_CULL_FACE);

    // E' necessario settare ogni volta tali parametri poiché vengono resettati da QPainter
    // Attiva canale alpha
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Elimina le parti nascoste: la primitiva piu' vicina all'osservatore (z minima) copre le altre
    glEnable(GL_DEPTH_TEST);

    paintGL();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    painter.endNativePainting();

    paint(&painter);

    m_d->context->swapBuffers(this);
}

void GLWindow::paintGL() {
    m_d->program->bind();

    m_d->context->makeCurrent( this );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glClearColor( m_d->bgColor.redF(), m_d->bgColor.greenF(), m_d->bgColor.blueF(), m_d->bgColor.alphaF() );

    glEnable(GL_CULL_FACE);
    drawSysRefIcon();
    glDisable(GL_CULL_FACE);

    glClear( GL_DEPTH_BUFFER_BIT);

    loadMatrix( m_d->matrixProj, m_d->matrixView );
    m_d->root->paintGL( m_d->program, m_d->hasVAOSupport );

    QMatrix4x4 identity;
    loadMatrix( m_d->matrixProj, identity );
    m_d->root->paintGL( m_d->program, m_d->matrixView, m_d->cameraZoom, m_d->hasVAOSupport );

    m_d->program->release();
}

void GLWindow::paint(QPainter *painter) {
    painter->save();

    QPen p;
    p.setColor( Qt::black );
    p.setWidth(3);
    painter->setPen( p );
    QString txt = "("+ QString::number(m_d->currentP[0]) + "," + QString::number(m_d->currentP[1]) + "," + QString::number(m_d->currentP[2]) + ")";
    int h = 60, w = 100, m = 10;
    QRectF rectTxt = painter->boundingRect( width() -w-m, height()-h-m, w, h, Qt::AlignRight | Qt::AlignBottom, txt );
    painter->drawText( rectTxt, txt );

    for( QList<GLWindowIcon *>::iterator i = m_d->icons.begin(); i != m_d->icons.end(); ++i ){
        painter->drawPixmap( (*i)->rect, *((*i)->icon()));
    }

    if( m_d->zoomWinFirstClick ){
        painter->drawRect( m_d->zoomWinRect );
    }

    painter->restore();

}

void GLWindow::initialize() {
    // Specify the format and create platform-specific surface
    QSurfaceFormat format;
    /* format.setDepthBufferSize( 24 );
    format.setMajorVersion( 2 );
    format.setMinorVersion( 1 );
    format.setSamples( 4 );*/
    format.setProfile( QSurfaceFormat::CoreProfile );
    setFormat( format );
    create();

    m_d->context = new QOpenGLContext(this);
    m_d->context->setFormat(format);
    m_d->context->create();
    m_d->context->makeCurrent(this);

    initializeOpenGLFunctions();
    m_d->root->initializeOpenGLFunctions();

    if (!m_d->device)
        m_d->device = new QOpenGLPaintDevice();

    initShaders();

    // posizione della matrice
    m_d->matrixLocation = m_d->program->uniformLocation("matrix");

    m_d->hasVAOSupport = hasVAOSupport();
}

void GLWindow::initShaders() {
    m_d->context->makeCurrent(this);

    // Sovrasvcrive la codifica fintantoche' non sono compilati gli shaders
    setlocale(LC_NUMERIC, "C");

    m_d->program = new QOpenGLShaderProgram(this);

    // Compila il Vertex Shader
    if (!m_d->program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.glsl"))
        close();

    // Compila il Fragment Shader
    if (!m_d->program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragment.glsl"))
        close();

    // Link degli shaders
    if (!m_d->program->link())
        close();

    // Ripristina la codifica del sistema
    setlocale(LC_ALL, "");
}

void GLWindow::setAnimating(bool animating) {
    m_d->animating = animating;

    if (animating)
        renderLater();
}

void GLWindow::setCameraLookAt(double posX, double posY, double posZ, double targetX, double targetY, double targetZ) {
    QVector4D position( -posX, -posY, -posZ, 1.0);

    // determina la direzione verso cui è orientata la camera
    QVector4D forward( posX - targetX, posY - targetY, posZ - targetZ, 0.0);
    forward.normalize();

    // assume come direzione superiore il versore y
    QVector4D up(0.0, 1.0, 0.0, 0.0);

    // calcola left come prodotto vettorial di up e forward
    QVector4D left = up * forward;

    // ricalcola up
    up = forward * left;

    // copia i vettori nella matrice
    m_d->matrixView.setToIdentity();
    m_d->matrixView.setColumn(0, left);
    m_d->matrixView.setColumn(1, up);
    m_d->matrixView.setColumn(2, forward);
    m_d->matrixView.setColumn(3, position);
    emit matrixViewChanged( m_d->matrixView.data() );
    update();
}

void GLWindow::setCameraPos(double posX, double posY, double posZ) {
    QVector4D position( -posX, -posY, -posZ, 1.0 );
    m_d->matrixView.setColumn( 3, position );
    emit matrixViewChanged( m_d->matrixView.data() );
    update();
}

void GLWindow::setCameraPosRot(double posX, double posY, double posZ, double rotX, double rotY) {
    m_d->cameraPosition.setX( posX );
    m_d->cameraPosition.setY( posY );
    m_d->cameraPosition.setZ( posZ );
    m_d->cameraRotation.setX( rotX );
    m_d->cameraRotation.setY( rotY );
    updateMatrixView();
    loadMatrix( m_d->matrixProj, m_d->matrixView );

    update();
}

void GLWindow::project(float *winX, float *winY, float *winZ, float posX, float posY, float posZ, QMatrix4x4 *mProj, QMatrix4x4 *mView) {
    QMatrix4x4 matrixView = m_d->matrixView;
    if( mView != NULL ){
        matrixView = *mView;
    }
    QMatrix4x4 matrixProj = m_d->matrixProj;
    if( mProj != NULL ){
        matrixProj = *mProj;
    }

    QVector4D PEye = matrixProj * matrixView * QVector4D(posX, posY, posZ,1.0);
    *winX = 0.0 + (width() * (PEye.x() + 1.0) ) / 2.0;
    *winY = 0.0 + (height() * (PEye.y() + 1.0) ) / 2.0;
    *winZ = (PEye.z() + 1.0) / 2.0;

    *winY = (double)height() - *winY;           // Passiamo da coordinate mouse a coordinate opengl
}

void GLWindow::unProject(float * posX, float * posY, float * posZ, float winX, float winY, float winZ, QMatrix4x4 * mProj, QMatrix4x4 * mView ) {
    winY = (double)height() - winY;           // Passiamo da coordiante mouse a coordinate opengl

    QMatrix4x4 matrixView = m_d->matrixView;
    if( mView != NULL ){
        matrixView = *mView;
    }
    QMatrix4x4 matrixProj = m_d->matrixProj;
    if( mProj != NULL ){
        matrixProj = *mProj;
    }
    QMatrix4x4 m = matrixProj * matrixView;
    QVector4D v( 2.0 * winX / width() - 1.0, 2.0 * winY / height()-1.0, 2.0 * winZ - 1.0, 1.0 );
    QVector4D ret = m.inverted() * v;
    *posX = ret.x();
    *posY = ret.y();
    *posZ = ret.z();
}

void GLWindow::updateMatrixView() {
    calcMatrixView( m_d->matrixView, m_d->cameraPosition, m_d->cameraRotation );

    emit matrixViewChanged( m_d->matrixView.data() );
}

void GLWindow::calcMatrixView(QMatrix4x4 &matrixView, QVector3D &cameraPosition, QVector2D &cameraRotation) {
    matrixView.setToIdentity();

    QMatrix4x4 FMatrix;
    FMatrix.setToIdentity();

    double xAngle = normalizeAngle( cameraRotation.x() );
    double yAngle = normalizeAngle( cameraRotation.y() );
    FMatrix.rotate( xAngle, 0.0, 0.0, 1.0 );
    FMatrix.rotate( yAngle, 1.0, 0.0, 0.0 );
    QVector4D F = FMatrix  * QVector4D( 0.0, 0.0, 1.0, 0.0);
    QVector3D f = F.toVector3D();
    f.normalize();

    QVector3D s;
    if( yAngle < 0.0 ){
        s = QVector3D::crossProduct( f, QVector3D(0.0, 0.0, 1.0) );
    } else {
        s = QVector3D::crossProduct( f, QVector3D(0.0, 0.0, -1.0) );
    }
    s.normalize();

    if( s.length() < 1.0e-9 ){
        QVector4D ps = FMatrix * QVector4D( QVector3D( 1.0, 0.0, 0.0), 0.0 );
        s = ps.toVector3D();
    }

    QVector3D u = -QVector3D::crossProduct( s, f);
    u.normalize();

    matrixView.setRow( 0, QVector4D( s, 0.0 ) );
    matrixView.setRow( 1, QVector4D( u, 0.0 ) );
    matrixView.setRow( 2, QVector4D( f, 0.0 ) );

    matrixView.translate( -cameraPosition );
}

void GLWindow::updateMatrixProj() {
    m_d->matrixProj.setToIdentity();
    m_d->matrixProj.ortho( - 0.50 * width() * m_d->cameraZoom, + 0.50 * width() * m_d->cameraZoom,
                           - 0.50 * height() * m_d->cameraZoom, + 0.50 * height() * m_d->cameraZoom,
                           m_d->cameraNearPlan , m_d->cameraFarPlan );

    emit matrixProjChanged( m_d->matrixProj.data() );
}

double GLWindow::normalizeAngle( double a ) {
    while( a <= -180.0 ){
        a += 360.0;
    }
    while( a > 180.0 ){
        a -= 360.0;
    }
    return a;
}

void GLWindow::loadMatrix(QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView) {
    m_d->context->makeCurrent( this );

    if( m_d->program != NULL ){
        m_d->program->setUniformValue( m_d->matrixLocation, matrixProj * matrixView);
    }
}

void GLWindow::zoomExtent() {
    if(m_d->root){
        QVector4D bounds[2];
        m_d->root->boundingBoxGL( bounds, m_d->matrixView );
        QVector4D pm = 0.5 * (bounds[0] + bounds[1]);
        QVector4D cp = m_d->matrixView.inverted() * pm;
        m_d->cameraPosition = cp.toVector3D();
        updateMatrixView();

        double z0 = (bounds[1].x() - bounds[0].x()) / width();
        double z1 = (bounds[1].y() - bounds[0].y()) / height();
        m_d->cameraZoom = 1.1 * qMax( z0, z1 );
        if( m_d->cameraZoom == 0.0 ){
            m_d->cameraZoom = 1.0;
        }
        QVector4D Dp = bounds[1]-pm;
        m_d->cameraFarPlan = 3.0 * Dp.length();
        m_d->cameraNearPlan = - m_d->cameraFarPlan;
        updateMatrixProj();

        loadMatrix( m_d->matrixProj, m_d->matrixView );

        update();
    }
}

void GLWindow::zoomWindow() {
    m_d->isZoomWinActivated = true;
}

void GLWindow::setScene(GLGroup *s) {
    m_d->root = s;
    updateSceneCenter();
    connect( m_d->root, SIGNAL(groupChanged()), this, SLOT(updateSceneCenter()) );
    update();
    connect( m_d->root, SIGNAL(groupChanged()), this, SLOT(update()) );
}

void GLWindow::updateSceneCenter(){
    QVector4D bounds[2];
    m_d->root->boundingBoxGL( bounds, m_d->matrixView );
    QVector4D cp = 0.5 * (bounds[0] + bounds[1]);
    cp = m_d->matrixView.inverted()  * cp;
    m_d->sceneCenter = cp.toVector3D();
}

void GLWindow::drawSysRefIcon() {
    // Imposta la matrice proiezione per disegnare l'icona del sistema di riferimento
    QMatrix4x4 matrixP;
    QMatrix4x4 matrixV = m_d->matrixView;

    matrixP.setToIdentity();
    matrixP.ortho(  - 0.50 * width() * m_d->cameraZoom, + 0.50 * width() * m_d->cameraZoom,
                    - 0.50 * height() * m_d->cameraZoom, + 0.50 * height() * m_d->cameraZoom,
                    -2.0, 2.0 );

    if( !m_d->isSysRefIconModelSize ){
        float x1, y1, z1, x2, y2, z2;
        unProject(&x1, &y1, &z1, m_d->sysRefIconPosition, height()-m_d->sysRefIconPosition, 0.5, &matrixP, &matrixV );
        unProject(&x2, &y2, &z2, 1.6 * m_d->sysRefIconPosition + m_d->sysRefIconModelSize, height()-m_d->sysRefIconPosition, 0.5, &matrixP, &matrixV );
        double sc = sqrt( pow(x2-x1, 2.0) + pow(y2-y1, 2.0) + pow(z2-z1, 2.0) );
        if( sc!= 0.0 ){
            matrixP.scale( sc );
        }
    }

    // Imposta la matrice vista per disegnare l'icona del sistema di riferimento
    float x,y,z;
    if( m_d->isSysRefIconOnOrigin ){
        float winX, winY, winZ;
        project( &winX, &winY, &winZ, 0.0, 0.0, 0.0 );
        if( (winX < 0.0 || winX > width()) || (winY < 0.0 || winY > height()) ){
            winX =  m_d->sysRefIconPosition;
            winY = height()-m_d->sysRefIconPosition;
        }
        unProject(&x, &y, &z, winX, winY, winZ, &matrixP, &matrixV );
    } else {
        unProject(&x, &y, &z, m_d->sysRefIconPosition, height()-m_d->sysRefIconPosition, 0.5, &matrixP, &matrixV );
    }
    matrixV.translate(x, y, z);

    // Carica le matici vista e proiezione per disegnare l'icona del sistema di riferimento
    loadMatrix( matrixP, matrixV );

    glLineWidth(3);

    if( !m_d->wasSysRefIconInitialized ){
        initializeSysRefIconGLData();
    }
    if( m_d->sysRefIconUpdatePending ){
        updateSysRefIconGLData();
    }

    if( m_d->hasVAOSupport ){
        m_d->sysRefIconAxisVAO.bind();
        glDrawArrays(GL_LINES, 0, 6 );
        m_d->sysRefIconAxisVAO.release();

        m_d->sysRefIconPlanVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 18 );
        m_d->sysRefIconPlanVAO.release();
    } else {
        m_d->sysRefIconAxisVertexVBO.bind();
        m_d->program->enableAttributeArray( "vertexPosition" );
        m_d->program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);
        m_d->sysRefIconAxisVertexVBO.release();
        m_d->sysRefIconAxisColorVBO.bind();
        m_d->program->enableAttributeArray( "vertexColor" );
        m_d->program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);
        m_d->sysRefIconAxisColorVBO.release();
        glDrawArrays(GL_LINES, 0, 6 );

        m_d->sysRefIconPlanVertexVBO.bind();
        m_d->program->enableAttributeArray( "vertexPosition" );
        m_d->program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);
        m_d->sysRefIconPlanVertexVBO.release();
        m_d->sysRefIconPlanColorVBO.bind();
        m_d->program->enableAttributeArray( "vertexColor" );
        m_d->program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);
        m_d->sysRefIconPlanColorVBO.release();
        glDrawArrays(GL_TRIANGLES, 0, 18 );
    }
}

void GLWindow::initializeSysRefIconGLData() {
    GLdouble m = 1.0;
    if( m_d->isSysRefIconModelSize ){
        m = (GLdouble)m_d->sysRefIconModelSize;
    }

    // Vertici usati per disegnare l'icona del sistema di riferimento
    GLdouble verticesAxis[] = {0.0, 0.0, 0.0,
                               m, 0.0, 0.0,
                               0.0, 0.0, 0.0,
                               0.0, m, 0.0,
                               0.0, 0.0, 0.0,
                               0.0, 0.0, m};

    // Colori usati per disegnare gli assi del sistema di riferimento
    GLfloat colorsAxis[] = {
        1.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 1.0 };

    // Vertici usati per disegnare l'icona del sistema di riferimento
    GLdouble verticesPlan[] = {0.0, 0.0, 0.0,
                               m, 0.0, 0.0,
                               0.0, m, 0.0,
                               m, 0.0, 0.0,
                               m, m, 0.0,
                               0.0, m, 0.0,
                               0.0, 0.0, 0.0,
                               0.0, 0.0, m,
                               m, 0.0, 0.0,
                               0.0, 0.0, m,
                               m, 0.0, m,
                               m, 0.0, 0.0,
                               0.0, 0.0, 0.0,
                               0.0, m, 0.0,
                               0.0, 0.0, m,
                               0.0, m, 0.0,
                               0.0, m, m,
                               0.0, 0.0, m};

    // Colori usati per disegnare gli assi del sistema di riferimento
    GLfloat alphaPlan = 0.2;
    GLfloat colorsPlan[] = {
        1.0, 1.0, 0.0, alphaPlan,
        1.0, 1.0, 0.0, alphaPlan,
        1.0, 1.0, 0.0, alphaPlan,
        1.0, 1.0, 0.0, alphaPlan,
        1.0, 1.0, 0.0, alphaPlan,
        1.0, 1.0, 0.0, alphaPlan,
        1.0, 0.0, 1.0, alphaPlan,
        1.0, 0.0, 1.0, alphaPlan,
        1.0, 0.0, 1.0, alphaPlan,
        1.0, 0.0, 1.0, alphaPlan,
        1.0, 0.0, 1.0, alphaPlan,
        1.0, 0.0, 1.0, alphaPlan,
        0.0, 1.0, 1.0, alphaPlan,
        0.0, 1.0, 1.0, alphaPlan,
        0.0, 1.0, 1.0, alphaPlan,
        0.0, 1.0, 1.0, alphaPlan,
        0.0, 1.0, 1.0, alphaPlan,
        0.0, 1.0, 1.0, alphaPlan};

    // crea i buffer
    m_d->sysRefIconAxisVertexVBO.create();
    m_d->sysRefIconAxisVertexVBO.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_d->sysRefIconAxisColorVBO.create();
    m_d->sysRefIconAxisColorVBO.setUsagePattern( QOpenGLBuffer::StaticDraw );

    m_d->sysRefIconPlanVertexVBO.create();
    m_d->sysRefIconPlanVertexVBO.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_d->sysRefIconPlanColorVBO.create();
    m_d->sysRefIconPlanColorVBO.setUsagePattern( QOpenGLBuffer::StaticDraw );

    if( m_d->hasVAOSupport ){
        m_d->sysRefIconAxisVAO.create();
        m_d->sysRefIconAxisVAO.bind();

        m_d->sysRefIconAxisVertexVBO.bind();
        m_d->sysRefIconAxisVertexVBO.allocate( verticesAxis, 6*3*sizeof(GLdouble));
        m_d->program->enableAttributeArray( "vertexPosition" );
        m_d->program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);

        m_d->sysRefIconAxisColorVBO.bind();
        m_d->sysRefIconAxisColorVBO.allocate( colorsAxis, 6*4*sizeof(GLfloat));
        m_d->program->enableAttributeArray( "vertexColor" );
        m_d->program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);

        m_d->sysRefIconAxisVertexVBO.release();
        m_d->sysRefIconAxisColorVBO.release();

        m_d->sysRefIconAxisVAO.release();

        m_d->sysRefIconPlanVAO.create();
        m_d->sysRefIconPlanVAO.bind();

        m_d->sysRefIconPlanVertexVBO.bind();
        m_d->sysRefIconPlanVertexVBO.allocate( verticesPlan, 18*3*sizeof(GLdouble));
        m_d->program->enableAttributeArray( "vertexPosition" );
        m_d->program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);

        m_d->sysRefIconPlanColorVBO.bind();
        m_d->sysRefIconPlanColorVBO.allocate( colorsPlan, 18*4*sizeof(GLfloat));
        m_d->program->enableAttributeArray( "vertexColor" );
        m_d->program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);

        m_d->sysRefIconPlanVertexVBO.release();
        m_d->sysRefIconPlanColorVBO.release();

        m_d->sysRefIconPlanVAO.release();
    } else {
        m_d->sysRefIconAxisVertexVBO.bind();
        m_d->sysRefIconAxisVertexVBO.allocate( verticesAxis, 6*3*sizeof(GLdouble));
        m_d->sysRefIconAxisVertexVBO.release();

        m_d->sysRefIconAxisColorVBO.bind();
        m_d->sysRefIconAxisColorVBO.allocate( colorsAxis, 6*4*sizeof(GLfloat));
        m_d->sysRefIconAxisColorVBO.release();

        m_d->sysRefIconPlanVertexVBO.bind();
        m_d->sysRefIconPlanVertexVBO.allocate( verticesPlan, 18*3*sizeof(GLdouble));
        m_d->sysRefIconPlanVertexVBO.release();

        m_d->sysRefIconPlanColorVBO.bind();
        m_d->sysRefIconPlanColorVBO.allocate( colorsPlan, 18*4*sizeof(GLfloat));
        m_d->sysRefIconPlanColorVBO.release();
    }

    m_d->wasSysRefIconInitialized = true;
    m_d->updatePending = false;
}

void GLWindow::updateSysRefIconGLData() {
    GLdouble m = 1.0;
    if( m_d->isSysRefIconModelSize ){
        m = (GLdouble)m_d->sysRefIconModelSize;
    }

    // Vertici usati per disegnare l'icona del sistema di riferimento
    GLdouble verticesAxis[] = {0.0, 0.0, 0.0,
                               m, 0.0, 0.0,
                               0.0, 0.0, 0.0,
                               0.0, m, 0.0,
                               0.0, 0.0, 0.0,
                               0.0, 0.0, m};

    m_d->sysRefIconAxisVertexVBO.bind();
    m_d->sysRefIconAxisVertexVBO.write( 0, verticesAxis, 6*3*sizeof(GLdouble));
    m_d->sysRefIconAxisVertexVBO.release();

    // Vertici usati per disegnare l'icona del sistema di riferimento
    GLdouble verticesPlan[] = {0.0, 0.0, 0.0,
                               m, 0.0, 0.0,
                               0.0, m, 0.0,
                               m, 0.0, 0.0,
                               m, m, 0.0,
                               0.0, m, 0.0,
                               0.0, 0.0, 0.0,
                               0.0, 0.0, m,
                               m, 0.0, 0.0,
                               0.0, 0.0, m,
                               m, 0.0, m,
                               m, 0.0, 0.0,
                               0.0, 0.0, 0.0,
                               0.0, m, 0.0,
                               0.0, 0.0, m,
                               0.0, m, 0.0,
                               0.0, m, m,
                               0.0, 0.0, m};

    m_d->sysRefIconPlanVertexVBO.bind();
    m_d->sysRefIconPlanVertexVBO.write( 0, verticesPlan, 18*3*sizeof(GLdouble));
    m_d->sysRefIconPlanVertexVBO.release();

    m_d->sysRefIconUpdatePending = false;
}

void GLWindow::update(){
    m_d->updatePending = true;
    QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
}

QColor GLWindow::bgColor() const {
    return m_d->bgColor;
}

void GLWindow::setBgColor(const QColor &value) {
    if( m_d->bgColor != value ){
        m_d->bgColor = value;
        update();
    }
}

double GLWindow::sysRefIconPosition() const {
    return m_d->sysRefIconPosition;
}

void GLWindow::setSysRefIconPosition(double value) {
    if( m_d->sysRefIconPosition != value ){
        m_d->sysRefIconPosition = value;
        m_d->sysRefIconUpdatePending = true;
        update();
    }
}

double GLWindow::sysRefIconModelSize() const {
    return m_d->sysRefIconModelSize;
}

void GLWindow::setSysRefIconModelSize(double value) {
    if(m_d->sysRefIconModelSize != value){
        m_d->sysRefIconModelSize = value;
        m_d->sysRefIconUpdatePending = true;
        update();
    }
}

bool GLWindow::isSysRefIconModelSize() const {
    return m_d->isSysRefIconModelSize;
}

void GLWindow::setIsSysRefIconModelSize(bool value) {
    if( m_d->isSysRefIconModelSize != value){
        m_d->isSysRefIconModelSize = value;
        m_d->sysRefIconUpdatePending = true;
        update();
    }
}
bool GLWindow::isSysRefIconOnOrigin() const {
    return m_d->isSysRefIconOnOrigin;
}

void GLWindow::setIsSysRefIconOnOrigin(bool value) {
    if( m_d->isSysRefIconOnOrigin != value ){
        m_d->isSysRefIconOnOrigin = value;
        m_d->sysRefIconUpdatePending = true;
        update();
    }
}

bool GLWindow::hasVAOSupport() {
    if ( m_d->context ){
        if( m_d->context->format().majorVersion() >= 3 ){
            return true;
        }
        if( m_d->context->hasExtension("GL_ARB_vertex_array_object")){
            return true;
        }
        if( m_d->context->hasExtension("GL_OES_vertex_array_object")){
            return true;
        }
    }
    return false;
}
