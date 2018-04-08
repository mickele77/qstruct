#include "gltext.h"

#include <cmath>
#include <iostream>
#include <QHash>
#include <QSysInfo>
#include <QPainter>
#include <QPixmap>
#include <QGLFormat>
#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include <QMatrix4x4>

#include <algorithm>

const int TEXTURE_SIZE = 256;

struct CharData
{
    GLuint textureId;
    uint width;
    uint height;
    GLfloat s[2];
    GLfloat t[2];
};

struct GLTextPrivate
{
    GLTextPrivate(const QFont &f);
    ~GLTextPrivate();

    void allocateTexture();
    CharData &createCharacter(QChar c);

    QFont font;
    QFontMetrics fontMetrics;

    QHash<ushort, CharData> characters;
    QList<GLuint> textures;

    GLint xOffset;
    GLint yOffset;

    double x[3];
    QString text;
};

GLTextPrivate::GLTextPrivate(const QFont &f)
    : font(f), fontMetrics(f), xOffset(0), yOffset(0) {
    text = "Ciao";
}

GLTextPrivate::~GLTextPrivate() {
    foreach (GLuint texture, textures)
        glDeleteTextures(1, &texture);
}

void GLTextPrivate::allocateTexture() {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    QImage image(TEXTURE_SIZE, TEXTURE_SIZE, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    image = QGLWidget::convertToGLFormat(image);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, TEXTURE_SIZE, TEXTURE_SIZE,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    textures += texture;
}

CharData &GLTextPrivate::createCharacter(QChar c)
{
    ushort unicodeC = c.unicode();
    if (characters.contains(unicodeC))
        return characters[unicodeC];

    if (textures.empty())
        allocateTexture();

    GLuint texture = textures.last();

    GLsizei width = fontMetrics.width(c);
    GLsizei height = fontMetrics.height();

    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHints(QPainter::HighQualityAntialiasing
                           | QPainter::TextAntialiasing);
    painter.setFont(font);
    painter.setPen(Qt::white);

    painter.drawText(0, fontMetrics.ascent(), c);
    painter.end();


    QImage image = QGLWidget::convertToGLFormat(pixmap.toImage());
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, width, height, GL_RGBA,
                    GL_UNSIGNED_BYTE, image.bits());

    CharData& character = characters[unicodeC];
    character.textureId = texture;
    character.width = width;
    character.height = height;
    character.s[0] = static_cast<GLfloat>(xOffset) / TEXTURE_SIZE;
    character.t[0] = static_cast<GLfloat>(yOffset) / TEXTURE_SIZE;
    character.s[1] = static_cast<GLfloat>(xOffset + width) / TEXTURE_SIZE;
    character.t[1] = static_cast<GLfloat>(yOffset + height) / TEXTURE_SIZE;

    xOffset += width;
    if (xOffset + fontMetrics.maxWidth() >= TEXTURE_SIZE)
    {
        xOffset = 1;
        yOffset += height;
    }
    if (yOffset + fontMetrics.height() >= TEXTURE_SIZE) {
        allocateTexture();
        yOffset = 1;
    }
    return character;
}

GLText::GLText(const QFont &f) :
    m_d(new GLTextPrivate(f)) {
}

GLText::~GLText() {
    delete m_d;
}

QFont GLText::font() const {
    return m_d->font;
}

QFontMetrics GLText::fontMetrics() const {
    return m_d->fontMetrics;
}

#ifdef QT_OPENGL_ES_2
void GLText::draw( QGLShaderProgram * program){
    GLint colorLocation = program->uniformLocation("color");
    program->enableAttributeArray( colorLocation );
    QVector4D col( m_color->redF(), m_color->greenF(), m_color->blueF(), m_color->alphaF() );
    program->setUniformValue( colorLocation, col );

    GLint vertexLocation = program->attributeLocation("vertex");
    program->enableAttributeArray( vertexLocation );

    QVector<QVector3D> vertices;
    vertices << QVector3D(m_d->x[0], m_d->x[1], m_d->x[2]);
    vertices << QVector3D(m_d->x[0] + m_d->fontMetrics.width(m_d->text), m_d->x[1], m_d->x[2]);
    vertices << QVector3D(m_d->x[0], m_d->x[1] + m_d->fontMetrics.height(), m_d->x[2]);
    vertices << QVector3D(m_d->x[0] + m_d->fontMetrics.width(m_d->text), m_d->x[1] + m_d->fontMetrics.height(), m_d->x[2]);
    program->setAttributeArray( vertexLocation, vertices.constData() );
    program->setUniformValue( colorLocation, *m_color );

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    program->disableAttributeArray( colorLocation );
    program->disableAttributeArray( vertexLocation );
}
#else
void GLText::draw(){
    // if( QGLFormat::openGLVersionFlags() > QGLFormat::OpenGL_Version_1_1 ){
    glColor4f( m_color->redF(), m_color->greenF(), m_color->blueF(), 1.0 );
    glLineWidth( m_width );

    glBegin(GL_LINES);
    for( int i=0; i < 2; i++){
        glVertex3d(m_d->x[3*i], m_d->x[3*i+1], m_d->x[3*i+2] );
    }
    glEnd();
}
#endif

void GLText::boundingBox(QVector4D bounds[2], QMatrix4x4 &m) {
    QList<QVector4D> p;
    p << QVector4D(m_d->x[0], m_d->x[1], m_d->x[2], 1.0);
    p << QVector4D(m_d->x[0], m_d->x[1] + m_d->fontMetrics.height(), m_d->x[2], 1.0);
    p << QVector4D(m_d->x[0] + m_d->fontMetrics.width(m_d->text), m_d->x[1], m_d->x[2], 1.0);
    p << QVector4D(m_d->x[0] + m_d->fontMetrics.width(m_d->text), m_d->x[1] + m_d->fontMetrics.height(), m_d->x[2], 1.0);
    QList<QVector4D> pm;
    pm.append( m * p.at(0));
    pm.append( m * p.at(1));
    pm.append( m * p.at(2));
    pm.append( m * p.at(3));

    QList<QVector4D>::iterator i = pm.begin();
    double minX = i->x(), minY = i->y(), minZ = i->z();
    double maxX = i->x(), maxY = i->y(), maxZ = i->z();
    for(++i; i!= pm.end(); ++i){
        if( i->x() < minX )
            minX = i->x();
        if( i->y() < minY )
            minY = i->y();
        if( i->z() < minZ )
            minZ = i->z();
        if( i->x() > maxX )
            maxX = i->x();
        if( i->y() > maxY )
            maxY = i->y();
        if( i->z() > maxZ )
            maxZ = i->z();
    }
    bounds[0] = QVector4D( minX, minY, minZ, 1.0);
    bounds[1] = QVector4D( maxX, maxY, maxZ, 1.0);
}

void GLText::draw( QPainter * painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height ){
    painter->save();

    QVector4D PClip = matrixProj * matrixView * QVector4D( m_d->x[0], m_d->x[1], m_d->x[2], 1.0);

    QPen pen;
    pen.setWidthF( m_printWidth );
    painter->setPen( pen );

    painter->restore();
}
