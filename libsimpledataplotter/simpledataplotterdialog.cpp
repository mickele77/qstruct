#include "simpledataplotterdialog.h"

#include "ui_simpledataplotterdialog.h"

#include "simpledataplotter.h"

class SimpleDataPlotterDialogPrivate{
public:
    SimpleDataPlotterDialogPrivate():
        ui( new Ui::SimpleDataPlotterDialog() ){
    }
    Ui::SimpleDataPlotterDialog * ui;
};

SimpleDataPlotterDialog::SimpleDataPlotterDialog( const QString &title, QWidget *parent ) :
    QDialog(parent),
    m_d( new SimpleDataPlotterDialogPrivate() ){
    m_d->ui->setupUi( this );
    setWindowTitle( title );
}

void SimpleDataPlotterDialog::setCurveData(int id, const QVector<QPointF> &data){
    m_d->ui->dataPlotter->setCurveData( id, data );
}

void SimpleDataPlotterDialog::setCurveData(int id,
                                           const QVector<QPointF> &data,
                                           Qt::PenStyle ps,
                                           const QColor & c ){
    m_d->ui->dataPlotter->setCurveData( id, data, ps, c );
}

void SimpleDataPlotterDialog::removeCurve(int id){
    m_d->ui->dataPlotter->removeCurve( id );
}

void SimpleDataPlotterDialog::setXAxisInverted( bool v ){
    m_d->ui->dataPlotter->setXAxisInverted( v );
}

void SimpleDataPlotterDialog::setYAxisInverted( bool v ){
    m_d->ui->dataPlotter->setYAxisInverted( v );
}
