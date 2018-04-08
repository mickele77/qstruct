#include "simpledataplotter.h"

#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPixmap>
#include <QToolButton>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFileDialog>
#include <QTextStream>
#include <QPolygonF>
#include <QPointF>
#include <QRect>
#include <QVector>
#include <QMap>

#include <cmath>
#include <cfloat>

class ZoomSettings {
public:
    ZoomSettings():
        m_minX( 0.0 ),
        m_maxX( 0.0 ),
        m_minY( 0.0 ),
        m_maxY( 0.0 ) {
    }

    ZoomSettings( const ZoomSettings& other ):
        m_minX( other.m_minX ),
        m_maxX( other.m_maxX ),
        m_minY( other.m_minY ),
        m_maxY( other.m_maxY ){
    }

    ZoomSettings & operator= (const ZoomSettings & other){
        if( this != &other ){
            m_minX = other.m_minX;
            m_maxX = other.m_maxX;
            m_minY = other.m_minY;
            m_maxY = other.m_maxY;
        }
        return *this;
    }

    void join( const ZoomSettings & plot ){
        join( plot.m_minX, plot.m_maxX, plot.m_minY, plot.m_maxY);
    }

    void join( double minX, double maxX, double minY, double maxY ){
        if(minX < m_minX) {
            m_minX = m_minX;
        }
        if( maxX > m_maxX) {
            m_maxX = maxX;
        }
        if( minY < m_minY) {
            m_minY = minY;
        }
        if( maxY > m_maxY) {
            m_maxY = maxY;
        }
    }

    double spanX( bool inverted = false ) const{
        double ret = m_maxX - m_minX;
        if( ret == 0.0 ){
            return 1.0;
        }
        return inverted? (-ret) : ret;
    }

    double spanY( bool inverted = false ) const {
        double ret = m_maxY - m_minY;
        if( ret == 0.0 ){
            ret = 1.0;
        }
        return inverted ? (-ret): ret;
    }

    double minX( bool inverted = false ) const{
        return inverted? m_maxX: m_minX;
    }

    double maxX( bool inverted = false ) const {
        return inverted? m_minX: m_maxX;
    }

    double minY( bool inverted = false ) const{
        return inverted? m_maxY: m_minY;
    }

    double maxY( bool inverted = false ) const{
        return inverted? m_minY: m_maxY;
    }

    void setLimits( double minX, double maxX, double minY, double maxY, bool Xinverted = false, bool Yinverted = false ){
        setMinX( minX, Xinverted );
        setMaxX( maxX, Xinverted );
        setMinY( minY, Yinverted );
        setMaxY( maxY, Yinverted );
    }

    void setMinX( double v, bool inverted = false ){
        if( inverted ){
            m_maxX = v;
        } else {
            m_minX = v;
        }
    }

    void setMaxX( double v, bool inverted = false ){
        if( inverted ){
            m_minX = v;
        } else {
            m_maxX = v;
        }
    }

    void setMinY( double v, bool inverted = false ){
        if( inverted ){
            m_maxY = v;
        } else {
            m_minY = v;
        }
    }

    void setMaxY( double v, bool inverted = false ){
        if( inverted ){
            m_minY = v;
        } else {
            m_maxY = v;
        }
    }

private:
    double m_minX;
    double m_maxX;
    double m_minY;
    double m_maxY;
};

class PlotData{
public:
    PlotData(){
    }
    QVector<QPointF> curve;
    QPen pen;
};

class SDPlotterPrivate{
public:
    SDPlotterPrivate( QWidget * parent, bool intAct ):
        interactive(intAct) {
        panIsActive = false;

        if( interactive ){
            zoomWindowButton = new QToolButton(parent);
            zoomWindowButton->setIcon(QIcon(":/images/zoomwin.svg"));
            zoomWindowButton->adjustSize();
            zoomWinIsActive = false;
            zoomWinRBIsShown = false;

            zoomAllButton = new QToolButton(parent);
            zoomAllButton->setIcon(QIcon(":/images/zoomall.svg"));
            zoomAllButton->adjustSize();

            zoomUndoButton = new QToolButton(parent);
            zoomUndoButton->setIcon(QIcon(":/images/zoomundo.svg"));
            zoomUndoButton->adjustSize();

            zoomRedoButton = new QToolButton(parent);
            zoomRedoButton->setIcon(QIcon(":/images/zoomredo.svg"));
            zoomRedoButton->adjustSize();

            printButton = new QToolButton(parent);
            printButton->setIcon(QIcon(":/images/print.svg"));
            printButton->adjustSize();

            readFileButton = new QToolButton(parent);
            readFileButton->setIcon(QIcon(":/images/openfile.svg"));
            readFileButton->adjustSize();

            buttonStack << zoomAllButton << zoomUndoButton << zoomRedoButton << zoomWindowButton << printButton << readFileButton;
        }
        calcMaxButtonSize();

        printFont = QFont("Arial", 12);

        // 5 pyxel
        margin = 5;
        buttonSpace = 2;
        // 5.0 mm
        printMargin = 5.0;

        numXTicks = 10;
        numYTicks = 10;

        xAxisInverted = false;
        yAxisInverted = false;

        resetAll();
    }

    int leftMargin(){
        if( m_resetLeftMargin ){
            QVector<double> xVal = xGridValues();
            QVector<double> yVal = yGridValues();

            QFontMetrics fm( font );
            int maxXWidth = 0;
            for( int i=0; i < xVal.size(); ++i){
                if( fm.width(QString::number(xVal.at(i))) > maxXWidth ){
                    maxXWidth = fm.width(QString::number(xVal.at(i)));
                }
            }
            int maxYWidth = 0;
            for( int i=0; i < yVal.size(); ++i){
                if( fm.width(QString::number(yVal.at(i))) > maxYWidth ){
                    maxYWidth = fm.width(QString::number(yVal.at(i)));
                }
            }
            if( int(0.5 * maxXWidth) > maxYWidth+margin ){
                maxYWidth = int(0.5 * maxXWidth);
            } else {
                maxYWidth += margin;
            }
            m_leftMargin = margin + maxYWidth;
            m_resetLeftMargin = false;
        }
        return m_leftMargin;
    }
    int leftPrintMargin( const QFont & f ){
        QVector<double> xVal = xGridValues();
        QVector<double> yVal = yGridValues();

        QFontMetrics fm( f );
        int maxXWidth = 0;
        for( int i=0; i < xVal.size(); ++i){
            if( fm.width(QString::number(xVal.at(i))) > maxXWidth ){
                maxXWidth = fm.width(QString::number(xVal.at(i)));
            }
        }
        int maxYWidth = 0;
        for( int i=0; i < yVal.size(); ++i){
            if( fm.width(QString::number(yVal.at(i))) > maxYWidth ){
                maxYWidth = fm.width(QString::number(yVal.at(i)));
            }
        }
        if( int(0.5 * maxXWidth) > maxYWidth+printMargin ){
            maxYWidth = int(0.5 * maxXWidth);
        } else {
            maxYWidth += printMargin;
        }
        return printMargin + maxYWidth;
    }
    int rightMargin(){
        if( m_resetRightMargin ){
            m_rightMargin = margin;
            m_resetRightMargin = true;
        }
        return m_rightMargin;
    }
    int rightPrintMargin(){
        return printMargin;
    }
    int bottomMargin(){
        if( m_resetBottomMargin ){
            QFontMetrics fm( font );
            m_bottomMargin = margin + fm.height() + margin;
            m_resetBottomMargin = false;
        }
        return m_bottomMargin;
    }
    int bottomPrintMargin( const QFont & f ){
        QFontMetrics fm( f );
        return printMargin + fm.height() + printMargin;
    }
    int topMargin( bool showButtons ){
        if( showButtons ){
            if( m_resetTopMargin ){
                m_topMargin = buttonSpace + maxButtonHeight + buttonSpace;
                m_resetTopMargin = false;
            }
            return m_topMargin;
        } else {
            return margin;
        }
    }
    int topPrintMargin(){
        return printMargin;
    }

    QVector<double> xGridValues(){
        if( m_resetXGridValues ){
            m_xGridValues.clear();
            if( zoomSettingsStack.size() > 0 ){
                ZoomSettings  settings = zoomSettingsStack.at(curZoomSettings);
                double magnitude = std::pow(10.0, roundf(std::log10( settings.spanX())) );
                double D = magnitude / numXTicks;
                D = std::ceil(settings.spanX() / numXTicks / D) * D;
                double v = 0.0;
                v = floor(settings.minX() / D ) * D;
                while( v < settings.minX() ){
                    v += D;
                }
                while( v <= settings.maxX() ){
                    m_xGridValues << v;
                    v += D;
                }
            }
            m_resetXGridValues = false;
        }
        return m_xGridValues;
    }

    QVector<double> yGridValues(){
        if( m_resetYGridValues ){
            m_yGridValues.clear();
            if( zoomSettingsStack.size() > 0 ){
                ZoomSettings  settings = zoomSettingsStack.at(curZoomSettings);
                double magnitude = std::pow(10.0, roundf(std::log10( settings.spanY())) );
                double D = magnitude / numYTicks;
                D = std::ceil(settings.spanY() / numYTicks / D) * D;
                double v = 0.0;
                v = floor(settings.minY() / D ) * D;
                while( v < settings.minY() ){
                    v += D;
                }
                while( v <= settings.maxY() ){
                    m_yGridValues << v;
                    v += D;
                }
            }
            m_resetYGridValues = false;
        }
        return m_yGridValues;
    }
    int minHeight( const QFont & font ){
        QVector<double> yVal = yGridValues();
        QFontMetrics fm( font );
        int ret = 0;
        if( interactive ){
            ret = margin + fm.height() + margin + ( yVal.size() - 1) * fm.height() + buttonSpace + maxButtonHeight + buttonSpace;
        } else {
            ret = margin + fm.height() + margin + int( ( yVal.size() - 0.5) * fm.height()) + margin;
        }
        if( ret < 150 ){
            ret = 150;
        }
        return ret;
    };
    int minWidth( const QFont & font  ){
        QVector<double> xVal = xGridValues();
        QVector<double> yVal = yGridValues();

        QFontMetrics fm( font );
        int maxXWidth = 0;
        for( int i=0; i < xVal.size(); ++i){
            if( (1.4*fm.width(QString::number(xVal.at(i)))) > maxXWidth ){
                maxXWidth = 1.4*fm.width(QString::number(xVal.at(i)));
            }
        }
        int maxYWidth = 0;
        for( int i=0; i < yVal.size(); ++i){
            if( fm.width(QString::number(yVal.at(i))) > maxYWidth ){
                maxYWidth = fm.width(QString::number(yVal.at(i)));
            }
        }
        if( int(0.5 * maxXWidth) > maxYWidth+margin ){
            maxYWidth = int(0.5 * maxXWidth);
        } else {
            maxYWidth += margin;
        }
        int ret = margin + maxYWidth + int( (xVal.size() - 0.5) * maxXWidth ) + margin;
        if( ret < 200 ){
            ret = 200;
        }
        return ret;
    }

    void resetAll(){
        m_resetXGridValues = true;
        m_resetYGridValues = true;
        m_resetLeftMargin = true;
        m_resetTopMargin = true;
        m_resetRightMargin = true;
        m_resetBottomMargin = true;
    }

    static QVector<double> yVal( double x, const QVector<QPointF> & data){
        QVector<double> ret;
        for( int i=1; i < data.size(); ++i ){
            if( (x >= data.at(i-1).x() && x < data.at(i).x()) ||
                    (x <= data.at(i-1).x() && x > data.at(i).x()) ){
                if( data.at(i-1).x() != data.at(i).x() ){
                    ret.append( data.at(i-1).y() + (data.at(i).y()-data.at(i-1).y()) / (data.at(i).x() - data.at(i-1).x()) * ( x - data.at(i-1).x()));
                } else {
                    ret.append( data.at(i-1).y() );
                }
            }
        }
        return ret;
    }

    double grabX( double x ){
        double DxRet = fabs( (zoomSettingsStack.at(curZoomSettings).maxX() - zoomSettingsStack.at(curZoomSettings).minX()) );

        if( DxRet == 0 ){
            return x;
        } else {
            double DxLim = 0.02 * DxRet;
            double xRet = x;
            QMapIterator<int, PlotData> i(curveStack);
            while (i.hasNext()) {
                i.next();
                QVector<QPointF> data = i.value().curve;
                for( int j=0; j < data.size(); ++j ){
                    if( fabs(data.at(j).x() - x) < DxRet ){
                        DxRet = fabs(data.at(j).x() - x );
                        xRet = data.at(j).x();
                    }
                }
            }
            if( DxRet < DxLim ){
                return xRet;
            } else {
                return x;
            }
        }
    }

    // flag per panning
    bool panIsActive;
    // punto partenza del panning
    QPoint panBegin;
    // flag per zoom finestra
    bool zoomWinIsActive;
    // flag per rettangolo zoom finestra
    bool zoomWinRBIsShown;
    // rettangolo dello zoom finestra
    QRect zoomWinRBRect;

    // bottoni
    QToolButton *zoomWindowButton;
    QToolButton *zoomAllButton;
    QToolButton *zoomUndoButton;
    QToolButton *zoomRedoButton;
    QToolButton *printButton;
    QToolButton *readFileButton;
    // contenitre dei bottoni
    QVector<QToolButton *> buttonStack;
    // Distanza tra bottoni
    int buttonSpace;

    // Margini per la visualizzazione a video
    int margin;
    QFont font;

    // Margini per la stampa
    double printMargin;
    QFont printFont;

    // Numero di tacche asse X
    int numXTicks;
    // Numero di tacche asse Y
    int numYTicks;

    // Contenitore delle curve
    QMap<int, PlotData> curveStack;

    // Impostazione di zoom che comprende tutto
    ZoomSettings baseSettings;
    // Contenitore delle impostazioni di zoom
    QVector<ZoomSettings> zoomSettingsStack;
    // Indice dell'impostazione corrente dello zoom
    int curZoomSettings;

    // ci dice se l'asse x ha orientamento da sx a dx (true) o da dx a sx (false)
    bool xAxisInverted;
    // ci dice se l'asse y ha orientamento da basso verso alto (true) o viceversa (false)
    bool yAxisInverted;

    // pixamp su cui viene disegnato il grafico
    QPixmap pixmap;

    // se il qidget è interattivo o meno
    bool interactive;

    int maxButtonHeight;
    int maxButtonWidth;

    QVector<QPointF> currentCoords;

private:
    void calcMaxButtonSize(){
        maxButtonHeight = 0;
        maxButtonWidth = 0;
        for( QVector<QToolButton*>::iterator i = buttonStack.begin(); i != buttonStack.end(); ++i ){
            if( (*i)->height() > maxButtonHeight )
                maxButtonHeight = (*i)->height();
            if( (*i)->width() > maxButtonWidth )
                maxButtonWidth = (*i)->width();
        }
    }

    bool m_resetXGridValues;
    bool m_resetYGridValues;
    bool m_resetLeftMargin;
    bool m_resetTopMargin;
    bool m_resetRightMargin;
    bool m_resetBottomMargin;

    QVector<double> m_xGridValues;
    QVector<double> m_yGridValues;
    int m_leftMargin;
    int m_topMargin;
    int m_rightMargin;
    int m_bottomMargin;

};

SimpleDataPlotter::SimpleDataPlotter(bool interactive, QWidget *parent) :
    QWidget(parent),
    m_d( new SDPlotterPrivate(this, interactive )) {

    m_d->font = font();

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);

    if( m_d->interactive ){
        connect(m_d->zoomAllButton, SIGNAL(clicked()), this, SLOT(zoomAll()));
        connect(m_d->zoomWindowButton, SIGNAL(clicked()), this, SLOT(zoomWindow()));
        connect(m_d->zoomUndoButton, SIGNAL(clicked()), this, SLOT(zoomUndo()));
        connect(m_d->zoomRedoButton, SIGNAL(clicked()), this, SLOT(zoomRedo()));
        connect(m_d->printButton, SIGNAL(clicked()), this, SLOT(printPreview()));
        connect(m_d->readFileButton, SIGNAL(clicked()), this, SLOT(readFromFile()));
    }

    resetPlotSettings(ZoomSettings());

    setMouseTracking( true );
}

SimpleDataPlotter::~SimpleDataPlotter() {
    delete m_d;
}

void SimpleDataPlotter::resetPlotSettings(const ZoomSettings &settings) {
    m_d->zoomSettingsStack.clear();
    m_d->zoomSettingsStack.append(settings);
    m_d->curZoomSettings = 0;
    m_d->resetAll();
    refreshPixmap();
}

void SimpleDataPlotter::setCurveData(int id, const QVector<QPointF> &data ){
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow
    };
    setCurveData( id, data, Qt::SolidLine, colorForIds[uint(id) % 6] );
}

void SimpleDataPlotter::setCurveData(int id, const QVector<QPointF> &data, Qt::PenStyle ps, const QColor & c ){

    m_d->curveStack[id].curve = data;

    QPen p( ps );
    p.setColor( c );
    m_d->curveStack[id].pen = p;

    if( data.size() > 0 ){
        QVector<QPointF>::const_iterator i = data.begin();
        double minX = i->x();
        double maxX = i->x();
        double minY = i->y();
        double maxY = i->y();
        ++i;
        for( ; i!= data.end(); ++i ){
            if( i->x() < minX )
                minX = i->x();
            if( i->x() > maxX )
                maxX = i->x();
            if( i->y() < minY )
                minY = i->y();
            if( i->y() > maxY )
                maxY = i->y();
        }
        if( m_d->curveStack.size() == 1 ){
            m_d->baseSettings.setLimits( minX, maxX, minY, maxY, m_d->xAxisInverted, m_d->yAxisInverted );
        } else {
            m_d->baseSettings.join(minX, maxX, minY, maxY);
        }
        resetPlotSettings( m_d->baseSettings );
    }

    refreshPixmap();
}

void SimpleDataPlotter::removeCurve(int id) {
    m_d->curveStack.remove(id);
    refreshPixmap();
}

QSize SimpleDataPlotter::minimumSizeHint() const {
    return QSize( m_d->minWidth(m_d->font), m_d->minHeight(m_d->font) );
}

QSize SimpleDataPlotter::sizeHint() const {
    return QSize( 2.0 * m_d->minWidth(m_d->font), 2.0 * m_d->minHeight(m_d->font) );
}

void SimpleDataPlotter::setXAxisInverted(bool v) {
    if( m_d->xAxisInverted != v ){
        m_d->xAxisInverted = v;
        refreshPixmap();
    }
}

void SimpleDataPlotter::setYAxisInverted(bool v) {
    if( m_d->yAxisInverted != v ){
        m_d->yAxisInverted = v;
        refreshPixmap();
    }
}

void SimpleDataPlotter::paintEvent(QPaintEvent * /* event */)
{
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, m_d->pixmap);

    if( m_d->interactive ){
        if( m_d->zoomWinRBIsShown ) {
            painter.setPen(palette().color(QPalette::Text ));
            painter.drawRect(m_d->zoomWinRBRect.normalized().adjusted(0, 0, -1, -1));
        }
    }

    drawCoords( &painter );

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = palette().color(QPalette::Text );
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
    }
}

void SimpleDataPlotter::resizeEvent(QResizeEvent * /* event */) {
    int x = width() - m_d->buttonSpace;
    for( QVector<QToolButton*>::iterator i = m_d->buttonStack.begin(); i != m_d->buttonStack.end(); ++i ){
        x -= (*i)->width() + m_d->buttonSpace;
    }
    for( QVector<QToolButton*>::iterator i = m_d->buttonStack.begin(); i != m_d->buttonStack.end(); ++i ){
        (*i)->move(x, m_d->buttonSpace);
        x += (*i)->width() + m_d->buttonSpace;
    }
    refreshPixmap();
}

void SimpleDataPlotter::mousePressEvent(QMouseEvent *event)
{
    if( m_d->interactive ){
        QRect rect(m_d->leftMargin(), m_d->topMargin( m_d->interactive ),
                   width() - m_d->leftMargin() - m_d->rightMargin(),
                   height() - m_d->topMargin( m_d->interactive ) - m_d->bottomMargin() );

        if (rect.contains(event->pos())) {
            if( m_d->zoomWinIsActive ){
                if( event->button() == Qt::LeftButton) {
                    if( m_d->zoomWinRBIsShown ){
                        m_d->zoomWinIsActive = false;
                        m_d->zoomWinRBIsShown = false;
                        updateRubberBandRegion( m_d->zoomWinRBRect.normalized() );
                        unsetCursor();

                        QRect rect = m_d->zoomWinRBRect.normalized();
                        if (rect.width() < 4 || rect.height() < 4)
                            return;
                        rect.translate(-m_d->leftMargin(), -m_d->topMargin( m_d->interactive ));

                        ZoomSettings prevSettings = m_d->zoomSettingsStack[m_d->curZoomSettings];
                        ZoomSettings settings;
                        double dx = prevSettings.spanX( m_d->xAxisInverted ) / (width() - m_d->leftMargin() - m_d->rightMargin() );
                        double dy = prevSettings.spanY( m_d->yAxisInverted ) / (height() - m_d->topMargin( m_d->interactive ) - m_d->bottomMargin() );
                        settings.setMinX( prevSettings.minX( m_d->xAxisInverted ) + dx * rect.left(), m_d->xAxisInverted );
                        settings.setMaxX( prevSettings.minX( m_d->xAxisInverted ) + dx * rect.right(),  m_d->xAxisInverted );
                        settings.setMinY( prevSettings.maxY(m_d->yAxisInverted) - dy * rect.bottom(), m_d->yAxisInverted );
                        settings.setMaxY( prevSettings.maxY(m_d->yAxisInverted) - dy * rect.top(), m_d->yAxisInverted );

                        m_d->zoomSettingsStack.resize(m_d->curZoomSettings + 1);
                        m_d->zoomSettingsStack.append(settings);
                        zoomRedo();
                    } else {
                        m_d->zoomWinRBIsShown = true;
                        m_d->zoomWinRBRect.setTopLeft(event->pos());
                        m_d->zoomWinRBRect.setBottomRight(event->pos());
                        setCursor(Qt::CrossCursor);
                        updateRubberBandRegion( m_d->zoomWinRBRect.normalized() );
                    }
                }
            }
            if (event->button() == Qt::MiddleButton) {
                m_d->panBegin = event->pos();
                m_d->panIsActive = true;
                ZoomSettings settings = m_d->zoomSettingsStack[m_d->curZoomSettings];
                m_d->zoomSettingsStack.resize(m_d->curZoomSettings + 1);
                m_d->zoomSettingsStack.append(settings);
                ++m_d->curZoomSettings;
            }
        }
    }
}

void SimpleDataPlotter::mouseMoveEvent(QMouseEvent *event)
{
    if( m_d->interactive ){
        if( m_d->zoomWinIsActive ){
            if (m_d->zoomWinRBIsShown) {
                updateRubberBandRegion( m_d->zoomWinRBRect.normalized() );
                m_d->zoomWinRBRect.setBottomRight(event->pos());
                updateRubberBandRegion( m_d->zoomWinRBRect.normalized() );
            }
        }
        if( m_d->panIsActive ){
            double xZoom = m_d->zoomSettingsStack[m_d->curZoomSettings - 1].spanX( m_d->xAxisInverted ) / (width() - m_d->leftMargin() - m_d->rightMargin() );
            double yZoom = m_d->zoomSettingsStack[m_d->curZoomSettings - 1].spanY( m_d->yAxisInverted ) / (height() - m_d->topMargin(m_d->interactive) - m_d->bottomMargin() );
            QPoint dP = m_d->panBegin - event->pos();
            m_d->zoomSettingsStack[m_d->curZoomSettings].setMinX( m_d->zoomSettingsStack[m_d->curZoomSettings - 1].minX(m_d->xAxisInverted) + dP.x() * xZoom, m_d->xAxisInverted );
            m_d->zoomSettingsStack[m_d->curZoomSettings].setMaxX( m_d->zoomSettingsStack[m_d->curZoomSettings - 1].maxX(m_d->xAxisInverted) + dP.x() * xZoom, m_d->xAxisInverted );
            m_d->zoomSettingsStack[m_d->curZoomSettings].setMinY( m_d->zoomSettingsStack[m_d->curZoomSettings - 1].minY(m_d->yAxisInverted) - dP.y() * yZoom, m_d->yAxisInverted );
            m_d->zoomSettingsStack[m_d->curZoomSettings].setMaxY( m_d->zoomSettingsStack[m_d->curZoomSettings - 1].maxY(m_d->yAxisInverted) - dP.y() * yZoom, m_d->yAxisInverted );
            m_d->resetAll();
            refreshPixmap();
        }
    }
    QRect rect(m_d->leftMargin(), m_d->topMargin( m_d->interactive ),
               width() - m_d->leftMargin() - m_d->rightMargin(),
               height() - m_d->topMargin( m_d->interactive ) - m_d->bottomMargin() );

    m_d->currentCoords.clear();
    if (rect.contains(event->pos())) {
        double xScreen = event->x() - rect.x();
        double x = m_d->zoomSettingsStack.at(m_d->curZoomSettings).minX( m_d->xAxisInverted) + m_d->zoomSettingsStack.at(m_d->curZoomSettings).spanX( m_d->xAxisInverted) * xScreen / rect.width();
        x = m_d->grabX(x);
        QMapIterator<int, PlotData> i(m_d->curveStack);
        while (i.hasNext()) {
            i.next();
            QVector<QPointF> data = i.value().curve;
            QVector<double> y = SDPlotterPrivate::yVal( x, data );
            for( int j=0; j < y.size(); ++j ){
                m_d->currentCoords.append( QPointF(x, y.at(j)) );
            }
        }
        update();
    }
}

void SimpleDataPlotter::mouseReleaseEvent(QMouseEvent *event) {
    if( m_d->interactive ){
        if( m_d->panIsActive ) {
            if( event->button() == Qt::MiddleButton ){
                m_d->panIsActive = false;
            }
        }
    }
}

void SimpleDataPlotter::wheelEvent(QWheelEvent *event)
{
    if( m_d->interactive ){
        double zoomFactor = 1.0 - event->delta() / 120.0 * 0.1 ;

        double w = width() - m_d->leftMargin() - m_d->rightMargin();
        double zoomX = m_d->zoomSettingsStack[m_d->curZoomSettings].spanX( m_d->xAxisInverted ) / w;
        double h = height() - m_d->topMargin(m_d->interactive) - m_d->bottomMargin();
        double zoomY = m_d->zoomSettingsStack[m_d->curZoomSettings].spanY( m_d->yAxisInverted ) / h;
        double xScreen = event->x() - m_d->leftMargin();
        double yScreen = h - (event->y() - m_d->topMargin(m_d->maxButtonHeight));
        double x = m_d->zoomSettingsStack[m_d->curZoomSettings].minX(m_d->xAxisInverted) + zoomX * xScreen;
        double y = m_d->zoomSettingsStack[m_d->curZoomSettings].minY(m_d->yAxisInverted) + zoomY * yScreen;

        // premendo Qt::ControlModifier abbiamo solo zoom orizzontale
        if( !( event->modifiers() & Qt::ControlModifier) ){
            // zoom verticale
            zoomY *= zoomFactor;
            m_d->zoomSettingsStack[m_d->curZoomSettings].setMinY( y - yScreen * zoomY, m_d->yAxisInverted );
            m_d->zoomSettingsStack[m_d->curZoomSettings].setMaxY( y + (h - yScreen) * zoomY, m_d->yAxisInverted );
        }
        // premendo Qt::ShiftModifier abbiamo solo zoom verticale
        if( !(event->modifiers() & Qt::ShiftModifier) ){
            // zoom orizzontale
            zoomX *= zoomFactor;
            m_d->zoomSettingsStack[m_d->curZoomSettings].setMinX( x - xScreen * zoomX, m_d->xAxisInverted );
            m_d->zoomSettingsStack[m_d->curZoomSettings].setMaxX( x + (w - xScreen) * zoomX, m_d->xAxisInverted );
        }
        m_d->resetAll();
        refreshPixmap();
    }
}

void SimpleDataPlotter::updateRubberBandRegion( const QRect & rect ) {
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

void SimpleDataPlotter::refreshPixmap() {
    m_d->pixmap = QPixmap(size());
    m_d->pixmap.fill( palette().color( QPalette::Base ) );

    QPainter painter(&m_d->pixmap);
    // painter.setRenderHint( QPainter::Antialiasing );
    painter.initFrom(this);
    drawGrid(&painter, width(), height(), m_d->leftMargin(), m_d->topMargin(m_d->interactive), m_d->rightMargin(), m_d->bottomMargin(), m_d->margin, m_d->font );
    drawCurves(&painter, width(), height(), m_d->leftMargin(), m_d->topMargin(m_d->interactive), m_d->rightMargin(), m_d->bottomMargin() );
    update();
}

void SimpleDataPlotter::drawGrid(QPainter *painter, int width, int height, int lm, int tm, int rm, int bm, double txtm, const QFont & f, double frameLineWidth ) {
    painter->save();
    QRect rect(lm, tm, width - lm - rm, height - tm - bm );
    if (!rect.isValid())
        return;

    ZoomSettings settings = m_d->zoomSettingsStack[m_d->curZoomSettings];
    QPen gridPen( palette().color(QPalette::Dark) );
    gridPen.setWidthF( 0.5 * frameLineWidth );
    QPen framePen( QPalette::Text );
    framePen.setWidthF( frameLineWidth );

    painter->setFont( f );

    QVector<double> xVal = m_d->xGridValues();
    for( int i=0; i<xVal.size(); ++i){
        double zoom = (rect.width() - 1) / settings.spanX(m_d->xAxisInverted);
        int x = rect.left() + zoom * (xVal.at(i)-settings.minX(m_d->xAxisInverted));
        painter->setPen(gridPen);
        painter->drawLine(x, rect.top(), x, rect.bottom());
        painter->setPen(framePen);
        painter->drawLine(x, rect.bottom(), x, rect.bottom() + int(0.8*txtm) );
        QRectF rectText(x-50, rect.bottom() + txtm, 100, 20 );
        rectText = painter->boundingRect( rectText, QString::number(xVal.at(i)), Qt::AlignHCenter | Qt::AlignTop);
        painter->drawText(rectText,
                          Qt::AlignHCenter | Qt::AlignTop,
                          QString::number(xVal.at(i)));
    }

    QVector<double> yVal = m_d->yGridValues();
    for( int i=0; i<yVal.size(); ++i){
        double zoom = (rect.height() - 1) / settings.spanY(m_d->yAxisInverted);
        int y = rect.bottom() - zoom * (yVal.at(i)-settings.minY(m_d->yAxisInverted));
        painter->setPen(gridPen);
        painter->drawLine(rect.left(), y, rect.right(), y);
        painter->setPen(framePen);
        painter->drawLine(rect.left() - int(0.8*txtm), y, rect.left(), y);
        QRectF rectText( rect.left() - lm, y - 10, lm - txtm, 20 );
        rectText = painter->boundingRect( rectText, QString::number(yVal.at(i)), Qt::AlignRight | Qt::AlignVCenter );
        painter->drawText(rectText,
                          Qt::AlignRight | Qt::AlignVCenter,
                          QString::number(yVal.at(i)) );
    }

    painter->drawRect(rect.adjusted(0, 0, -1, -1));
    painter->restore();
}

void SimpleDataPlotter::drawCurves(QPainter *painter, int width, int height, int lm, int tm, int rm, int bm, double lineWidth)
{
    painter->save();

    ZoomSettings settings = m_d->zoomSettingsStack[m_d->curZoomSettings];
    QRect rect(lm, tm, width - lm - rm, height - tm - bm);
    if (!rect.isValid())
        return;

    painter->setClipRect(rect.adjusted(+1, +1, -1, -1));

    double spanX = settings.spanX( m_d->xAxisInverted );
    double minX = settings.minX( m_d->xAxisInverted );
    double spanY = settings.spanY( m_d->yAxisInverted );
    double minY = settings.minY( m_d->yAxisInverted );
    QMapIterator<int, PlotData> i(m_d->curveStack);
    while (i.hasNext()) {
        i.next();

        QVector<QPointF> data = i.value().curve;
        QPolygonF polyline(data.count());

        for (int j = 0; j < data.count(); ++j) {
            double dx = data[j].x() - minX;
            double dy = data[j].y() - minY;
            double x = rect.left() + (dx * (rect.width() - 1) / spanX );
            double y = rect.bottom() - (dy * (rect.height() - 1) / spanY );
            polyline[j] = QPointF(x, y);
        }
        QPen p = i.value().pen;
        p.setWidthF( lineWidth );
        painter->setPen( p );
        painter->drawPolyline(polyline);
    }

    painter->restore();
}

void SimpleDataPlotter::drawCoords(QPainter *painter ) {
    if( !m_d->currentCoords.isEmpty() ){
        painter->save();
        painter->setFont( m_d->font );
        ZoomSettings settings = m_d->zoomSettingsStack[m_d->curZoomSettings];
        QRect rect( m_d->leftMargin(),  m_d->topMargin(m_d->interactive), width() - m_d->leftMargin() - m_d->rightMargin(), height() - m_d->topMargin(m_d->interactive) - m_d->bottomMargin());
        double zoomX = (rect.width() - 1) / settings.spanX(m_d->xAxisInverted);
        int x = rect.left() + zoomX * (m_d->currentCoords.at(0).x()-settings.minX(m_d->xAxisInverted));

        QPen p(Qt::DotLine );
        painter->setPen( p );
        painter->drawLine( x, rect.bottom(), x, rect.top() );

        for( int i=0; i < m_d->currentCoords.size(); ++i){
            double zoomY = (rect.height() - 1) / settings.spanY(m_d->yAxisInverted);
            int y = rect.bottom() - zoomY * (m_d->currentCoords.at(i).y()-settings.minY(m_d->yAxisInverted));
            if( rect.contains(x,y)){
                QFontMetrics fm(m_d->font);
                QRectF rectText(x, y, 100, fm.height() );
                QString txt = "(" + QString::number(m_d->currentCoords.at(i).x()) + "," + QString::number(m_d->currentCoords.at(i).y()) + ")";
                rectText = painter->boundingRect( rectText, txt, Qt::AlignLeft | Qt::AlignTop );
                painter->drawText(rectText,
                                  Qt::AlignLeft | Qt::AlignTop,
                                  txt);
            }
        }
        painter->restore();
    }
}

void SimpleDataPlotter::zoomWindow() {
    m_d->zoomWinIsActive = true;
    setCursor(Qt::CrossCursor);
}

void SimpleDataPlotter::zoomAll() {
    m_d->zoomSettingsStack.append( m_d->baseSettings );
    m_d->curZoomSettings = m_d->zoomSettingsStack.size() - 1;
    m_d->resetAll();
    refreshPixmap();
}

void SimpleDataPlotter::zoomUndo() {
    if (m_d->curZoomSettings > 0) {
        --(m_d->curZoomSettings);
        m_d->resetAll();
        refreshPixmap();
    }
}

void SimpleDataPlotter::zoomRedo() {
    if (m_d->curZoomSettings < m_d->zoomSettingsStack.count() - 1) {
        ++(m_d->curZoomSettings);
        m_d->resetAll();
        refreshPixmap();
    }
}

void SimpleDataPlotter::printPreview() {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOrientation( QPrinter::Landscape );
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowFlags ( Qt::Window );
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), this, SLOT(print(QPrinter *)));
    preview.exec();
}

void SimpleDataPlotter::print( QPrinter * printer ) {
    QPainter painter(printer);

    painter.setWindow( 0,0,printer->pageRect(QPrinter::Millimeter).width(), printer->pageRect(QPrinter::Millimeter).height() );

    QFont f = m_d->printFont;
    f.setPointSizeF( m_d->printFont.pointSizeF() * printer->pageRect(QPrinter::Millimeter).height() / printer->pageRect().height());

    drawGrid( &painter, printer->pageRect(QPrinter::Millimeter).width(), printer->pageRect(QPrinter::Millimeter).height(),
              m_d->leftPrintMargin( f ), m_d->topPrintMargin(),
              m_d->rightPrintMargin(), m_d->bottomPrintMargin(f),
              m_d->printMargin, f, 0.3);
    drawCurves( &painter, printer->pageRect(QPrinter::Millimeter).width(), printer->pageRect(QPrinter::Millimeter).height(),
                m_d->leftPrintMargin( f ) , m_d->topPrintMargin() ,
                m_d->rightPrintMargin() , m_d->bottomPrintMargin(f) ,
                0.6  );
}

void SimpleDataPlotter::readFromFile(){
    QString fileName = QFileDialog::getOpenFileName(this, trUtf8("Apri file"),
                                                    "",
                                                    tr("Files (*.*)"));
    if( !fileName.isEmpty() ){
        readFromFile( fileName);
    }
}


void SimpleDataPlotter::readFromFile( const QString &fileName) {
    QVector< QVector<QPointF> > data;
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        int nData = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList coords = line.split(QRegExp("\\s+"));
            if( nData == 0 && coords.size() > 1 ){
                nData = coords.size() - 1;
                for( int i=0; i < nData; ++i){
                    data.append( QVector<QPointF>() );
                }
            }
            if( coords.size() > 1 ) {
                double x = coords.at(0).toDouble();
                for (int i = 1; (i < coords.size()) && (i < nData+1); ++i) {
                    double y = coords.at(i).toDouble();
                    data[i-1].append(QPointF(x,y));
                }
            }
        }
    }

    int id = 0;
    for( int i = 0; i < data.size(); ++i ){
        while( m_d->curveStack.contains(id)){
            ++id;
        }
        setCurveData(id, data.at(i) );
    }
}
