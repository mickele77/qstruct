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
    if( m_var ){
        m_var = 0;
        clear();
    }
}

void QComboBoxEnum::connectVar() {
    if(m_var){
        for( int i=0; i < m_var->enumCount(); ++i ){
            addItem( m_var->enumStr(i) );
        }
        setCurrentIndex( m_var->valueInt() );
        connect(this, SIGNAL(currentIndexChanged(int)), m_var, SLOT(setValueInt(int)));
        connect( m_var, SIGNAL(valueIntChangedWidget(int)), this, SLOT(setCurrentIndex(int)) );
        connect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        setVisible( m_var->enabled() );
        connect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
    }
}

void QComboBoxEnum::disconnectVar() {
    if(m_var){
        disconnect(this, SIGNAL(currentIndexChanged(int)), m_var, SLOT(setValueInt(int)));
        disconnect( m_var, SIGNAL(valueIntChangedWidget(int)), this, SLOT(setCurrentIndex(int)) );
        disconnect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        disconnect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        m_var = 0;
        clear();
    }
}
