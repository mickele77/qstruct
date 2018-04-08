#ifndef TSCONNECTION_H
#define TSCONNECTION_H

#include "timberconnection.h"

class TSShearPlanePlus;
class Steel;

class TSConnection : public TimberConnection
{
    Q_OBJECT
public:
    enum ShearPlane {
      SingleShear,
      DoubleShearCentralSteel,
      DoubleShearOuterSteel
    };

    TSConnection( UnitMeasure * ump,
                  Timber * timb,
                  Steel * st,
                  TimberConnector * con,
                  ServiceClass * sc,
                  LoadDurationClass * ld,
                  LSType * ls,
                  QObject * parent = 0 );

    Steel * steel();
    void setSteel( Steel * );

    double RkNormal();

    /** Piani di taglio */
    TSShearPlanePlus * shearPlane;

    /** Spessore acciaio */
    DoublePlus * tSteel;

    DoublePlus * Rk1A;
    DoublePlus * Rk1B;
    DoublePlus * Rk1;
    DoublePlus * Rk2A;
    DoublePlus * Rk2B;
    DoublePlus * Rk2;
    DoublePlus * Rk3;

public slots:
    void setSteelNULL();

    void setRk1A();
    void setRk1B();
    void setRk1();
    void setRk2A();
    void setRk2B();
    void setRk2();
    void setRk3();

private:
    Steel * m_steel;

    /** Inizializza gli attributi pubblici (del tipo VarPlus) presenti nella classe */
    void initVar();

private slots:
    void setRkVisible();

signals:
    void steelChanged();
};


#include "varpluscombobox.h"

class TSShearPlanePlus : public VarPlusComboBox
{
    Q_OBJECT
public:
    TSShearPlanePlus( TSConnection::ShearPlane tt,
                      const QString & nn,
                      VarPlus::EditMode em);

    QString typeVar();

    TSShearPlanePlus & operator=( TSShearPlanePlus &);
    VarPlus & operator=( VarPlus &);

    TSConnection::ShearPlane value();
    TSConnection::ShearPlane valueNormal();
    QString valueNormalStr();

public slots:
    void setValue( TSConnection::ShearPlane );
    void setValue( int );
    void setValueNormal( TSConnection::ShearPlane );
    void setValueNormal( int );
    void setValueNormal( const QString & );

private:
    TSConnection::ShearPlane m_value;
    QList<TSConnection::ShearPlane> * m_shearPlaneContainer;

    void initDesc();
    int currentIndex();

    ~TSShearPlanePlus();
};

#endif // TSCONNECTION_H
