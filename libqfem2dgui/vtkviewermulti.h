#ifndef VTKVIEWERMULTI_H
#define VTKVIEWERMULTI_H

class GLGroup;
class SplitterHandler;

class VTKViewerMultiPrivate;

#include <QWidget>
#include "vtkviewer.h"

class VTKViewerMulti : public QWidget {
    Q_OBJECT
public:
    explicit VTKViewerMulti(GLGroup *s, QWidget *parent = 0);
    ~VTKViewerMulti();

    QSize sizeHint() const;
    void resizeEvent(QResizeEvent * event);

    void keyPressEvent(QKeyEvent *e);
signals:
    void activeMatrixViewChanged( float * );
    void activeMatrixProjChanged( float * );
private slots:
    void joinDownRight(SplitterHandler *handler);
    void joinUpLeft(SplitterHandler *handler);
    void splitHorizontal(VTKViewer *glWidget);
    void splitVertical(VTKViewer *glWidget);
    void handlerDimensionChanging( int newDim);
    void setDivColor( const QColor & c );
    void setActiveWidget(VTKViewer *glWidget);
private:
    VTKViewerMultiPrivate * m_d;
    void connectSplitterHandler(SplitterHandler *handler);
    void connectGLWidget( VTKViewer * glWidget );
    void updateMinimumSize();
    int maxSplitterCountToRight(SplitterHandler *handler);
    int maxSplitterCountToDown(SplitterHandler *handler);
};

class SplitterHandlerPrivate;

class SplitterHandler: public QWidget {
    Q_OBJECT
public:
    enum SplitterType {
        Horizontal,
        Vertical
    };

    SplitterHandler(SplitterType stype, int dim, QWidget *parent = 0, bool intrn = true );
    ~SplitterHandler();
    SplitterHandler &operator =(const SplitterHandler &cp);

    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

    void updatePositionSize();

    void setHandlers( SplitterHandler * upLeft, SplitterHandler * downRight);
    SplitterType splitterType();
    int dimension();
    void setDimension( int d );
    void setColor(const QColor &c);
signals:
    void handlerChanged();
    void joinUpLeft( SplitterHandler * );
    void joinDownRight( SplitterHandler * );
    void dimensionChanging( int newDim );
    void dimensionChanged();

private slots:
    void emitJoinUpLeft();
    void emitJoinDownRight();

private:
    SplitterHandlerPrivate * m_d;
    void updateCursor();
    void updateActions();
};

class VTKViewerConnectedPrivate;

class VTKViewerConnected: public QObject {
    Q_OBJECT
public:
    VTKViewerConnected( VTKViewer * glw, SplitterHandler * up, SplitterHandler * down, SplitterHandler * left, SplitterHandler * right);
    ~VTKViewerConnected();

    SplitterHandler *handlerUp();
    SplitterHandler *handlerDown();
    SplitterHandler *handlerLeft();
    SplitterHandler *handlerRight();
    VTKViewer *glWidget();
    void setHandlerUp(SplitterHandler *handler);
    void setHandlerDown(SplitterHandler *handler);
    void setHandlerLeft(SplitterHandler *handler);
    void setHandlerRight(SplitterHandler *handler);
private slots:
    void updatePositionSize();
    void checkDimension();
private:
    VTKViewerConnectedPrivate * m_d;
};

#endif // VTKVIEWERMULTI_H
