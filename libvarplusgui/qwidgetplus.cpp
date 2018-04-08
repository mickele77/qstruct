#include "qwidgetplus.h"

QWidgetPlus::QWidgetPlus(VarPlus *v):
    m_var(v){
}

void QWidgetPlus::setVar(VarPlus *v) {
    if( v != m_var ){
        m_var = v;
    }
}

VarPlus *QWidgetPlus::var() {
    return m_var;
}
