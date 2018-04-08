#include "qcheckboxbool.h"

#include "boolplus.h"
#include "varplus.h"

QCheckBoxBool::QCheckBoxBool(QWidget *parent, VarPlus * v ) :
    QCheckBox(parent),
    QWidgetPlus(v) {
    connectVar();
}

QCheckBoxBool::~QCheckBoxBool(){
}

void QCheckBoxBool::setVar(VarPlus *v) {
    BoolPlus * vbool = dynamic_cast<BoolPlus *>(v);
    if( vbool ){
        if( m_var != vbool ){
            disconnectVar();
            m_var = vbool;
            connectVar();
        }
    }
}

void QCheckBoxBool::resetVar() {
    if( m_var ){
        m_var = 0;
        setChecked( false );
    }
}

void QCheckBoxBool::connectVar() {
    BoolPlus * vbool = dynamic_cast<BoolPlus *>(m_var);
    if( vbool ){
        connect( vbool, SIGNAL(valueChangedBool(bool)), this, SLOT(setChecked(bool)) );
        connect( this, SIGNAL(toggled(bool)), vbool, SLOT(setValueNormal(bool)) );
        connect( vbool, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        setVisible( vbool->enabled() );
        connect( vbool, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        setChecked( vbool->value() );
    }
}

void QCheckBoxBool::disconnectVar() {
    BoolPlus * vbool = dynamic_cast<BoolPlus *>(m_var);
    if( vbool ){
        disconnect( vbool, SIGNAL(valueChanged(QString)), this, SLOT(setChecked(bool)) );
        disconnect( this, SIGNAL(toggled(bool)), vbool, SLOT(setReadOnly(bool)) );
        disconnect( vbool, SIGNAL(destroyed()), this, SLOT(resetVar()) );
        disconnect( vbool, SIGNAL(enabledChanged(bool)), this, SLOT(setVisible(bool)) );
        m_var = 0;
        setChecked( false );
    }
}
