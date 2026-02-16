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
        connect( this, &QRadioButtonEnum::toggled, this, &QRadioButtonEnum::setVarValue );
        connect( m_var, &EnumPlus::valueIntChangedWidget, this, &QRadioButtonEnum::setCheckedInt );
        connect( m_var, &EnumPlus::destroyed, this, &QRadioButtonEnum::resetVar );
        setVisible( m_var->enabled() );
        connect( m_var, &EnumPlus::enabledChanged, this, &QRadioButtonEnum::setVisible );
        setText( m_var->enumStr( m_value ) );
        setChecked( m_value == m_var->valueInt() );
    }
}

void QRadioButtonEnum::disconnectVar() {
    if( m_var ){
        disconnect( this, &QRadioButtonEnum::toggled, this, &QRadioButtonEnum::setVarValue );
        disconnect( m_var, &EnumPlus::valueIntChangedWidget, this, &QRadioButtonEnum::setCheckedInt );
        disconnect( m_var, &EnumPlus::destroyed, this, &QRadioButtonEnum::resetVar );
        disconnect( m_var, &EnumPlus::enabledChanged, this, &QRadioButtonEnum::setVisible );
        m_var = 0;
        setText( "" );
        setChecked( false );
    }
}
