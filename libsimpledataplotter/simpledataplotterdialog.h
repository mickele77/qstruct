#ifndef SIMPLEDATAPLOTTERDIALOG_H
#define SIMPLEDATAPLOTTERDIALOG_H

class SimpleDataPlotterDialogPrivate;

#include <QDialog>

class SimpleDataPlotterDialog : public QDialog {
    Q_OBJECT
public:
    explicit SimpleDataPlotterDialog(const QString &title, QWidget *parent = 0);

    void setCurveData(int id, const QVector<QPointF> &data);
    void setCurveData(int id, const QVector<QPointF> &data, Qt::PenStyle ps, const QColor & c );
    void removeCurve(int id);

    void setXAxisInverted( bool v = true );
    void setYAxisInverted( bool v = true );

private:
    SimpleDataPlotterDialogPrivate * m_d;
};

#endif // SIMPLEDATAPLOTTERDIALOG_H
