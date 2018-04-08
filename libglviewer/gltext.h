#ifndef GLTEXT_H
#define GLTEXT_H

#include "glitem.h"

class QChar;
class QFont;
class QFontMetrics;
class QString;

class GLTextPrivate;

class GLText: public GLItem {
public:
    explicit GLText(const QFont &f);
    virtual ~GLText();

    QFont font() const;
    QFontMetrics fontMetrics() const;

    void renderGLText(float x, float y, const QString &text);
    void draw( QPainter * painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height );

#ifdef QT_OPENGL_ES_2
    void draw(QGLShaderProgram *);
#else
    void draw();
#endif

    void boundingBox( QVector4D bounds[2], QMatrix4x4 & m );

private:
      GLTextPrivate * m_d;
};

#endif //GLTEXT_H
