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
    if( v != m_var ) {
        BoolPlus * vbool = dynamic_cast<BoolPlus *>(v);
        if( vbool != nullptr ){
            disconnectVar();
            m_var = vbool;
            connectVar();
        }
    }
}

void QCheckBoxBool::resetVar() {
    if( m_var != nullptr ){
        disconnectVar();
        m_var = nullptr ;
        setChecked( false );
    }
}

void QCheckBoxBool::connectVar() {
    if( m_var != nullptr ){
        BoolPlus * vbool = static_cast<BoolPlus *>(m_var);
        connect( vbool, &BoolPlus::valueChangedBool, this, &QCheckBoxBool::setChecked );
        connect( this, &QCheckBoxBool::toggled, vbool, static_cast< void (BoolPlus::*) (bool) > (&BoolPlus::setValueNormal) );
        connect( vbool, &BoolPlus::destroyed, this, &QCheckBoxBool::resetVar );
        setVisible( vbool->enabled() );
        connect( vbool, &BoolPlus::enabledChanged, this, &QCheckBoxBool::setVisible );
        setChecked( vbool->value() );
    }
}

void QCheckBoxBool::disconnectVar() {
    if( m_var != nullptr ){
        BoolPlus * vbool = static_cast<BoolPlus *>(m_var);
        disconnect( vbool, &BoolPlus::valueChangedBool, this, &QCheckBoxBool::setChecked );
        disconnect( this, &QCheckBoxBool::toggled, vbool, static_cast< void (BoolPlus::*) (bool) > (&BoolPlus::setValueNormal) );
        disconnect( vbool, &BoolPlus::destroyed, this, &QCheckBoxBool::resetVar );
        disconnect( vbool, &BoolPlus::enabledChanged, this, &QCheckBoxBool::setVisible );
        m_var = nullptr;
        setChecked( false );
    }
}
