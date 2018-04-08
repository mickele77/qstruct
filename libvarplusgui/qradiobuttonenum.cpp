#include "qradiobuttonenum.h"

#include "enumplus.h"

QRadioButtonEnum::QRadioButtonEnum(QWidget * parent, EnumPlus * v):
    QRadioButton(parent),
    QWidgetEnumPlus( v ),
    m_value(0){
    connectVar();
}

QRadioButtonEnum::~QRadioButtonEnum(){

}

void QRadioButtonEnum::setVar(EnumPlus *v, int val) {
    if( m_value != val || m_var != v ){
        disconnectVar();
        m_var = v;
        m_value = val;
        connectVar();
    }
}

void QRadioButtonEnum::setVarValue(){
    if( m_var ){
        m_var->setValueInt( m_value );
    }
}

void QRadioButtonEnum::setCheckedInt(int v ) {
    setChecked( m_value == v );
}

void QRadioButtonEnum::resetVar() {
    if( m_var ){
        m_var = 0;
        setChecked( false );
    }
}

void QRadioButtonEnum::connectVar() {
    if( m_var ){
        connect( this, SIGNAL(toggled(bool)), this, SLOT(setVarValue()) );
        connect( m_var, SIGNAL(valueIntChangedWidget(int)), this, SLOT(setCheckedInt(int)) );
        connect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        setVisible( m_var->enabled() );
        connect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        setText( m_var->enumStr( m_value ) );
        setChecked( m_value == m_var->valueInt() );
    }
}

void QRadioButtonEnum::disconnectVar() {
    if( m_var ){
        disconnect( this, SIGNAL(toggled(bool)), this, SLOT(setVarValue()) );
        disconnect( m_var, SIGNAL(valueIntChangedWidget(int)), this, SLOT(setCheckedInt(int)) );
        disconnect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        disconnect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        m_var = 0;
        setText( "" );
        setChecked( false );
    }
}
