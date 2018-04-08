#ifndef GLTEXT2D_H
#define GLTEXT2D_H

#include "glitem.h"

class QChar;
class QFont;
class QFontMetricsF;
class QString;
class QOpenGLShaderProgram;

class GLText2DPrivate;

class GLText2D: public GLItem {
public:
    explicit GLText2D(const QFont &f, double xTmp=0.0, double yTmp=0.0, double zTmp=0.0, GLGroup * p = 0 );
    virtual ~GLText2D();

    QFont font() const;
    QFontMetricsF fontMetrics() const;

    void renderGLText2D(float x, float y, const QString &text);
    void paint( QPainter * painter, QMatrix4x4 &matrixProj, QMatrix4x4 &matrixView, int width, int height );

    void paintGL(QOpenGLShaderProgram * program, bool hasVAOSupport = false );
    void paintGL(QOpenGLShaderProgram *program, QMatrix4x4 &modelView, double zoom, bool hasVAOSupport = false );

    void boundingBoxGL(QVector4D bounds[2], QMatrix4x4 & mView );

private:
    GLText2DPrivate * m_d;
    void updateGLData(QMatrix4x4 &matrixView, double zoom );
    void initializeGLData(QOpenGLShaderProgram *program, QMatrix4x4 &matrixView, double zoom, bool hasVAOSupport);
};

#endif //GLTEXT2D_H
