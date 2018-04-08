#ifndef DATAPLOTTER_H
#define DATAPLOTTER_H

class ZoomSettings;
class QPointF;
template <typename T> class QVector;
class QPrinter;

class SDPlotterPrivate;

#include <QWidget>

class SimpleDataPlotter : public QWidget {
    Q_OBJECT

public:
    SimpleDataPlotter(bool interactive = true, QWidget *parent = 0);
    ~SimpleDataPlotter();

    void resetPlotSettings(const ZoomSettings &settings);
    void setCurveData(int id, const QVector<QPointF> &data);
    void setCurveData(int id, const QVector<QPointF> &data, Qt::PenStyle ps, const QColor & c );
    void removeCurve(int id);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void setXAxisInverted( bool v = true );
    void setYAxisInverted( bool v = true );

    void readFromFile(const QString &fileName);
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    SDPlotterPrivate * m_d;

    void updateRubberBandRegion( const QRect & );
    void refreshPixmap();
    /**
    @param lm margine sinistro
    @param tm margine superiore
    @param rm margine destro
    @param bm margine inferiore
    @param txtm margine del testo */
    void drawGrid(QPainter *painter, int width, int height, int lm, int tm, int rm, int bm, double txtm, const QFont & f, double frameLineWidth = 1.0);
    void drawCurves(QPainter *painter, int width, int height, int lm, int tm, int rm, int bm, double lineWidth = 1.0);
    void drawCoords(QPainter *painter );

private slots:
    void zoomWindow();
    void zoomAll();
    void zoomUndo();
    void zoomRedo();
    void printPreview();
    void print(QPrinter *printer);
    void readFromFile();
};

#endif // DATAPLOTTER_H
