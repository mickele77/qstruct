#ifndef STEELCONNECTIONMODULE_H
#define STEELCONNECTIONMODULE_H

class SteelConnectionModulePrivate;

#include "structmodule.h"

class SteelConnectionModule : public StructModule
{
public:
    SteelConnectionModule( UnitMeasure * um,
                           QWidget *parent = 0);

    QString name();
    QString richName();
    QIcon icon();

private:
    SteelConnectionModulePrivate * m_d;
};

#endif // STEELCONNECTIONMODULE_H
