#ifndef SOILGUI_H
#define SOILGUI_H

#include <QWidget>

class SoilGUIPrivate;
class Soil;
class UnitMeasure;

class SoilGUI : public QWidget {
    Q_OBJECT
    
public:
    explicit SoilGUI(UnitMeasure * ump, QWidget *parent = 0);
    ~SoilGUI();

public slots:
    void setSoil( Soil * );
    
private:
    SoilGUIPrivate * m_d;

    void initVar();

private slots:
    void setKPhiPrimeD();
    void setKa();
    void setKp();

    void setQLimGammaWd();
    void setQLimPhiPrimeD();
    void setQLimCPrimeD();
    void setQLimHd();
    void setQLimBPrime();
    void setQLimLInfinite();
    void setQLimLPrime();
    void setNGammaWPrime();
    void setSGammaWPrime();
    void setIGammaWPrime();
    void setBGammaWPrime();
    void setGGammaWPrime();
    void setNcPrime();
    void setScPrime();
    void setIcPrime();
    void setBcPrime();
    void setGcPrime();
    void setDcPrime();
    void setNqPrime();
    void setSqPrime();
    void setIqPrime();
    void setBqPrime();
    void setGqPrime();
    void setDqPrime();
    void setqLimPrimeD();
    void setFLimPrimeD();
};

#endif // SOILGUI_H
