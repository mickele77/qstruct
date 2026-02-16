#ifndef QWIDGETENUMPLUS_H
#define QWIDGETENUMPLUS_H

class EnumPlus;

#include "varplusgui_export.h"

class EXPORT_VARPLUSGUI_LIB_OPT QWidgetEnumPlus
{
public:
    QWidgetEnumPlus( EnumPlus * v=nullptr );

    EnumPlus * var();

protected:
    EnumPlus * m_var;
};

#endif // QWIDGETENUMPLUS_H
