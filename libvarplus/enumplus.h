#ifndef ENUMPLUS_H
#define ENUMPLUS_H

#include "varplus.h"

/**
* @class EnumPlus
*
* @brief Classe usata per modellizzare grandezze fisiche rappresentate con valori interi finiti (enum)
*
* Questa classe viene impiegata per modellizzare grandezze fisiche rappresentabili con un singolo
* valore intero compreso tra un numero finito di alternative.
*
* @author Michele Mocciola
*
*/

class EXPORT_VARPLUS_LIB_OPT EnumPlus : public VarPlus {
    Q_OBJECT
public:
    explicit EnumPlus(const QString &nn, bool rOnly = false );
    EnumPlus( EnumPlus & );

    virtual int enumCount() = 0;
    virtual QString enumStr( int ) = 0;

    virtual int valueInt();
    virtual int valueNormalInt() = 0;

signals:
    void valueIntChanged( int );
    void valueIntChangedWidget( int );

public slots:
    virtual void setValueInt( int, bool emitAuto = true ) = 0;
};

#endif // ENUMPLUS_H
