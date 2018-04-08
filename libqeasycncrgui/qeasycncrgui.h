#ifndef QEASYCNCRGUI_H
#define QEASYCNCRGUI_H

class QEasyCncrGUIPrivate;
class UnitMeasure;

#include <QWidget>

class QEasyCncrGUI : public QWidget {
    Q_OBJECT
    
public:
    explicit QEasyCncrGUI(UnitMeasure *ump, QWidget *parent = 0);
    ~QEasyCncrGUI();
    
private:
    QEasyCncrGUIPrivate * m_d;

    void connectVar();
};

#endif // QEASYCNCRGUI_H
