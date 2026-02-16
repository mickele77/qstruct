#include "qlineeditvalue.h"

#include "qstringplus.h"
#include "varplus.h"

QLineEditValue::QLineEditValue(QWidget *parent, VarPlus * v ) :
    QLineEdit(parent),
    QWidgetPlus(v) {
    setAlignment( Qt::AlignLeft );
    connectVar();
}

QLineEditValue::~QLineEditValue(){
}

void QLineEditValue::setVar(VarPlus *v) {
    if( m_var != v ){
        disconnectVar();
        m_var = v;
        QStringPlus * str = dynamic_cast<QStringPlus *>(m_var);
        if( str ){
            setAlignment( Qt::AlignLeft );
        } else {
            setAlignment( Qt::AlignRight );
        }
        connectVar();
    }
}

void QLineEditValue::update() {
    setEnabled( m_var->enabled() );
    setText( m_var->valueStr() );
    setToolTip( m_var->toolTip() );
    setReadOnly( m_var->readOnly() );
    updateReadOnly();
}

void QLineEditValue::setValue() {
    if( m_var != nullptr){
        m_var->setValue( text() );
    }
}

void QLineEditValue::resetVar() {
    if( m_var != nullptr ){
        m_var = 0;
        clear();
    }
}

void QLineEditValue::updateReadOnly() {
    if( m_var != nullptr ){
        setReadOnly( m_var->readOnly() );
        if( m_var->readOnly() ){
            setStyleSheet("* {background-color: rgb(250,235,215);}");
            return;
        }
    }
    setStyleSheet("* {background-color: white;}");
    if( m_var == nullptr ){
        setReadOnly( true );
    }
}

void QLineEditValue::connectVar() {
    if( m_var != nullptr ){
        connect( m_var, &VarPlus::valueChangedInternal, this, &QLineEditValue::setText );
        connect( this, &QLineEditValue::editingFinished, this, &QLineEditValue::setValue );
        connect( m_var, &VarPlus::readOnlyChanged, this, &QLineEditValue::updateReadOnly );
        connect( m_var, &VarPlus::destroyed, this, &QLineEditValue::resetVar );
        setVisible( m_var->enabled() );
        connect( m_var, &VarPlus::enabledChanged, this, &QLineEditValue::setVisible );
        setText( m_var->valueStr() );
        setReadOnly( m_var->readOnly() );
        updateReadOnly();
    }
}

void QLineEditValue::disconnectVar() {
    if( m_var != nullptr ){
        disconnect( m_var, &VarPlus::valueChangedInternal, this, &QLineEditValue::setText );
        disconnect( this, &QLineEditValue::editingFinished, this, &QLineEditValue::setValue );
        disconnect( m_var, &VarPlus::readOnlyChanged, this, &QLineEditValue::updateReadOnly );
        disconnect( m_var, &VarPlus::destroyed, this, &QLineEditValue::resetVar );
        disconnect( m_var, &VarPlus::enabledChanged, this, &QLineEditValue::setVisible );
        m_var = nullptr;
        clear();
        setReadOnly( true );
        updateReadOnly();
    }
}

