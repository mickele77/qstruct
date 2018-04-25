#ifndef QCUSTOMPLOTDIALOG_H
#define QCUSTOMPLOTDIALOG_H

class QCustomPlotDialogPrivate;

#include <QDialog>

class QCustomPlotDialog : public QDialog {
    Q_OBJECT
public:
    explicit QCustomPlotDialog(const QString &title, QWidget *parent = 0);

    void setCurveData(int id, const QVector<QPointF> &data);
    void setCurveData(int id, const QVector<QPointF> &data, Qt::PenStyle ps, const QColor & c );
    void removeCurve(int id);

    void setXAxisInverted( bool v = true );
    void setYAxisInverted( bool v = true );

private:
    QCustomPlotDialogPrivate * m_d;
};

#endif // QCUSTOMPLOTDIALOG_H
