#ifndef QEASYGUI_H
#define QEASYGUI_H

class QEasyGUIPrivate;
class UnitMeasure;

#include <QWidget>

class QEasyGUI : public QWidget {
    Q_OBJECT
    
public:
    explicit QEasyGUI(UnitMeasure *ump, QWidget *parent = 0);
    ~QEasyGUI();
    
private:
    QEasyGUIPrivate * m_d;

    void connectVar();
};

#endif // QEASYGUI_H
