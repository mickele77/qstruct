#include "qcustomplotdialog.h"

#include "ui_qcustomplotdialog.h"

#include "qcustomplot.h"

#include <QPen>
#include <QMap>

class QCustomPlotDialogPrivate{
public:
    QCustomPlotDialogPrivate():
        ui( new Ui::QCustomPlotDialog() ){
    }
    Ui::QCustomPlotDialog * ui;
    QMap<int, QCPCurve *> curves;
};

QCustomPlotDialog::QCustomPlotDialog( const QString &title, QWidget *parent ) :
    QDialog(parent),
    m_d( new QCustomPlotDialogPrivate() ){
    m_d->ui->setupUi( this );
    setWindowTitle( title );
}

void QCustomPlotDialog::setCurveData(int id, const QVector<QPointF> &data) {
    setCurveData(id, data, Qt::SolidLine, QColor(Qt::blue) );
}

void QCustomPlotDialog::setCurveData(int id,
                                     const QVector<QPointF> &data,
                                     Qt::PenStyle ps,
                                     const QColor & c ){
    // create empty curve objects:
    QCPCurve *curve = new QCPCurve(m_d->ui->dataPlot->xAxis, m_d->ui->dataPlot->yAxis);

    // pass the data to the curves; we know t (i in loop above) is ascending, so set alreadySorted=true (saves an extra internal sort):
    int pointCount = data.size();
    QVector<QCPCurveData> curveData( pointCount );
    for (int i=0; i<pointCount; ++i)     {
        curveData[i] = QCPCurveData(i, data.at(i).x(), data.at(i).y() );
    }
    curve->data()->set( curveData, true);

    // color the curves:
    QPen pen( c );
    pen.setStyle( ps );
    curve->setPen(pen);

    m_d->curves[id] = curve;

    // set some basic customPlot config:
    m_d->ui->dataPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_d->ui->dataPlot->axisRect()->setupFullAxesBox();
    m_d->ui->dataPlot->rescaleAxes();
}

void QCustomPlotDialog::removeCurve(int id){
    QCPCurve * curve = m_d->curves.value( id, NULL );
    if( curve != NULL ){
        m_d->ui->dataPlot->removePlottable( curve );
        m_d->curves.remove( id );
    }
}

void QCustomPlotDialog::setXAxisInverted( bool v ){
    m_d->ui->dataPlot->xAxis->setRangeReversed( v );
}

void QCustomPlotDialog::setYAxisInverted( bool v ){
    m_d->ui->dataPlot->yAxis->setRangeReversed( v );
}
