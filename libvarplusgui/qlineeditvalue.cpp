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
    if( m_var ){
        m_var->setValue( text() );
    }
}

void QLineEditValue::resetVar() {
    if( m_var ){
        m_var = 0;
        clear();
    }
}

void QLineEditValue::updateReadOnly() {
    if( m_var != NULL ){
        setReadOnly( m_var->readOnly() );
        if( m_var->readOnly() ){
            setStyleSheet("* {background-color: rgb(250,235,215);}");
            return;
        }
    }
    setStyleSheet("* {background-color: white;}");
    if( m_var == NULL ){
        setReadOnly( true );
    }
}

void QLineEditValue::connectVar() {
    if( m_var ){
        connect( m_var, SIGNAL(valueChangedInternal(QString)), this, SLOT(setText(QString)) );
        connect( this, SIGNAL(editingFinished()), this, SLOT(setValue()) );
        connect( m_var, SIGNAL(readOnlyChanged(bool)), this, SLOT(updateReadOnly()) );
        connect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        setVisible( m_var->enabled() );
        connect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        setText( m_var->valueStr() );
        setReadOnly( m_var->readOnly() );
        updateReadOnly();
    }
}

void QLineEditValue::disconnectVar() {
    if( m_var ){
        disconnect( m_var, SIGNAL(valueChangedInternal(QString)), this, SLOT(setText(QString)) );
        disconnect( this, SIGNAL(editingFinished()), this, SLOT(setValue()) );
        disconnect( m_var, SIGNAL(readOnlyChanged(bool)), this, SLOT(updateReadOnly()) );
        disconnect( m_var, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        disconnect( m_var, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        m_var = 0;
        clear();
        setReadOnly( true );
        updateReadOnly();
    }
}

