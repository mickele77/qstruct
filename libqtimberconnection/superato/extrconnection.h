#ifndef EXTRCONNECTION_H
#define EXTRCONNECTION_H

#include "timberconnection.h"

class ExtrConnection : public TimberConnection
{
    Q_OBJECT
public:
    ExtrConnection( UnitMeasure * ump,
                    Timber * timb,
                    TimberConnector * con,
                    ServiceClass * sc,
                    LoadDurationClass * ld,
                    LSType * ls,
                    QObject * parent = 0 );

    double RkNormal();

    DoublePlus * tTimber;
    DoublePlus * f1k;
    DoublePlus * f2k;
    DoublePlus * f3k;

public slots:

    void setf1k();
    void setf2k();
    void setf3k();


private:
    /** Inizializza gli attributi pubblici (del tipo VarPlus) presenti nella classe */
    void initVar();

private slots:
    void setfkVisible();

};

#endif // EXTRCONNECTION_H
