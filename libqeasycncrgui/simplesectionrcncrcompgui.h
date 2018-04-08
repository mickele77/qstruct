#ifndef SIMPLESECTIONRCNCRCOMPGUI_H
#define SIMPLESECTIONRCNCRCOMPGUI_H

class UnitMeasure;
class SimpleSectionRCncrCompGUIPrivate;

#include <QWidget>

class SimpleSectionRCncrCompGUI : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleSectionRCncrCompGUI(UnitMeasure * ump, QWidget *parent = 0);
    ~SimpleSectionRCncrCompGUI();
private slots:
    void calcMChi();
    void calcNRdMRd();
    void setMRd();
    void addCncrRect();
    void delCncrRect();
    void addSteelA();
    void delSteelA();
    void setSLSValues();
private:
    SimpleSectionRCncrCompGUIPrivate * m_d;
    void connectVar();
};

#endif // SIMPLESECTIONRCNCRCOMPGUI_H
