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
    if( m_var ){
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
    if( m_var ){
        m_var = 0;
        clear();
    }
}

void QLabelUnitMeasure::connectVar() {
    if( m_var ){
        connect( m_var, SIGNAL(unitMeasureRichStringChanged(QString)), this, SLOT(updateText()) );
        connect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        setVisible( m_var->enabled() );
        connect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        updateText();
    }
}

void QLabelUnitMeasure::disconnectVar() {
    if( m_var ){
        disconnect( m_var, SIGNAL(unitMeasureRichStringChanged(QString)), this, SLOT(updateText()) );
        disconnect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        disconnect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        m_var = 0;
        clear();
    }
}
