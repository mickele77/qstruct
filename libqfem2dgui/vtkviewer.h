#ifndef VTKVIEWER_H
#define VTKVIEWER_H

class VTKViewerPrivate;

#include <QWidget>

class VTKViewer: public QWidget {
    Q_OBJECT
public:
    VTKViewer( QWidget * parent = 0 );
    
private:
    VTKViewerPrivate * m_d;

    void contextMenuEvent(QContextMenuEvent *event);

    void initActions();
    void emitSplit();
signals:
    void splitMeVertical( VTKViewer * );
    void splitMeHorizontal( VTKViewer * );
    void changeDivColor( const QColor & );
    void setActiveSignal( VTKViewer * );
};

#endif // VTKVIEWER_H
