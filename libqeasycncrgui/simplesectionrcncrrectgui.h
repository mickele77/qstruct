#ifndef SIMPLESECTIONRCNCRRECTGUI_H
#define SIMPLESECTIONRCNCRRECTGUI_H

class UnitMeasure;
class SimpleSectionRCncrRectGUIPrivate;

#include <QWidget>

class SimpleSectionRCncrRectGUI : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleSectionRCncrRectGUI(UnitMeasure * ump, QWidget *parent = 0);
    ~SimpleSectionRCncrRectGUI();
private slots:
    void calcMChi();
    void copyToClipboardMChi();
    void calcNRdMRd();
    void copyToClipboardNRdMRd();
    void setMRd();
    void addSteelA();
    void delSteelA();
    void setSLSValues();
    void setBVULS();
    void setDVULS();
    void setVULSNoSteel();
    void setVULSSteel();
    void setTULS();
private:
    SimpleSectionRCncrRectGUIPrivate * m_d;
    void connectVar();
};

#endif // SIMPLESECTIONRCNCRRECTGUI_H
