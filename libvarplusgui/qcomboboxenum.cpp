#include "qcomboboxenum.h"

#include "enumplus.h"

QComboBoxEnum::QComboBoxEnum( QWidget * parent, EnumPlus * v ):
    QComboBox( parent ),
    QWidgetEnumPlus(v){
    connectVar();
}

QComboBoxEnum::~QComboBoxEnum() {
}

void QComboBoxEnum::setVar(EnumPlus *v) {
    if( m_var != v ){
        disconnectVar();
        m_var = v;
        connectVar();
    }
}

void QComboBoxEnum::resetVar() {
    if( m_var != nullptr ){
        m_var = 0;
        clear();
    }
}

void QComboBoxEnum::connectVar() {
    if( m_var != nullptr ){
        for( int i=0; i < m_var->enumCount(); ++i ){
            addItem( m_var->enumStr(i) );
        }
        setCurrentIndex( m_var->valueInt() );
        connect(this, static_cast<void (QComboBox::*)(int)> (&QComboBoxEnum::currentIndexChanged), m_var, static_cast<void (EnumPlus::*)(int)>(&EnumPlus::setValueInt) );
        connect( m_var, &EnumPlus::valueIntChangedWidget, this, &QComboBoxEnum::setCurrentIndex );
        connect( m_var, &EnumPlus::destroyed, this, &QComboBoxEnum::resetVar);
        setVisible( m_var->enabled() );
        connect( m_var, &EnumPlus::enabledChanged, this, &QComboBoxEnum::setVisible );
    }
}

void QComboBoxEnum::disconnectVar() {
    if( m_var != nullptr ){
        disconnect(this, static_cast<void (QComboBox::*)(int)> (&QComboBoxEnum::currentIndexChanged), m_var, static_cast<void (EnumPlus::*)(int)>(&EnumPlus::setValueInt) );
        disconnect( m_var, &EnumPlus::valueIntChangedWidget, this, &QComboBoxEnum::setCurrentIndex );
        disconnect( m_var, &EnumPlus::destroyed, this, &QComboBoxEnum::resetVar);
        disconnect( m_var, &EnumPlus::enabledChanged, this, &QComboBoxEnum::setVisible );

        m_var = nullptr;
        clear();
    }
}
