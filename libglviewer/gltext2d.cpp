#include "gltext2d.h"

#include <QPainter>
#include <QPixmap>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

#include <QMatrix4x4>
#include <QFontMetricsF>
#include <cmath>

struct GLText2DPrivate {
    GLText2DPrivate(const QFont &f, double xTmp=0.0, double yTmp=0.0, double zTmp=0.0);
    ~GLText2DPrivate();

    QFont font;
    QFontMetricsF fontMetrics;
    QFont printFont;
    GLint xOffset;
    GLint yOffset;

    double x[3];
    QString text;

    QOpenGLVertexArrayObject vertexArrayObject;
    QOpenGLBuffer vertexPositionBuffer;
    QOpenGLBuffer vertexColorBuffer;
    QOpenGLBuffer vertexIndexBuffer;
    GLuint textureId;
    QOpenGLBuffer vertexTexCoordBuffer;

    bool wasInitialized;
};

GLText2DPrivate::GLText2DPrivate(const QFont &f, double xTmp, double yTmp, double zTmp)
    : font(f), fontMetrics(f),
      printFont(f),
      xOffset(0), yOffset(0),
      vertexIndexBuffer(QOpenGLBuffer::IndexBuffer){
    x[0] = xTmp;
    x[1] = yTmp;
    x[2] = zTmp;
    text = "Ciao";
}

GLText2DPrivate::~GLText2DPrivate() {
    // glDeleteTextures(1, &texture);
}

GLText2D::GLText2D(const QFont &f, double xTmp, double yTmp, double zTmp, GLGroup * s ) :
    GLItem(s),
    m_d(new GLText2DPrivate(f, xTmp, yTmp, zTmp)) {
}

GLText2D::~GLText2D() {
    delete m_d;
}

QFont GLText2D::font() const {
    return m_d->font;
}

QFontMetricsF GLText2D::fontMetrics() const {
    return m_d->fontMetrics;
}

void GLText2D::paintGL(QOpenGLShaderProgram * program, bool hasVAOSupport){
    Q_UNUSED( program );
    Q_UNUSED( hasVAOSupport );
}

void GLText2D::paintGL(QOpenGLShaderProgram *program, QMatrix4x4 &modelView, double zoom, bool hasVAOSupport) {
    if( !(m_d->wasInitialized) ){
        initializeGLData( program, modelView, zoom, hasVAOSupport );
    } else {
        updateGLData( modelView, zoom );
    }

    m_functions.glLineWidth( m_width );

    bool pV = m_functions.glIsEnabled( GL_DEPTH_TEST );

    if( pV ){
        m_functions.glDisable(GL_DEPTH_TEST);
    }

    program->setUniformValue("outVertexUseTex", GLfloat(1.0) );

    m_functions.glActiveTexture(GL_TEXTURE0);
    m_functions.glBindTexture(GL_TEXTURE_2D, m_d->textureId);
    program->setUniformValue("texture", 0 );


    if( hasVAOSupport ){
        m_d->vertexArrayObject.bind();
        m_functions.glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0 );
        m_d->vertexArrayObject.release();
    } else {
        m_d->vertexColorBuffer.bind();
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0, 4);
        m_d->vertexColorBuffer.release();

        m_d->vertexPositionBuffer.bind();
        program->enableAttributeArray("vertexPosition");
        program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3);
        m_d->vertexPositionBuffer.release();

        m_d->vertexTexCoordBuffer.bind();
        program->enableAttributeArray("vertexTexCoord");
        program->setAttributeBuffer("vertexTexCoord", GL_FLOAT, 0, 2);
        m_d->vertexTexCoordBuffer.release();

        m_d->vertexIndexBuffer.bind();

        m_functions.glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);
    }

    if( pV ){
        m_functions.glEnable(GL_DEPTH_TEST);
    }

    program->setUniformValue("outVertexUseTex", GLfloat(0.0) );

}

void GLText2D::initializeGLData(QOpenGLShaderProgram * program, QMatrix4x4 &matrixView, double zoom, bool hasVAOSupport ) {

    // Array contenente i colori del contorno sterno
    GLfloat colorData[] = {
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f
    };

    // Vertici
    QRectF bRect = m_d->fontMetrics.boundingRect( m_d->text );
    double l = bRect.left() / zoom ;
    double r = bRect.right() / zoom;
    double t = bRect.bottom() / zoom;
    double b = bRect.top() / zoom;
    QVector4D p( float(m_d->x[0]), float(m_d->x[1]), float(m_d->x[2]), float(1.0) );
    p = matrixView * p;

    GLdouble vertexData[] = {
        GLdouble(p.x()+l), GLdouble(p.y()+b), GLdouble(p.z()),
        GLdouble(p.x()+r), GLdouble(p.y()+b), GLdouble(p.z()),
        GLdouble(p.x()+l), GLdouble(p.y()+t), GLdouble(p.z()),
        GLdouble(p.x()+r), GLdouble(p.y()+t), GLdouble(p.z())
    };

    // coordinate della texture
    GLfloat texCoord[] = {
        GLfloat(0.0), GLfloat(0.0),
        GLfloat(1.0), GLfloat(0.0),
        GLfloat(0.0), GLfloat(1.0),
        GLfloat(1.0), GLfloat(1.0)
    };


    // Indici
    GLubyte indexData[] = {
        0,  1,  2,  3
    };

    // crea i VertexBuffer
    m_d->vertexPositionBuffer.create();
    m_d->vertexPositionBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_d->vertexColorBuffer.create();
    m_d->vertexColorBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_d->vertexIndexBuffer.create();
    m_d->vertexIndexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_d->vertexTexCoordBuffer.create();
    m_d->vertexTexCoordBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    QPixmap pixmap(m_d->fontMetrics.width(m_d->text), m_d->fontMetrics.height());
    pixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pixmap);
    // painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing);
    painter.setFont(m_d->font);
    painter.setPen(Qt::white);
    painter.drawText(0, m_d->fontMetrics.ascent(), m_d->text );
    painter.end();

    QImage img( pixmap.toImage() );
    img = img.convertToFormat(QImage::Format_ARGB32);
    m_functions.glGenTextures(1, &(m_d->textureId));
    m_functions.glBindTexture(GL_TEXTURE_2D, m_d->textureId);
    m_functions.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_functions.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    m_functions.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_functions.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_functions.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits() );

    if( hasVAOSupport ){
        // Disegno contorno esterno
        m_d->vertexArrayObject.create();
        m_d->vertexArrayObject.bind();

        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.allocate(vertexData, 4 * 3 * sizeof(GLdouble));
        program->enableAttributeArray("vertexPosition");
        program->setAttributeBuffer("vertexPosition", GL_DOUBLE, 0, 3 );

        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.allocate(colorData, 4 * 4 * sizeof(GLfloat));
        program->enableAttributeArray("vertexColor");
        program->setAttributeBuffer("vertexColor", GL_FLOAT, 0,  4);

        m_d->vertexTexCoordBuffer.bind();
        m_d->vertexTexCoordBuffer.allocate(texCoord, 4 * 2 * sizeof(GLfloat));
        program->enableAttributeArray("vertexTexCoord");
        program->setAttributeBuffer("vertexTexCoord", GL_FLOAT, 0, 2);

        m_d->vertexIndexBuffer.bind();
        m_d->vertexIndexBuffer.allocate(indexData, 4 * sizeof(GLubyte));
        // causa segfault
        // m_d->vertexIndexBuffer.release();

        m_d->vertexArrayObject.release();
    } else {
        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.allocate(vertexData, 4 * 3 * sizeof(GLdouble));
        m_d->vertexPositionBuffer.release();

        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.allocate(colorData, 4 * 4 * sizeof(GLfloat));
        m_d->vertexColorBuffer.release();

        m_d->vertexIndexBuffer.bind();
        m_d->vertexIndexBuffer.allocate(indexData, 4 * sizeof(GLubyte));
        m_d->vertexIndexBuffer.release();

        m_d->vertexTexCoordBuffer.bind();
        m_d->vertexTexCoordBuffer.allocate(texCoord, 4 * 2 * sizeof(GLfloat));
        m_d->vertexTexCoordBuffer.release();
    }
    m_d->wasInitialized = true;
    m_updatePending = false;
}



void GLText2D::updateGLData( QMatrix4x4 &matrixView, double zoom ) {
    // coordinate punti
    if( m_d->vertexPositionBuffer.isCreated() ){
        // Vertici
        QRectF bRect = m_d->fontMetrics.boundingRect( m_d->text );
        QVector4D p( float(m_d->x[0]), float(m_d->x[1]), float(m_d->x[2]), float(1.0) );
        p = matrixView * p;

        double l = bRect.left() * zoom ;
        double r = bRect.right() * zoom;
        double t = bRect.bottom() * zoom;
        double b = bRect.top() * zoom;

        GLdouble vertexData[] = {
            GLdouble(p.x()+l), GLdouble(p.y()+b), GLdouble(p.z()),
            GLdouble(p.x()+r), GLdouble(p.y()+b), GLdouble(p.z()),
            GLdouble(p.x()+l), GLdouble(p.y()+t), GLdouble(p.z()),
            GLdouble(p.x()+r), GLdouble(p.y()+t), GLdouble(p.z())
        };

        m_d->vertexPositionBuffer.bind();
        m_d->vertexPositionBuffer.write(0, vertexData, 4 * 3 * sizeof(GLdouble) );
        m_d->vertexPositionBuffer.release();
    }

    // Colore contorno
    GLfloat colorData[] = {
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f,
        GLfloat(m_color->redF()), GLfloat(m_color->greenF()), GLfloat(m_color->blueF()), 1.0f
    };
    if( m_d->vertexColorBuffer.isCreated() ){
        m_d->vertexColorBuffer.bind();
        m_d->vertexColorBuffer.write(0, colorData, 4 * 4 * sizeof(GLfloat));
        m_d->vertexColorBuffer.release();
    }
    m_updatePending = false;
}

void GLText2D::boundingBoxGL(QVector4D bounds[2], QMatrix4x4 &mView ) {
    QVector4D PClip( m_d->x[0], m_d->x[1], m_d->x[2], 1.0);
    PClip = mView * PClip;
    bounds[0] = PClip;
    bounds[1] = PClip;
}

void GLText2D::paint(QPainter * painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height){
    painter->save();

    painter->setFont( m_d->printFont );
    QPointF PClip = project(  m_d->x[0], m_d->x[1], m_d->x[2], matrixProj, matrixView, width, height );
    QFontMetricsF fm( m_d->printFont );
    painter->drawText( PClip.x(), PClip.y(), m_d->text );

    painter->restore();
}
