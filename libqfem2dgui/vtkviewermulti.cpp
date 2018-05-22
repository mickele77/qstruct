#include "vtkviewermulti.h"

#include "vtkviewer.h"

#include <QResizeEvent>
#include <QAction>
#include <QMenu>

class SplitterHandlerPrivate {
public:
    SplitterHandlerPrivate(SplitterHandler::SplitterType stype, int dim, bool intrn ):
        splitterType(stype),
        dimension(dim),
        handlerUpLeft(NULL),
        handlerDownRight(NULL),
        internal( intrn),
        joinUpLeftAction(NULL),
        joinDownRightAction(NULL) {
    }
    ~SplitterHandlerPrivate(){
    }
    static int handlerWidth;
    SplitterHandler::SplitterType splitterType;
    int dimension;
    SplitterHandler * handlerUpLeft;
    SplitterHandler * handlerDownRight;
    bool internal;
    QAction * joinUpLeftAction;
    QAction * joinDownRightAction;
    int movingDimension;
    static QColor divColor;
};

int SplitterHandlerPrivate::handlerWidth = 4;
QColor SplitterHandlerPrivate::divColor = QColor( Qt::darkGray );

SplitterHandler::SplitterHandler( SplitterType stype, int dim, QWidget *parent, bool intrn ):
    QWidget(parent),
    m_d( new SplitterHandlerPrivate(stype, dim, intrn) ) {

    updateCursor();
    updateActions();

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, m_d->divColor );
    setAutoFillBackground(true);
    setPalette(Pal);
}

SplitterHandler::~SplitterHandler(){
    delete m_d;
}

SplitterHandler &SplitterHandler::operator =(const SplitterHandler &cp) {
    if( &cp != this ){
        m_d->splitterType = cp.m_d->splitterType;
        m_d->internal = cp.m_d->internal;
        updateCursor();
        updateActions();
        m_d->dimension = cp.m_d->dimension;
        setHandlers( cp.m_d->handlerUpLeft, cp.m_d->handlerDownRight );
    }
    return *this;
}

void SplitterHandler::setColor( const QColor & c ){
    if( c != m_d->divColor ){
        m_d->divColor = c;
    }
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, m_d->divColor );
    setPalette(Pal);
    update();
}

void SplitterHandler::updateActions(){
    if( m_d->internal ){
        if( m_d->joinDownRightAction == NULL ){
            m_d->joinDownRightAction = new QAction( "", this );
            connect( m_d->joinDownRightAction, &QAction::triggered, this, &SplitterHandler::emitJoinDownRight );
        }
        if( m_d->joinUpLeftAction == NULL ){
            m_d->joinUpLeftAction = new QAction( "", this );
            connect( m_d->joinUpLeftAction, &QAction::triggered, this, &SplitterHandler::emitJoinUpLeft );
        }
        if( m_d->splitterType == Vertical ){
            m_d->joinDownRightAction->setText( trUtf8("Sinistra → Destra") );
            m_d->joinUpLeftAction->setText( trUtf8("Destra → Sinistra") );
        } else if( m_d->splitterType == Horizontal ){
            m_d->joinDownRightAction->setText( trUtf8("Sopra → Sotto") );
            m_d->joinUpLeftAction->setText( trUtf8("Sotto → Sopra") );
        }
    }
}

void SplitterHandler::updateCursor(){
    if( m_d->internal ){
        if( m_d->splitterType == Vertical ) {
            setCursor( QCursor(Qt::SizeHorCursor ) );
        } else if( m_d->splitterType == Horizontal ) {
            setCursor( QCursor(Qt::SizeVerCursor ) );
        }
    } else {
        setCursor( QCursor(Qt::ArrowCursor) );
    }
}

int SplitterHandler::dimension(){
    return m_d->dimension;
}

void SplitterHandler::setDimension( int d ){
    if( d != m_d->dimension && d > 0 ){
        m_d->dimension = d;
        updatePositionSize();
        emit dimensionChanged();
        emit handlerChanged();
    }
}
void SplitterHandler::setHandlers( SplitterHandler * upLeft, SplitterHandler * downRight){
    if( m_d->handlerUpLeft != upLeft ){
        if( m_d->handlerUpLeft != NULL ){
            disconnect( m_d->handlerUpLeft, &SplitterHandler::handlerChanged, this, &SplitterHandler::updatePositionSize );
        }
        m_d->handlerUpLeft = upLeft;
        if( m_d->handlerUpLeft != NULL ){
            connect( m_d->handlerUpLeft, &SplitterHandler::handlerChanged, this, &SplitterHandler::updatePositionSize );
        }
    }
    if( m_d->handlerDownRight != downRight ){
        if( m_d->handlerDownRight != NULL ){
            disconnect( m_d->handlerDownRight, &SplitterHandler::handlerChanged, this, &SplitterHandler::updatePositionSize );
        }
        m_d->handlerDownRight = downRight;
        if( m_d->handlerDownRight != NULL ){
            connect( m_d->handlerDownRight, &SplitterHandler::handlerChanged, this, &SplitterHandler::updatePositionSize );
        }
    }
    updatePositionSize();
    emit handlerChanged();
}

SplitterHandler::SplitterType SplitterHandler::splitterType() {
    return m_d->splitterType;
}

void SplitterHandler::updatePositionSize( ){
    if( m_d->handlerDownRight != NULL && m_d->handlerUpLeft != NULL ){
        if( m_d->splitterType == Horizontal ){
            move( m_d->handlerUpLeft->m_d->dimension, m_d->dimension );
            resize( abs(m_d->handlerDownRight->m_d->dimension - m_d->handlerUpLeft->m_d->dimension) + SplitterHandlerPrivate::handlerWidth, SplitterHandlerPrivate::handlerWidth );
        } else if( m_d->splitterType == Vertical ){
            move( m_d->dimension, m_d->handlerUpLeft->m_d->dimension );
            resize( SplitterHandlerPrivate::handlerWidth, abs(m_d->handlerDownRight->m_d->dimension - m_d->handlerUpLeft->m_d->dimension) + SplitterHandlerPrivate::handlerWidth);
        }
    }
}

void SplitterHandler::contextMenuEvent(QContextMenuEvent *event) {
    if( m_d->internal ){
        QMenu menu(this);
        menu.addSection( trUtf8("Unisci viste"));
        menu.addAction( m_d->joinDownRightAction );
        menu.addAction( m_d->joinUpLeftAction );
        menu.exec(event->globalPos());
    }
}

void SplitterHandler::mousePressEvent(QMouseEvent *event) {
    if( m_d->internal ){
        if( m_d->splitterType == Horizontal ){
            m_d->movingDimension = event->globalPos().y();
        } else if( m_d->splitterType == Vertical ){
            m_d->movingDimension = event->globalPos().x();
        }
    }
}

void SplitterHandler::mouseMoveEvent(QMouseEvent *event) {
    if( m_d->internal ){
        if( m_d->splitterType == Horizontal ){
            // setDimension( m_d->dimension - (m_d->movingDimension - event->globalPos().y() ) );
            int newDim = m_d->dimension - (m_d->movingDimension - event->globalPos().y());
            emit dimensionChanging( newDim );
            m_d->movingDimension = event->globalPos().y();
        } else if( m_d->splitterType == Vertical ){
            // setDimension( m_d->dimension - (m_d->movingDimension - event->globalPos().x() ) );
            int newDim = m_d->dimension - (m_d->movingDimension - event->globalPos().x());
            emit dimensionChanging( newDim );
            m_d->movingDimension = event->globalPos().x();
        }
    }
}

void SplitterHandler::emitJoinUpLeft(){
    emit joinUpLeft( this );
}

void SplitterHandler::emitJoinDownRight(){
    emit joinDownRight( this );
}

class VTKViewerConnectedPrivate {
public:
    VTKViewerConnectedPrivate( VTKViewer * glw, SplitterHandler * up, SplitterHandler * down, SplitterHandler * left, SplitterHandler * right ):
        glWidget(glw), handlerUp(up), handlerDown(down), handlerLeft(left), handlerRight(right){
    }
    ~VTKViewerConnectedPrivate(){
    }

    VTKViewer * glWidget;
    SplitterHandler * handlerUp;
    SplitterHandler * handlerDown;
    SplitterHandler * handlerLeft;
    SplitterHandler * handlerRight;
};

VTKViewerConnected::VTKViewerConnected( VTKViewer * glw, SplitterHandler * up, SplitterHandler * down, SplitterHandler * left, SplitterHandler * right):
    m_d( new VTKViewerConnectedPrivate(glw, up, down, left, right)){
    updatePositionSize();
    connect( m_d->handlerUp, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
    connect( m_d->handlerUp, &SplitterHandler::dimensionChanged, this, &VTKViewerConnected::checkDimension);
    connect( m_d->handlerDown, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
    connect( m_d->handlerDown, &SplitterHandler::dimensionChanged, this, &VTKViewerConnected::checkDimension);
    connect( m_d->handlerLeft, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
    connect( m_d->handlerLeft, &SplitterHandler::dimensionChanged, this, &VTKViewerConnected::checkDimension);
    connect( m_d->handlerRight, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
    connect( m_d->handlerRight, &SplitterHandler::dimensionChanged, this, &VTKViewerConnected::checkDimension);
}

VTKViewerConnected::~VTKViewerConnected(){
    delete m_d;
}

void VTKViewerConnected::updatePositionSize(){
    m_d->glWidget->move( m_d->handlerLeft->dimension()+SplitterHandlerPrivate::handlerWidth, m_d->handlerUp->dimension()+SplitterHandlerPrivate::handlerWidth );
    m_d->glWidget->resize( m_d->handlerRight->dimension() - m_d->handlerLeft->dimension() - SplitterHandlerPrivate::handlerWidth,
                           m_d->handlerDown->dimension() - m_d->handlerUp->dimension() - SplitterHandlerPrivate::handlerWidth );
}

void VTKViewerConnected::checkDimension(){
    if( sender() == m_d->handlerLeft ){
        int newWidth = m_d->handlerRight->dimension() - m_d->handlerLeft->dimension() - SplitterHandlerPrivate::handlerWidth;
        if( newWidth < m_d->glWidget->minimumWidth() ){
            m_d->handlerRight->setDimension( m_d->handlerRight->dimension() + (m_d->glWidget->minimumWidth() - newWidth) );
        }
    }
    if( sender() == m_d->handlerRight ){
        int newWidth = m_d->handlerRight->dimension() - m_d->handlerLeft->dimension() - SplitterHandlerPrivate::handlerWidth;
        if( newWidth < m_d->glWidget->minimumWidth() ){
            m_d->handlerLeft->setDimension( m_d->handlerLeft->dimension() - (m_d->glWidget->minimumWidth() - newWidth) );
        }
    }
    if( sender() == m_d->handlerUp ){
        int newHeight = m_d->handlerDown->dimension() - m_d->handlerUp->dimension() - SplitterHandlerPrivate::handlerWidth;
        if( newHeight < m_d->glWidget->minimumHeight() ){
            m_d->handlerDown->setDimension( m_d->handlerDown->dimension() + (m_d->glWidget->minimumHeight() - newHeight) );
        }
    }
    if( sender() == m_d->handlerDown ){
        int newHeight = m_d->handlerDown->dimension() - m_d->handlerUp->dimension() - SplitterHandlerPrivate::handlerWidth;
        if( newHeight < m_d->glWidget->minimumHeight() ){
            m_d->handlerUp->setDimension( m_d->handlerUp->dimension() - (m_d->glWidget->minimumHeight() - newHeight) );
        }
    }
}

SplitterHandler * VTKViewerConnected::handlerUp(){
    return m_d->handlerUp;
}

void VTKViewerConnected::setHandlerUp(SplitterHandler * handler){
    if( handler != m_d->handlerUp ){
        disconnect( m_d->handlerUp, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
        m_d->handlerUp = handler;
        updatePositionSize();
        connect( m_d->handlerUp, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
    }
}

SplitterHandler * VTKViewerConnected::handlerDown(){
    return m_d->handlerDown;
}

void VTKViewerConnected::setHandlerDown(SplitterHandler * handler){
    if( handler != m_d->handlerDown ){
        disconnect( m_d->handlerDown, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
        m_d->handlerDown = handler;
        updatePositionSize();
        connect( m_d->handlerDown, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
    }
}

SplitterHandler * VTKViewerConnected::handlerLeft(){
    return m_d->handlerLeft;
}

void VTKViewerConnected::setHandlerLeft(SplitterHandler * handler){
    if( handler != m_d->handlerLeft ){
        disconnect( m_d->handlerLeft, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
        m_d->handlerLeft = handler;
        updatePositionSize();
        connect( m_d->handlerLeft, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
    }
}

SplitterHandler * VTKViewerConnected::handlerRight(){
    return m_d->handlerRight;
}

void VTKViewerConnected::setHandlerRight(SplitterHandler * handler){
    if( handler != m_d->handlerRight ){
        disconnect( m_d->handlerRight, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
        m_d->handlerRight = handler;
        updatePositionSize();
        connect( m_d->handlerRight, &SplitterHandler::handlerChanged, this, &VTKViewerConnected::updatePositionSize);
    }
}

VTKViewer * VTKViewerConnected::glWidget(){
    return m_d->glWidget;
}

class VTKViewerMultiPrivate{
public:
    VTKViewerMultiPrivate(GLGroup * s):
        scene(s),
        activeWidget(NULL){
    }
    ~VTKViewerMultiPrivate(){
    }
    GLGroup * scene;
    QList<VTKViewerConnected *> glWidgetList;
    SplitterHandler * splitterUp;
    SplitterHandler * splitterDown;
    SplitterHandler * splitterLeft;
    SplitterHandler * splitterRight;
    VTKViewer * activeWidget;
};

VTKViewerMulti::VTKViewerMulti( GLGroup * s, QWidget *parent) :
    QWidget(parent),
    m_d( new VTKViewerMultiPrivate(s) ) {

    m_d->splitterUp = new SplitterHandler( SplitterHandler::Horizontal, 0, this, false );
    m_d->splitterDown = new SplitterHandler( SplitterHandler::Horizontal, height()-SplitterHandlerPrivate::handlerWidth, this, false );
    m_d->splitterLeft = new SplitterHandler( SplitterHandler::Vertical, 0, this, false );
    m_d->splitterRight = new SplitterHandler( SplitterHandler::Vertical, width()-SplitterHandlerPrivate::handlerWidth, this, false );
    m_d->splitterUp->setHandlers( m_d->splitterLeft, m_d->splitterRight );
    m_d->splitterDown->setHandlers( m_d->splitterLeft, m_d->splitterRight );
    m_d->splitterLeft->setHandlers( m_d->splitterUp, m_d->splitterDown );
    m_d->splitterRight->setHandlers( m_d->splitterUp, m_d->splitterDown );
    connectSplitterHandler( m_d->splitterUp );
    connectSplitterHandler( m_d->splitterDown );
    connectSplitterHandler( m_d->splitterLeft );
    connectSplitterHandler( m_d->splitterRight );

    VTKViewer * glw = new VTKViewer( this );
    connectGLWidget( glw );

    m_d->glWidgetList << new VTKViewerConnected( glw, m_d->splitterUp, m_d->splitterDown, m_d->splitterLeft, m_d->splitterRight );
    updateMinimumSize();
}

VTKViewerMulti::~VTKViewerMulti() {
    delete m_d;
}

void VTKViewerMulti::connectSplitterHandler( SplitterHandler * handler ) {
    connect( handler, &SplitterHandler::joinDownRight, this, &VTKViewerMulti::joinDownRight );
    connect( handler, &SplitterHandler::joinUpLeft, this, &VTKViewerMulti::joinUpLeft );
    connect( handler, &SplitterHandler::dimensionChanging, this, &VTKViewerMulti::handlerDimensionChanging);
}

void VTKViewerMulti::connectGLWidget( VTKViewer * glWidget ) {
    connect( glWidget, &VTKViewer::splitMeHorizontal, this, &VTKViewerMulti::splitHorizontal );
    connect( glWidget, &VTKViewer::splitMeVertical, this, &VTKViewerMulti::splitVertical );
    connect( glWidget, &VTKViewer::changeDivColor, this, &VTKViewerMulti::setDivColor );
    connect( glWidget, &VTKViewer::setActiveSignal, this, &VTKViewerMulti::setActiveWidget );
}

void VTKViewerMulti::setActiveWidget( VTKViewer * glWidget ) {
    if( m_d->activeWidget != glWidget ){
        if( m_d->activeWidget != NULL ){

        }
        m_d->activeWidget = glWidget;
        if( m_d->activeWidget != NULL ){

        }
    }
}

QSize VTKViewerMulti::sizeHint() const{
    return QSize(640,480 );
}

void VTKViewerMulti::resizeEvent(QResizeEvent *event) {
    m_d->splitterRight->setDimension( event->size().width() - SplitterHandlerPrivate::handlerWidth );
    m_d->splitterDown->setDimension( event->size().height() - SplitterHandlerPrivate::handlerWidth );
}

void VTKViewerMulti::splitHorizontal( VTKViewer * glWidget) {
    VTKViewerConnected * glWidgetCon = 0;
    for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
        if( (*i)->glWidget() == glWidget ){
            glWidgetCon = (*i);
            break;
        }
    }
    if( glWidgetCon ){
        SplitterHandler * midHandler = new SplitterHandler( SplitterHandler::Horizontal,
                                                            (glWidgetCon->handlerUp()->dimension() + glWidgetCon->handlerDown()->dimension()) * 0.50,
                                                            this );
        midHandler->setHandlers( glWidgetCon->handlerLeft(), glWidgetCon->handlerRight() );
        connectSplitterHandler( midHandler );

        VTKViewer * downGLWidget = new VTKViewer( this );
        // TODO
        // (*downGLWidget) = *(glWidgetCon->glWidget() );
        connectGLWidget( downGLWidget );
        m_d->glWidgetList << new VTKViewerConnected( downGLWidget,
                                                    midHandler, glWidgetCon->handlerDown(),
                                                    glWidgetCon->handlerLeft(), glWidgetCon->handlerRight() );

        glWidgetCon->setHandlerDown( midHandler );

        midHandler->show();
        downGLWidget->show();
    }
    updateMinimumSize();
}

void VTKViewerMulti::splitVertical( VTKViewer * glWidget) {
    VTKViewerConnected * glWidgetCon = 0;
    for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
        if( (*i)->glWidget() == glWidget ){
            glWidgetCon = (*i);
            break;
        }
    }
    if( glWidgetCon ){
        SplitterHandler * midHandler = new SplitterHandler( SplitterHandler::Vertical,
                                                            (glWidgetCon->handlerLeft()->dimension() + glWidgetCon->handlerRight()->dimension()) * 0.50,
                                                            this );
        midHandler->setHandlers( glWidgetCon->handlerUp(), glWidgetCon->handlerDown() );
        connectSplitterHandler( midHandler );

        VTKViewer * rightGLWidget = new VTKViewer( this );
        // TODO
        // (*rightGLWidget) = *(glWidgetCon->glWidget() );
        connectGLWidget( rightGLWidget );
        m_d->glWidgetList << new VTKViewerConnected( rightGLWidget,
                                                    glWidgetCon->handlerUp(), glWidgetCon->handlerDown(),
                                                    midHandler, glWidgetCon->handlerRight() );

        glWidgetCon->setHandlerRight( midHandler );

        midHandler->show();
        rightGLWidget->show();
    }
    updateMinimumSize();
}

void VTKViewerMulti::joinUpLeft( SplitterHandler * handler ) {
    if( handler->splitterType() == SplitterHandler::Vertical ){
        VTKViewerConnected * glWidgetConRight = NULL;
        VTKViewerConnected * glWidgetConLeft = NULL;
        for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
            if( (*i)->handlerRight() == handler ){
                if( glWidgetConLeft == NULL ){
                    glWidgetConLeft = *i;
                } else {
                    // più di un widget a sx
                    // non possiamo unire
                    return;
                }
            }
            if( (*i)->handlerLeft() == handler ){
                if( glWidgetConRight == NULL ){
                    glWidgetConRight = *i;
                } else {
                    // più di un widget a dx
                    // non possiamo unire
                    return;
                }
            }
        }
        if( glWidgetConRight != NULL && glWidgetConLeft != NULL ){
            // abbiamo trovato un widget a sx ed uno a dx
            // possiamo proseguire
            glWidgetConRight->setHandlerLeft( glWidgetConLeft->handlerLeft() );

            // eliminiamo il glWidget
            m_d->glWidgetList.removeAll( glWidgetConLeft );
            delete glWidgetConLeft->glWidget();
            delete glWidgetConLeft;
            handler->deleteLater();
        }
    } else if( handler->splitterType() == SplitterHandler::Horizontal ){
        VTKViewerConnected * glWidgetConDown = NULL;
        VTKViewerConnected * glWidgetConUp = NULL;
        for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
            if( (*i)->handlerDown() == handler ){
                if( glWidgetConUp == NULL ){
                    glWidgetConUp = *i;
                } else {
                    // più di un widget sopra
                    // non possiamo unire
                    return;
                }
            }
            if( (*i)->handlerUp() == handler ){
                if( glWidgetConDown == NULL ){
                    glWidgetConDown = *i;
                } else {
                    // più di un widget sotto
                    // non possiamo unire
                    return;
                }
            }
        }
        if( glWidgetConDown != NULL && glWidgetConUp != NULL ){
            // abbiamo trovato un widget sopra ed uno sotto
            // possiamo proseguire
            glWidgetConDown->setHandlerUp( glWidgetConUp->handlerUp() );

            // eliminiamo il glWidget
            m_d->glWidgetList.removeAll( glWidgetConUp );
            delete glWidgetConUp->glWidget();
            delete glWidgetConUp;
            handler->deleteLater();
        }
    }
    updateMinimumSize();
}

void VTKViewerMulti::joinDownRight( SplitterHandler * handler ) {
    if( handler->splitterType() == SplitterHandler::Vertical ){
        VTKViewerConnected * glWidgetConRight = NULL;
        VTKViewerConnected * glWidgetConLeft = NULL;
        for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
            if( (*i)->handlerRight() == handler ){
                if( glWidgetConLeft == NULL ){
                    glWidgetConLeft = *i;
                } else {
                    // più di un widget a sx
                    // non possiamo unire
                    return;
                }
            }
            if( (*i)->handlerLeft() == handler ){
                if( glWidgetConRight == NULL ){
                    glWidgetConRight = *i;
                } else {
                    // più di un widget a dx
                    // non possiamo unire
                    return;
                }
            }
        }
        if( glWidgetConRight != NULL && glWidgetConLeft != NULL ){
            // abbiamo trovato un widget a sx ed uno a dx
            // possiamo proseguire
            glWidgetConLeft->setHandlerRight( glWidgetConRight->handlerRight() );

            // eliminiamo il glWidget
            m_d->glWidgetList.removeAll( glWidgetConRight );
            delete glWidgetConRight->glWidget();
            delete glWidgetConRight;
            handler->deleteLater();
        }
    } else if( handler->splitterType() == SplitterHandler::Horizontal ){
        VTKViewerConnected * glWidgetConDown = NULL;
        VTKViewerConnected * glWidgetConUp = NULL;
        for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
            if( (*i)->handlerDown() == handler ){
                if( glWidgetConUp == NULL ){
                    glWidgetConUp = *i;
                } else {
                    // più di un widget a sx
                    // non possiamo unire
                    return;
                }
            }
            if( (*i)->handlerUp() == handler ){
                if( glWidgetConDown == NULL ){
                    glWidgetConDown = *i;
                } else {
                    // più di un widget a dx
                    // non possiamo unire
                    return;
                }
            }
        }
        if( glWidgetConDown != NULL && glWidgetConUp != NULL ){
            // abbiamo trovato un widget sopra ed uno sotto
            // possiamo proseguire
            glWidgetConUp->setHandlerDown( glWidgetConDown->handlerDown() );

            // eliminiamo il glWidget
            m_d->glWidgetList.removeAll( glWidgetConDown );
            delete glWidgetConDown->glWidget();
            delete glWidgetConDown;
            handler->deleteLater();
        }
    }
    updateMinimumSize();
}

void VTKViewerMulti::handlerDimensionChanging( int newDim ){
    SplitterHandler * handler = dynamic_cast<SplitterHandler *>(sender());

    if( handler->dimension() == newDim ){
        return;
    }

    int effectiveDim = newDim;

    if( handler->splitterType() == SplitterHandler::Vertical ){
        if( newDim > handler->dimension() ){
            for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
                if( (*i)->handlerLeft() == handler ){
                    int newWidth = (*i)->handlerRight()->dimension() - effectiveDim - SplitterHandlerPrivate::handlerWidth;
                    if( newWidth < 0 ){
                        return;
                    }
                    if( newWidth < (*i)->glWidget()->minimumWidth() ){
                        effectiveDim = (*i)->handlerRight()->dimension() - (*i)->glWidget()->minimumWidth() - SplitterHandlerPrivate::handlerWidth;
                    }
                }
            }
        } else { // if( newDim < handler->dimension() )
            for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
                if( (*i)->handlerRight() == handler ){
                    int newSize = effectiveDim - (*i)->handlerLeft()->dimension() - SplitterHandlerPrivate::handlerWidth;
                    if( newSize < 0 ){
                        return;
                    }
                    if( newSize < (*i)->glWidget()->minimumWidth() ){
                        effectiveDim = (*i)->glWidget()->minimumWidth() + (*i)->handlerLeft()->dimension() + SplitterHandlerPrivate::handlerWidth;
                    }
                }
            }
        }
    }

    if( handler->splitterType() == SplitterHandler::Horizontal ){
        if( newDim > handler->dimension() ){
            for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
                if( (*i)->handlerUp() == handler ){
                    int newHeight = (*i)->handlerDown()->dimension() - effectiveDim - SplitterHandlerPrivate::handlerWidth;
                    if( newHeight < 0 ){
                        return;
                    }
                    if( newHeight < (*i)->glWidget()->minimumHeight() ){
                        effectiveDim = (*i)->handlerDown()->dimension() - (*i)->glWidget()->minimumHeight() - SplitterHandlerPrivate::handlerWidth;
                    }
                }
            }
        } else { // if( newDim < handler->dimension() )
            for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
                if( (*i)->handlerDown() == handler ){
                    int newHeight = effectiveDim - (*i)->handlerUp()->dimension() - SplitterHandlerPrivate::handlerWidth;
                    if( newHeight < 0 ){
                        return;
                    }
                    if( newHeight < (*i)->glWidget()->minimumHeight() ){
                        effectiveDim = (*i)->glWidget()->minimumHeight() + (*i)->handlerUp()->dimension() + SplitterHandlerPrivate::handlerWidth;
                    }
                }
            }
        }
    }

    if( handler->dimension() != effectiveDim && effectiveDim > 0 ){
        handler->setDimension( effectiveDim );
    }
}

void VTKViewerMulti::setDivColor(const QColor &c) {
    for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
        (*i)->handlerDown()->setColor( c );
        (*i)->handlerUp()->setColor( c );
        (*i)->handlerLeft()->setColor( c );
        (*i)->handlerRight()->setColor( c );
    }
}

void VTKViewerMulti::updateMinimumSize(){
    if( m_d->glWidgetList.size() > 0 ){
        // minimum width
        int nSplitter = maxSplitterCountToRight( m_d->splitterLeft );
        setMinimumWidth( nSplitter * m_d->glWidgetList.first()->glWidget()->minimumWidth() + (nSplitter+1) * SplitterHandlerPrivate::handlerWidth );

        // minimum height
        nSplitter = maxSplitterCountToDown( m_d->splitterUp );
        setMinimumHeight( nSplitter * m_d->glWidgetList.first()->glWidget()->minimumHeight() + (nSplitter+1) * SplitterHandlerPrivate::handlerWidth );
    } else {
        setMinimumWidth( 0 );
        setMinimumHeight( 0 );
    }
}

int VTKViewerMulti::maxSplitterCountToRight( SplitterHandler * handler ){
    if( handler == m_d->splitterRight ){
        return 0;
    }
    int max = 0;
    for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
        int maxTmp = 0;
        if( (*i)->handlerLeft() == handler ){
            if( (*i)->handlerRight() == m_d->splitterRight ){
                maxTmp = 1;
            } else {
                maxTmp = maxSplitterCountToRight((*i)->handlerRight()) + 1;
            }
        }
        if( maxTmp > max ){
            max = maxTmp;
        }
    }
    return max;
}

int VTKViewerMulti::maxSplitterCountToDown( SplitterHandler * handler ){
    if( handler == m_d->splitterDown ){
        return 0;
    }
    int max = 0;
    for( QList<VTKViewerConnected *>::iterator i = m_d->glWidgetList.begin(); i != m_d->glWidgetList.end(); ++i ){
        int maxTmp = 0;
        if( (*i)->handlerUp() == handler ){
            if( (*i)->handlerDown() == m_d->splitterDown ){
                maxTmp = 1;
            } else {
                maxTmp = maxSplitterCountToDown((*i)->handlerDown()) + 1;
            }
        }
        if( maxTmp > max ){
            max = maxTmp;
        }
    }
    return max;
}

void VTKViewerMulti::keyPressEvent(QKeyEvent *e) {
    // The following keys are forwarded by the completer to the widget
    switch (e->key()) {
    case Qt::Key_S: {
        break;
    }
    case Qt::Key_D: {
        break;
    }
    case Qt::Key_N: {
        break;
    }
    default:
        break;
    }
}
