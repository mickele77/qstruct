#include "qlabelunitmeasure.h"

#include "varplus.h"
#include "unitmeasure.h"

QLabelUnitMeasure::QLabelUnitMeasure(QWidget *parent, VarPlus * v ) :
    QLabel(parent),
    QWidgetPlus(v) {
    connectVar();
}

QLabelUnitMeasure::~QLabelUnitMeasure(){
}

void QLabelUnitMeasure::setVar(VarPlus *v) {
    if( m_var != v ){
        disconnectVar();
        m_var = v;
        connectVar();
    }
}

void QLabelUnitMeasure::updateText()  {
    if( m_var != nullptr ){
        if( m_var->unitMeasurePointer() != NULL ){
            setText( QString("[%1]").arg(m_var->unitMeasurePointer()->richString( m_var->unitMeasureValue() )) );
        } else {
            setText( QString("[-]") );
        }
    } else {
        clear();
    }
}

void QLabelUnitMeasure::resetVar() {
    if( m_var != nullptr ){
        m_var = 0;
        clear();
    }
}

void QLabelUnitMeasure::connectVar() {
    if( m_var != nullptr ){
        connect( m_var, &VarPlus::unitMeasureRichStringChanged, this, &QLabelUnitMeasure::updateText );
        connect( m_var, &VarPlus::destroyed, this, &QLabelUnitMeasure::resetVar );
        setVisible( m_var->enabled() );
        connect( m_var, &VarPlus::enabledChanged, this, &QLabelUnitMeasure::setVisible );
        updateText();
    }
}

void QLabelUnitMeasure::disconnectVar() {
    if( m_var != nullptr ){
        disconnect( m_var, &VarPlus::unitMeasureRichStringChanged, this, &QLabelUnitMeasure::updateText );
        disconnect( m_var, &VarPlus::destroyed, this, &QLabelUnitMeasure::resetVar );
        disconnect( m_var, &VarPlus::enabledChanged, this, &QLabelUnitMeasure::setVisible );
        m_var = nullptr;
        clear();
    }
}
