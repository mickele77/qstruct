#ifndef QWIDGETPLUS_H
#define QWIDGETPLUS_H

class VarPlus;

#include "varplusgui_export.h"

class EXPORT_VARPLUSGUI_LIB_OPT QWidgetPlus
{
public:
    QWidgetPlus( VarPlus * v=nullptr );

    virtual void setVar( VarPlus * v);
    VarPlus * var();

protected:
    VarPlus * m_var;
};

#endif // QWIDGETPLUS_H
