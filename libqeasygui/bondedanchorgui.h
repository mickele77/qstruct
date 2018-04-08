#ifndef BONDEDANCHORGUI_H
#define BONDEDANCHORGUI_H

class UnitMeasure;
class BondedAnchorGUIPrivate;

#include <QWidget>

class BondedAnchorGUI : public QWidget
{
    Q_OBJECT

public:
    explicit BondedAnchorGUI(UnitMeasure * ump, QWidget *parent = 0);
    ~BondedAnchorGUI();

private:
    BondedAnchorGUIPrivate * m_d;
    void connectVars();
};

#endif // BONDEDANCHORGUI_H
