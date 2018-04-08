#include "qcheckboxreadonly.h"

#include "varplus.h"

QCheckBoxReadOnly::QCheckBoxReadOnly(QWidget *parent, VarPlus * v ) :
    QCheckBox(parent),
    QWidgetPlus(v) {
    connectVar();
}

QCheckBoxReadOnly::~QCheckBoxReadOnly(){
}

void QCheckBoxReadOnly::setVar(VarPlus *v) {
    if( m_var != v ){
        disconnectVar();
        m_var = v;
        connectVar();
    }
}

void QCheckBoxReadOnly::resetVar() {
    if( m_var != NULL ){
        disconnectVar();
        m_var = NULL;
        setChecked( false );
    }
}

void QCheckBoxReadOnly::connectVar() {
    if( m_var != NULL ){
        connect( m_var, &VarPlus::readOnlyChanged, this, &QCheckBox::setChecked );
        connect( this, &QCheckBox::toggled, m_var, static_cast<void(VarPlus::*)(bool)>(&VarPlus::setReadOnlyWithEmit) );
        connect( m_var, &VarPlus::aboutToBeDeleted, this, &QCheckBoxReadOnly::resetVar );
        setVisible( m_var->enabled() );
        connect( m_var, &VarPlus::enabledChanged, this, &QCheckBox::setVisible );
        setChecked( m_var->readOnly() );
    }
}

void QCheckBoxReadOnly::disconnectVar() {
    if( m_var != NULL ){
        disconnect( m_var, &VarPlus::readOnlyChanged, this, &QCheckBox::setChecked );
        disconnect( this, &QCheckBox::toggled, m_var, static_cast<void(VarPlus::*)(bool)>(&VarPlus::setReadOnlyWithEmit) );
        disconnect( m_var, &VarPlus::aboutToBeDeleted, this, &QCheckBoxReadOnly::resetVar );
        disconnect( m_var, &VarPlus::enabledChanged, this, &QCheckBox::setVisible );
        m_var = NULL;
        setChecked( false );
    }
}
