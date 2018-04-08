#ifndef TTCONNECTION_H
#define TTCONNECTION_H

#include "timberconnection.h"

class TTShearPlanePlus;

class TTConnection : public TimberConnection
{
    Q_OBJECT
public:
    enum ShearPlane {
        SingleShear,
        DoubleShear
    };

    TTConnection( UnitMeasure * ump,
                  Timber * timb1,
                  Timber * timb2,
                  TimberConnector * con,
                  ServiceClass * sc,
                  LoadDurationClass * ld,
                  LSType * ls,
                  QObject * parent = 0 );

    Timber * timber1();
    void setTimber1( Timber * );
    Timber * timber2();
    void setTimber2( Timber * );

    double RkNormal();

    /** Piani di taglio */
    TTShearPlanePlus * shearPlane;

    /** Spessore legno */
    DoublePlus * tTimber1;
    DoublePlus * tTimber2;
    /** Valore caratteristico di rifollamento nel caso di chiodi inseriti con preforatura */
    DoublePlus * fh1k;
    DoublePlus * fh2k;
    /** Rapporto tra fh2k e fh1k */
    DoublePlus * beta;

    DoublePlus * Rk1A;
    DoublePlus * Rk1B;
    DoublePlus * Rk1C;
    DoublePlus * Rk1;
    DoublePlus * Rk2A;
    DoublePlus * Rk2B;
    DoublePlus * Rk2;
    DoublePlus * Rk3;

public slots:
    void setFh2k();
    void setBeta();
    void setKmod();

    void setTimber2NULL();

    void setRk1A();
    void setRk1B();
    void setRk1C();
    void setRk1();
    void setRk2A();
    void setRk2B();
    void setRk2();
    void setRk3();

private:
    Timber * m_timber2;

    /** Inizializza gli attributi pubblici (del tipo VarPlus) presenti nella classe */
    void initVar();

private slots:
    void setRkVisible();

signals:
    void timber2Changed();
};


#include "varpluscombobox.h"

class TTShearPlanePlus : public VarPlusComboBox
{
    Q_OBJECT
public:
    TTShearPlanePlus( TTConnection::ShearPlane tt,
                      const QString & nn,
                      VarPlus::EditMode em);

    QString typeVar();

    TTShearPlanePlus & operator=( TTShearPlanePlus &);
    VarPlus & operator=( VarPlus &);

    TTConnection::ShearPlane value();
    TTConnection::ShearPlane valueNormal();
    QString valueNormalStr();

public slots:
    void setValue( TTConnection::ShearPlane );
    void setValue( int );
    void setValueNormal( TTConnection::ShearPlane );
    void setValueNormal( int );
    void setValueNormal( const QString & );

private:
    TTConnection::ShearPlane m_value;
    QList<TTConnection::ShearPlane> * m_shearPlaneContainer;

    void initDesc();
    int currentIndex();

    ~TTShearPlanePlus();
};

#endif // TTCONNECTION_H
