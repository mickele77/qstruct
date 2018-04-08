#include "qwidgetenumplus.h"

QWidgetEnumPlus::QWidgetEnumPlus(EnumPlus *v):
    m_var(v){
}

EnumPlus *QWidgetEnumPlus::var() {
    return m_var;
}
